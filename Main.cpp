#include "Main.h"
#include "PluginAPIManager.h"
#include "VersionInfo.h"
#include "GECK Extender_Resource.h"

#include "[Common]\CLIWrapper.h"
#include "Hooks\Hooks-ScriptEditor.h"
#include "Hooks\Hooks-Misc.h"

#include "Console.h"
#include "UIManager.h"
#include "WorkspaceManager.h"
#include "Coda\Coda.h"
#include "MainWindowOverrides.h"

#include "[BGSEEBase]\ToolBox.h"
#include "[BGSEEBase]\Script\CodaVM.h"

namespace gecke
{
	NVSEMessagingInterface*						XSEMsgIntfc = nullptr;
	NVSECommandTableInterface*					XSECommandTableIntfc = nullptr;
	PluginHandle								XSEPluginHandle = kPluginHandle_Invalid;

	ReleaseNameTable							ReleaseNameTable::Instance;
	bool										IsWarholAGenius = false;

	ReleaseNameTable::ReleaseNameTable() :
		bgsee::ReleaseNameTable()
	{
		RegisterRelease(0, 1, "shadeMe Skunkworks");
	}

	ReleaseNameTable::~ReleaseNameTable()
	{
		;//
	}

	InitCallbackQuery::InitCallbackQuery(const NVSEInterface* NVSE) :
		bgsee::DaemonCallback(),
		NVSE(NVSE)
	{
		;//
	}

	InitCallbackQuery::~InitCallbackQuery()
	{
		;//
	}

	bool InitCallbackQuery::Handle(void* Parameter)
	{
		BGSEECONSOLE_MESSAGE("Initializing NVSE Interfaces");
		BGSEECONSOLE->Indent();
		XSEMsgIntfc = (NVSEMessagingInterface*)NVSE->QueryInterface(kInterface_Messaging);
		XSECommandTableIntfc = (NVSECommandTableInterface*)NVSE->QueryInterface(kInterface_CommandTable);

		if (XSEMsgIntfc == nullptr || XSECommandTableIntfc == nullptr)
		{
			BGSEECONSOLE_MESSAGE("Messaging/CommandTable interface not found");
			return false;
		}
		BGSEECONSOLE->Outdent();

		BGSEECONSOLE_MESSAGE("Initializing Component DLLs");
		BGSEECONSOLE->Indent();
		if (cliWrapper::ImportInterfaces(NVSE) == false)
			return false;
		BGSEECONSOLE->Outdent();

		return true;
	}

	InitCallbackLoad::InitCallbackLoad(const NVSEInterface* NVSE) :
		bgsee::DaemonCallback(),
		NVSE(NVSE)
	{
		;//
	}

	InitCallbackLoad::~InitCallbackLoad()
	{
		;//
	}

	bool InitCallbackLoad::Handle(void* Parameter)
	{
		BGSEECONSOLE_MESSAGE("Initializing Hooks");
		BGSEECONSOLE->Indent();
		hooks::PatchEntryPointHooks();
		hooks::PatchScriptEditorHooks();
		hooks::PatchMiscHooks();
		hooks::PatchMessageHanders();
		BGSEECONSOLE->Outdent();

		BGSEECONSOLE_MESSAGE("Initializing Events");
		BGSEECONSOLE->Indent();
		events::InitializeSinks();
		events::InitializeSources();
		BGSEECONSOLE->Outdent();

		BGSEECONSOLE_MESSAGE("Initializing UI Manager");
		BGSEECONSOLE->Indent();
		bool ComponentInitialized = bgsee::UIManager::Initialize("Garden of Eden Creation Kit",
																 LoadMenu(BGSEEMAIN->GetExtenderHandle(), MAKEINTRESOURCE(IDR_MAINMENU)));
		BGSEECONSOLE->Outdent();

		if (ComponentInitialized == false)
			return false;

		BGSEECONSOLE_MESSAGE("Registering NVSE Plugin Message Handlers");
		XSEMsgIntfc->RegisterListener(XSEPluginHandle, "NVSE", NVSEMessageHandler);

		return true;
	}

	int InitCallbackPostMainWindowInit::CrtNewHandler(size_t)
	{
		BGSEEDAEMON->GenerateCrashReportAndTerminate(bgsee::Daemon::CrashType::CPP_NEW_OPERATOR_ERROR, nullptr);
		return 0;
	}

	void InitCallbackPostMainWindowInit::CrtPureCallHandler(void)
	{
		BGSEEDAEMON->GenerateCrashReportAndTerminate(bgsee::Daemon::CrashType::CPP_PURE_CALL, nullptr);
	}

	void InitCallbackPostMainWindowInit::CrtInvalidParameterHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved)
	{
		BGSEECONSOLE_MESSAGE("EditorCrt Invalid Parameter Error!");
		BGSEEDAEMON->GenerateCrashReportAndTerminate(bgsee::Daemon::CrashType::CPP_INVALID_PARAMETER, nullptr);
	}

	void InitCallbackPostMainWindowInit::RegisterCrtExceptionHandlers()
	{
		//Crt::SetNewHandler(reinterpret_cast<_PNH>(&InitCallbackPostMainWindowInit::CrtNewHandler));
		//Crt::SetPureCallHandler(reinterpret_cast<_purecall_handler>(&InitCallbackPostMainWindowInit::CrtPureCallHandler));
		//Crt::SetInvalidParameterHandler(reinterpret_cast<_invalid_parameter_handler>(&InitCallbackPostMainWindowInit::CrtInvalidParameterHandler));
	}

	InitCallbackPostMainWindowInit::~InitCallbackPostMainWindowInit()
	{
		;//
	}

	bool InitCallbackPostMainWindowInit::Handle(void* Parameter)
	{
		uiManager::InitializeMainWindowOverrides();
		//uiManager::DeferredComboBoxController::Instance.Initialize();

		if (settings::dialogs::kShowMainWindowsInTaskbar.GetData().i)
		{
			bgsee::WindowStyler::StyleData RegularAppWindow = { 0 };
			RegularAppWindow.Extended = WS_EX_APPWINDOW;
			RegularAppWindow.ExtendedOp = bgsee::WindowStyler::StyleData::kOperation_OR;

			BGSEEUI->GetWindowStyler()->RegisterStyle(TESDialog::kDialogTemplate_FindText, RegularAppWindow);
			BGSEEUI->GetWindowStyler()->RegisterStyle(TESDialog::kDialogTemplate_SearchReplace, RegularAppWindow);
		}


		BGSEECONSOLE_MESSAGE("Registering Editor CRT Exception Handlers");
		RegisterCrtExceptionHandlers();

		return true;
	}

	InitCallbackEpilog::~InitCallbackEpilog()
	{
		;//
	}

	bool InitCallbackEpilog::Handle(void* Parameter)
	{
		BGSEECONSOLE_MESSAGE("Initializing Component DLL Interfaces");
		BGSEECONSOLE->Indent();
		cliWrapper::QueryInterfaces();
		BGSEECONSOLE->Outdent();

		BGSEECONSOLE_MESSAGE("Initializing Console");
		BGSEECONSOLE->Indent();
		console::Initialize();
		BGSEECONSOLE->Outdent();

		BGSEECONSOLE_MESSAGE("Initializing ScriptEditor");
		BGSEECONSOLE->Indent();

		componentDLLInterface::CommandTableData XSECommandTableData;
		XSECommandTableData.GetCommandReturnType = XSECommandTableIntfc->GetReturnType;
		XSECommandTableData.GetParentPlugin = XSECommandTableIntfc->GetParentPlugin;
		XSECommandTableData.GetRequiredNVSEVersion = XSECommandTableIntfc->GetRequiredNVSEVersion;
		XSECommandTableData.CommandTableStart = XSECommandTableIntfc->Start();
		XSECommandTableData.CommandTableEnd = XSECommandTableIntfc->End();

		componentDLLInterface::IntelliSenseUpdateData GMSTCollectionData;
		cliWrapper::interfaces::SE->InitializeComponents(&XSECommandTableData, &GMSTCollectionData);

		BGSEECONSOLE->Outdent();

		BGSEECONSOLE_MESSAGE("Initializing Coda \"Virtual Machine\"");
		BGSEECONSOLE->Indent();
		script::Initialize();
		BGSEECONSOLE->Outdent();

		//BGSEECONSOLE_MESSAGE("Initializing Toolbox");
		//BGSEECONSOLE->Indent();
		//bgsee::ToolBox::Initialize(BGSEEMAIN->INIGetter(), BGSEEMAIN->INISetter());
		//BGSEECONSOLE->Outdent();

		BGSEECONSOLE_MESSAGE("Initializing Workspace Manager");
		BGSEECONSOLE->Indent();
		workspaceManager::Initialize();
		BGSEECONSOLE->Outdent();

		BGSEECONSOLE_MESSAGE("Initializing Dialogs");
		BGSEECONSOLE->Indent();
		uiManager::Initialize();
		BGSEECONSOLE->Outdent();

		//BGSEECONSOLE_MESSAGE("Initializing Panic Save Handler");
		//BGSEECONSOLE->Indent();
		//_DATAHANDLER->PanicSave(true);
		//BGSEECONSOLE->Outdent();

		//BGSEECONSOLE_MESSAGE("Initializing Startup Manager");
		//BGSEECONSOLE->Indent();
		//StartupManager::LoadStartupWorkspace();
		//StartupManager::LoadStartupPlugin();
		//StartupManager::LoadStartupScript();
		//BGSEECONSOLE->Outdent();

		BGSEECONSOLE->OutdentAll();
		BGSEECONSOLE_MESSAGE("%s Initialized!", BGSEEMAIN->ExtenderGetDisplayName());
		BGSEECONSOLE->Pad(2);

		return true;
	}

	DeinitCallback::~DeinitCallback()
	{
		;//
	}

	bool DeinitCallback::Handle(void* Parameter)
	{
		TESDialog::WriteBoundsToINI(*TESCSMain::WindowHandle, nullptr);
		TESDialog::WriteBoundsToINI(*TESCellViewWindow::WindowHandle, "Cell View");
		TESDialog::WriteBoundsToINI(*TESObjectWindow::WindowHandle, "Object Window");
		TESDialog::WriteBoundsToINI(*TESRenderWindow::WindowHandle, "Render Window");

		BGSEECONSOLE_MESSAGE("Flushed GECK INI Settings");

		//settings::dialogs::kRenderWindowState.SetInt((GetMenuState(*TESCSMain::MainMenuHandle,
		//	TESCSMain::kMainMenu_View_RenderWindow, MF_BYCOMMAND) & MF_CHECKED) != 0);
		//settings::dialogs::kCellViewWindowState.SetInt((GetMenuState(*TESCSMain::MainMenuHandle,
		//	TESCSMain::kMainMenu_View_CellViewWindow, MF_BYCOMMAND) & MF_CHECKED) != 0);
		//settings::dialogs::kObjectWindowState.SetInt((GetMenuState(*TESCSMain::MainMenuHandle,
		//	TESCSMain::kMainMenu_View_ObjectWindow, MF_BYCOMMAND) & MF_CHECKED) != 0);

		TESCSMain::DeinitializeCSWindows();
		events::general::kShutdown.RaiseEvent();


		//BGSEECONSOLE_MESSAGE("Deinitializing Tool Manager");
		//BGSEECONSOLE->Indent();
		//bgsee::ToolBox::Deinitialize();
		//BGSEECONSOLE->Outdent();

		BGSEECONSOLE_MESSAGE("Deinitializing Workspace Manager");
		BGSEECONSOLE->Indent();
		workspaceManager::Deinitialize();
		BGSEECONSOLE->Outdent();

		BGSEECONSOLE_MESSAGE("Deinitializing Coda \"Virtual Machine\"");
		BGSEECONSOLE->Indent();
		script::Deinitialize();
		BGSEECONSOLE->Outdent();

		BGSEECONSOLE_MESSAGE("Deinitializing Script Editor");
		BGSEECONSOLE->Indent();
		cliWrapper::interfaces::SE->Deinitalize();
		BGSEECONSOLE->Outdent();

		BGSEECONSOLE_MESSAGE("Deinitializing Events");
		BGSEECONSOLE->Indent();
		events::DeinitializeSinks();
		events::DeinitializeSources();
		BGSEECONSOLE->Outdent();

#ifndef NDEBUG
		BGSEECONSOLE_MESSAGE("Performing Diagnostics...");
		BGSEECONSOLE->Indent();
		componentDLLInterface::DumpInstanceCounters();
		BGSEECONSOLE->Outdent();
#endif

		return true;
	}

	CrashCallback::CrashCallback()
	{
		HandlerCalled = false;
	}

	CrashCallback::~CrashCallback()
	{
		;//
	}

	bool CrashCallback::Handle(void* Parameter)
	{
		if (HandlerCalled)
			return false;
		else
			HandlerCalled = true;

		BGSEECONSOLE->Pad(2);
		BGSEECONSOLE_MESSAGE("The editor crashed, dammit!");
		BGSEECONSOLE->Indent();

		bool PanicSaved = false;
	/*	if (settings::general::kSalvageActivePluginOnCrash().i)
		{
			BGSEECONSOLE_MESSAGE("Attempting to salvage the active file...");
			BGSEECONSOLE->Indent();

			if ((PanicSaved = _DATAHANDLER->PanicSave()))
				BGSEECONSOLE_MESSAGE("Yup, we're good! Look for the panic save file in the Backup directory");
			else
				BGSEECONSOLE_MESSAGE("Bollocks-bollocks-bollocks! No can do...");

			BGSEECONSOLE->Outdent();
		}*/

		BGSEECONSOLE->Outdent();

		// it's highly inadvisable to do anything inside the handler apart from the bare minimum of diagnostics
		// memory allocations are a big no-no as the CRT state can potentially be corrupted...
		// ... but sod that! Achievements are more important, obviously.
		CR_CRASH_CALLBACK_INFO* CrashInfo = (CR_CRASH_CALLBACK_INFO*)Parameter;
		bool ResumeExecution = false;

		std::string CrashMessage("The editor has encountered a critical error!\n\nA crash report will be generated shortly in the following folder: ");
		CrashMessage += BGSEEMAIN->GetCrashReportDirPath();
		CrashMessage += "\n\n";
		int MBFlags = MB_TASKMODAL | MB_TOPMOST | MB_SETFOREGROUND | MB_ICONSTOP | MB_OK;

		if (PanicSaved)
			CrashMessage += "Unsaved changes were saved to the panic save file in the Data\\Backup folder.\n\n";


		switch (MessageBox(nullptr, CrashMessage.c_str(), BGSEEMAIN->ExtenderGetDisplayName(), MBFlags))
		{
		case IDOK:
			// nothing to do here
			break;
		case IDYES:
			ResumeExecution = true;
			break;
		case IDNO:
			ResumeExecution = false;
			break;
		}

		return ResumeExecution;
	}


	void StartupManager::LoadStartupPlugin()
	{
		bool Load = settings::startup::kLoadPlugin.GetData().i;
		const char* PluginName = settings::startup::kPluginName.GetData().s;

		if (Load)
		{
			// TODO: implement
		}
	}

	void StartupManager::LoadStartupScript()
	{
		bool Load = settings::startup::kOpenScriptWindow.GetData().i;
		const char* ScriptID = settings::startup::kScriptEditorID.GetData().s;

		if (Load)
		{
			if (strcmp(ScriptID, ""))
				TESDialog::ShowScriptEditorDialog(TESForm::LookupByEditorID(ScriptID));
			else
				TESDialog::ShowScriptEditorDialog(nullptr);
		}
	}

	void StartupManager::LoadStartupWorkspace()
	{
		bool Load = settings::startup::kSetWorkspace.GetData().i;
		const char* WorkspacePath = settings::startup::kWorkspacePath.GetData().s;

		if (Load)
			BGSEEWORKSPACE->SelectCurrentWorkspace(WorkspacePath);
	}

	void GECKSEInteropHandler(NVSEMessagingInterface::Message* Msg)
	{
		switch (Msg->type)
		{
		case 'CSEI':
			{
				BGSEECONSOLE_MESSAGE("Dispatching Plugin Interop Interface to '%s'", Msg->sender);
				BGSEECONSOLE->Indent();
				// XSEMsgIntfc->Dispatch(XSEPluginHandle, 'CSEI', (void*)PluginAPIManager::Instance.GetInterface(), 4, Msg->sender);
				BGSEECONSOLE->Outdent();
			}

			break;
		}
	}

	void NVSEMessageHandler(NVSEMessagingInterface::Message* Msg)
	{
		switch (Msg->type)
		{
		case NVSEMessagingInterface::kMessage_PostLoad:
			XSEMsgIntfc->RegisterListener(XSEPluginHandle, nullptr, GECKSEInteropHandler);
			break;
		case NVSEMessagingInterface::kMessage_PostPostLoad:
			break;
		}
	}
}

using namespace gecke;

extern "C"
{
	__declspec(dllexport) bool NVSEPlugin_Query(const NVSEInterface * nvse, PluginInfo * info)
	{
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = BGSEEMAIN_EXTENDERSHORTNAME;
		info->version = PACKED_SME_VERSION;

		XSEPluginHandle = nvse->GetPluginHandle();

		if (nvse->isEditor == false)
			return false;

		SME::MersenneTwister::init_genrand(GetTickCount());
		if (SME::MersenneTwister::genrand_real1() < 0.05)
			IsWarholAGenius = true;

		bgsee::Main::InitializationParams InitParams;
		InitParams.LongName = BGSEEMAIN_EXTENDERLONGNAME;
		InitParams.DisplayName = IsWarholAGenius ? "GECKO Extender" : nullptr;
		InitParams.ShortName = BGSEEMAIN_EXTENDERSHORTNAME;
		InitParams.ReleaseName = ReleaseNameTable::Instance.LookupRelease(VERSION_MAJOR, VERSION_MINOR);
		InitParams.Version = PACKED_SME_VERSION;
		InitParams.EditorID = bgsee::Main::kExtenderParentEditor_FONVGECK;
		InitParams.EditorSupportedVersion = 0x04002060;
		InitParams.EditorCurrentVersion = nvse->editorVersion;
		InitParams.APPPath = nvse->GetRuntimeDirectory();
		InitParams.SEPluginHandle = XSEPluginHandle;
		InitParams.SEMinimumVersion = 0x06030050;
		InitParams.SECurrentVersion = nvse->nvseVersion;
		InitParams.DotNETFrameworkVersion = "v4.0.30319";
		InitParams.CLRMemoryProfiling = false;

		settings::Register(InitParams.INISettings);

#ifdef WAIT_FOR_DEBUGGER
		InitParams.WaitForDebugger = true;
#else
		InitParams.WaitForDebugger = false;
#endif

#ifdef NDEBUG
	#ifndef WAIT_FOR_DEBUGGER
		InitParams.CrashRptSupport = true;
		TODO("Save debug symbols, dammit!")
	#else
		InitParams.CrashRptSupport = false;
	#endif
#else
		InitParams.CrashRptSupport = false;
#endif

		bool ComponentInitialized = bgsee::Main::Initialize(InitParams);
		if (ComponentInitialized == false)
		{
			MessageBox(nullptr,
					   "The GECK Extender failed to initialize correctly!\n\nCouldn't initialize main module.",
					   "Fatal Error",
					   MB_TASKMODAL | MB_SETFOREGROUND | MB_ICONERROR | MB_OK);

			return false;
		}

		BGSEEDAEMON->RegisterInitCallback(bgsee::Daemon::kInitCallback_Query, new InitCallbackQuery(nvse));
		BGSEEDAEMON->RegisterInitCallback(bgsee::Daemon::kInitCallback_Load, new InitCallbackLoad(nvse));
		BGSEEDAEMON->RegisterInitCallback(bgsee::Daemon::kInitCallback_PostMainWindowInit, new InitCallbackPostMainWindowInit());
		BGSEEDAEMON->RegisterInitCallback(bgsee::Daemon::kInitCallback_Epilog, new InitCallbackEpilog());
		BGSEEDAEMON->RegisterDeinitCallback(new DeinitCallback());
		BGSEEDAEMON->RegisterCrashCallback(new CrashCallback());

		if (BGSEEDAEMON->ExecuteInitCallbacks(bgsee::Daemon::kInitCallback_Query) == false)
		{
			MessageBox(nullptr,
					   "The GECK Extender failed to initialize correctly!\n\nIt's highly advised that you close the editor right away. More information can be found in the log file (GECK Extender.log in the root game directory).",
					   "The Cyrodiil Bunny Ranch",
					   MB_TASKMODAL | MB_SETFOREGROUND | MB_ICONERROR | MB_OK);

			BGSEECONSOLE->OpenDebugLog();
			return false;
		}

		return true;
	}

	__declspec(dllexport) bool NVSEPlugin_Load(const NVSEInterface * nvse)
	{
		if (BGSEEDAEMON->ExecuteInitCallbacks(bgsee::Daemon::kInitCallback_Load) == false)
		{
			MessageBox(nullptr,
					   "The GECK Extender failed to load correctly!\n\nIt's highly advised that you close the editor right away. More information can be found in the log file (GECK Extender.log in the root game directory).",
					   "Rumpy-Pumpy!!",
					   MB_TASKMODAL | MB_SETFOREGROUND | MB_ICONERROR | MB_OK);

			BGSEECONSOLE->OpenDebugLog();
			return false;
		}

		return true;
	}
};

BOOL WINAPI DllMain(HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
#ifdef WAIT_FOR_DEBUGGER
		bgsee::Daemon::WaitForDebugger();
#endif
		break;
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}