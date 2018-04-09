#include "OldCSInteropManager.h"
#include "Construction Set Extender_Resource.h"
#include "[Common]\OldCSInteropData.h"

namespace cse
{
#define INJECT_TIMEOUT			5000

	OldCSInteropManager*	OldCSInteropManager::Singleton = nullptr;
	const char*				OldCSInteropManager::kTempMp3Path = "templip.mp3";
	const char*				OldCSInteropManager::kTempWavPath = "templip.wav";
	const char*				OldCSInteropManager::kTempLipPath = "templip.lip";

	OldCSInteropManager* OldCSInteropManager::GetSingleton()
	{
		if (Singleton == nullptr)
			Singleton = new OldCSInteropManager();

		return Singleton;
	}

	OldCSInteropManager::OldCSInteropManager()
	{
		Loaded = false;
		DLLPath = BGSEEMAIN->GetAPPPath();
		InteropPipeHandle = INVALID_HANDLE_VALUE;

		ZeroMemory(&CS10ProcInfo, sizeof(PROCESS_INFORMATION));
		ZeroMemory(&PipeGUID, sizeof(GUID));
	}

	OldCSInteropManager::~OldCSInteropManager()
	{
		if (Loaded)
		{
			OldCSInteropData InteropDataOut(OldCSInteropData::kMessageType_Quit);
			DWORD BytesWritten = 0;

			if (!WriteFile(InteropPipeHandle,
				&InteropDataOut,
				sizeof(OldCSInteropData),
				&BytesWritten,
				nullptr) &&
				GetLastError() != ERROR_SUCCESS)
			{
				BGSEECONSOLE_ERROR("Couldn't write exit message to interop pipe!");
			}

			CloseHandle(InteropPipeHandle);
		}

		Singleton = nullptr;
	}

	bool OldCSInteropManager::CreateNamedPipeServer(char** GUIDOut)
	{
		RPC_STATUS GUIDReturn = UuidCreate(&PipeGUID),
			GUIDStrReturn = UuidToString(&PipeGUID, (RPC_CSTR*)GUIDOut);

		if ((GUIDReturn == RPC_S_OK || GUIDReturn == RPC_S_UUID_LOCAL_ONLY) && GUIDStrReturn == RPC_S_OK)
		{
	//		BGSEECONSOLE_MESSAGE("Pipe GUID = %s", *GUIDOut);
			char PipeName[0x200] = {0};
			sprintf_s(PipeName, 0x200, "\\\\.\\pipe\\{%s}", *GUIDOut);

			InteropPipeHandle = CreateNamedPipe(PipeName,
				PIPE_ACCESS_DUPLEX|FILE_FLAG_FIRST_PIPE_INSTANCE|FILE_FLAG_WRITE_THROUGH|WRITE_OWNER,
				PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE|PIPE_WAIT,
				1,
				sizeof(OldCSInteropData),
				sizeof(OldCSInteropData),
				50,
				nullptr);

			if (InteropPipeHandle == INVALID_HANDLE_VALUE)
			{
				BGSEECONSOLE_ERROR("Couldn't create interop pipe!");

				return false;
			}

			return true;
		}
		else
		{
			BGSEECONSOLE_MESSAGE("Couldn't get pipe GUID!");
			return false;
		}
	}

	bool OldCSInteropManager::InjectDLL(PROCESS_INFORMATION * info)
	{
		bool	result = false;

		HANDLE	process = OpenProcess(PROCESS_CREATE_THREAD|PROCESS_QUERY_INFORMATION|PROCESS_VM_OPERATION|PROCESS_VM_WRITE|PROCESS_VM_READ,
									FALSE,
									info->dwProcessId);

		if (process)
		{
			UInt32	hookBase = (UInt32)VirtualAllocEx(process, nullptr, 8192, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			if(hookBase)
			{
				// safe because kernel32 is loaded at the same address in all processes
				// (can change across restarts)
				UInt32	loadLibraryAAddr = (UInt32)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");

				UInt32	bytesWritten;
				WriteProcessMemory(process, (LPVOID)(hookBase + 5), DLLPath.c_str(), strlen(DLLPath.c_str()) + 1, &bytesWritten);

				UInt8	hookCode[5];

				hookCode[0] = 0xE9;
				*((UInt32 *)&hookCode[1]) = loadLibraryAAddr - (hookBase + 5);

				WriteProcessMemory(process, (LPVOID)(hookBase), hookCode, sizeof(hookCode), &bytesWritten);

				HANDLE	thread = CreateRemoteThread(process, nullptr, 0, (LPTHREAD_START_ROUTINE)hookBase, (void *)(hookBase + 5), 0, nullptr);
				if(thread)
				{
					switch(WaitForSingleObject(thread, INJECT_TIMEOUT))
					{
					case WAIT_OBJECT_0:
						result = true;
						break;
					case WAIT_ABANDONED:
						BGSEECONSOLE_MESSAGE("Process::InstallHook: waiting for thread = WAIT_ABANDONED");
						break;
					case WAIT_TIMEOUT:
						BGSEECONSOLE_MESSAGE("Process::InstallHook: waiting for thread = WAIT_TIMEOUT");
						break;
					}

					CloseHandle(thread);
				}
				else
					BGSEECONSOLE_ERROR("CreateRemoteThread failed!");

				VirtualFreeEx(process, (LPVOID)hookBase, 8192, MEM_RELEASE);
			}
			else
				BGSEECONSOLE_MESSAGE("Process::InstallHook: couldn't allocate memory in target process");

			CloseHandle(process);
		}
		else
			BGSEECONSOLE_MESSAGE("Process::InstallHook: couldn't get process handle");

		return result;
	}

	void OldCSInteropManager::DoInjectDLL(PROCESS_INFORMATION * info)
	{
		Loaded = false;

		__try
		{
			Loaded = InjectDLL(&CS10ProcInfo);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			;//
		}
	}

	bool OldCSInteropManager::Initialize()
	{
		if (Loaded)
			return true;

		char* GUIDStr = 0;
		if (!CreateNamedPipeServer(&GUIDStr))
		{
			return false;
		}

		STARTUPINFO			startupInfo = { 0 };

		startupInfo.cb = sizeof(startupInfo);

		const char	* procName = "TESConstructionSetOld.exe";
		this->DLLPath = std::string(BGSEEMAIN->GetComponentDLLPath()) + "LipSyncPipeClient.dll";

		// check to make sure the dll exists
		IFileStream	tempFile;

		if(!tempFile.Open(this->DLLPath.c_str()))
		{
			BGSEECONSOLE_MESSAGE("Couldn't find DLL (%s)!", this->DLLPath.c_str());
			RpcStringFree((RPC_CSTR*)&GUIDStr);

			return false;
		}

		Loaded = CreateProcess(procName,
							GUIDStr,	// pass the pipe guid
							nullptr,		// default process security
							nullptr,		// default thread security
							FALSE,		// don't inherit handles
							CREATE_SUSPENDED,
							nullptr,		// no new environment
							nullptr,		// no new cwd
							&startupInfo, &CS10ProcInfo) != 0;

		RpcStringFree((RPC_CSTR*)&GUIDStr);

		// check for Vista failing to create the process due to elevation requirements
		if(Loaded == false && (GetLastError() == ERROR_ELEVATION_REQUIRED))
		{
			// in theory we could figure out how to UAC-prompt for this process and then run CreateProcess again, but I have no way to test code for that
			BGSEECONSOLE_MESSAGE("Vista has decided that launching the CS 1.0 requires UAC privilege elevation. There is no good reason for this to happen, but to fix it, right-click on obse_loader.exe, go to Properties, pick the Compatibility tab, then turn on \"Run this program as an administrator\".");
			return Loaded;
		}

		if (Loaded == false)
		{
			BGSEECONSOLE_ERROR("Couldn't load CS 1.0!");
			return Loaded;
		}

		DoInjectDLL(&CS10ProcInfo);

		if (Loaded)
		{
			ResumeThread(CS10ProcInfo.hThread);
		}
		else
		{
			BGSEECONSOLE_MESSAGE("DLL injection failed. In most cases, this is caused by an overly paranoid software firewall or antivirus package. Disabling either of these may solve the problem.");

			// kill the partially-created process
			TerminateProcess(CS10ProcInfo.hProcess, 0);
		}

		// clean up
		CloseHandle(CS10ProcInfo.hProcess);
		CloseHandle(CS10ProcInfo.hThread);

		return Loaded;
	}

	bool OldCSInteropManager::GenerateLIPSyncFile(const char* InputPath, const char* ResponseText)
	{
		if (Loaded == false)
		{
			BGSEECONSOLE_MESSAGE("Interop manager not initialized!");
			return false;
		}

		BGSEECONSOLE_MESSAGE("Generating LIP file for '%s'...", InputPath);

		bool Result = false, ExitLoop = false, UsingTempWav = false;
		DWORD ByteCounter = 0;
		std::string MP3Path(InputPath), WAVPath(InputPath), LIPPath(InputPath);
		MP3Path += ".mp3", WAVPath += ".wav", LIPPath += ".lip";

		IFileStream	TempWav;
		if (TempWav.Open(WAVPath.c_str()))
			;// use the existing wav file
		else
		{
			// convert the source mp3 file to a temp wav
			WAVPath = kTempWavPath;
			UsingTempWav = true;

			IFileStream	TempMp3;
			if (TempMp3.Open(MP3Path.c_str()) == false)
			{
				// if not found, look in the archives
				if (ArchiveManager::ExtractArchiveFile(MP3Path.c_str(), kTempMp3Path) == false)
				{
					BGSEECONSOLE_MESSAGE("Couldn't find source MP3/WAV file!");
					return false;
				}
				else
					MP3Path = kTempMp3Path;
			}

			STARTUPINFO StartupInfo = { 0 };
			PROCESS_INFORMATION ProcInfo = { 0 };
			StartupInfo.cb = sizeof(StartupInfo);

			std::string FullMp3Path = std::string(BGSEEMAIN->GetAPPPath()) + std::string(MP3Path);
			std::string FullWavPath = std::string(BGSEEMAIN->GetAPPPath()) + std::string(WAVPath);

			std::string DecoderArgs(BGSEEMAIN->GetAPPPath());
			DecoderArgs += "lame.exe \"" + FullMp3Path + "\" \"" + FullWavPath + "\" --decode";

			bool Result = CreateProcess(nullptr,
										(LPSTR)DecoderArgs.c_str(),
										nullptr,		// default process security
										nullptr,		// default thread security
										FALSE,		// don't inherit handles
										NULL,
										nullptr,		// no new environment
										nullptr,		// no new cwd
										&StartupInfo, &ProcInfo) != 0;

			if (Result == false)
			{
				BGSEECONSOLE_ERROR("Couldn't launch LAME decoder!");
				return false;
			}

			WaitForSingleObject(ProcInfo.hProcess, INFINITE);		// wait till the decoder's done its job

			CloseHandle(ProcInfo.hProcess);
			CloseHandle(ProcInfo.hThread);
		}

		OldCSInteropData InteropDataOut(OldCSInteropData::kMessageType_GenerateLIP), InteropDataIn(OldCSInteropData::kMessageType_Wait);
		sprintf_s(InteropDataOut.StringBufferA, sizeof(InteropDataOut.StringBufferA), "%s", WAVPath.c_str());
		sprintf_s(InteropDataOut.StringBufferB, sizeof(InteropDataOut.StringBufferB), "%s", ResponseText);

		if (PerformPipeOperation(InteropPipeHandle, kPipeOperation_Write, &InteropDataOut, &ByteCounter))
		{
			InteropDataOut.MessageType = OldCSInteropData::kMessageType_Wait;

			while (true)
			{
				if (PerformPipeOperation(InteropPipeHandle, kPipeOperation_Read, &InteropDataIn, &ByteCounter))
				{
					switch (InteropDataIn.MessageType)
					{
					case OldCSInteropData::kMessageType_DebugPrint:
						BGSEECONSOLE_MESSAGE(InteropDataIn.StringBufferA);
						break;
					case OldCSInteropData::kMessageType_OperationResult:
						Result = InteropDataIn.OperationResult;
						ExitLoop = true;
						break;
					}

					if (ExitLoop)
						break;
				}
				else
				{
					BGSEECONSOLE_ERROR("CSInteropManager::GenerateLIPSyncFile - Idle loop encountered an error!");
					break;
				}
			}

			PerformPipeOperation(InteropPipeHandle, kPipeOperation_Write, &InteropDataOut, &ByteCounter);
		}
		else
		{
			BGSEECONSOLE_ERROR("Couldn't communicate with CS v1.0!");
		}

		// copy the result to the correct directory
		if (Result && UsingTempWav)
		{
			std::string IntermediatePath(BGSEEMAIN->GetAPPPath() + std::string(InputPath));
			IntermediatePath.erase(IntermediatePath.rfind("\\"));

			if (SHCreateDirectoryEx(nullptr, IntermediatePath.c_str(), nullptr) &&
				GetLastError() != ERROR_FILE_EXISTS &&
				GetLastError() != ERROR_ALREADY_EXISTS)
			{
				BGSEECONSOLE_ERROR("Couldn't create intermediate path for the lip file! - %s", IntermediatePath.c_str());
			}
			else
				CopyFile(std::string(BGSEEMAIN->GetAPPPath() + std::string(kTempLipPath)).c_str(), LIPPath.c_str(), FALSE);
		}

		// delete temp files
		DeleteFile((std::string(BGSEEMAIN->GetAPPPath() + std::string(kTempWavPath))).c_str());
		DeleteFile((std::string(BGSEEMAIN->GetAPPPath() + std::string(kTempMp3Path))).c_str());
		DeleteFile((std::string(BGSEEMAIN->GetAPPPath() + std::string(kTempLipPath))).c_str());


		return Result;
	}

	bool OldCSInteropManager::GetInitialized() const
	{
		return Loaded;
	}
}

