#include "Hooks-Misc.h"

#pragma warning(push)
#pragma warning(disable: 4005 4748)

namespace gecke
{
	namespace hooks
	{
		_DefineHookHdlrWithBuffer(CSInit, 0x004407B9, 5, 0x8B, 0x85, 0x38, 0x08, 00);
		_DefineHookHdlr(CSExit, 0x00440924);
		_DefineHookHdlr(CustomCSWindow, 0x00465725);

		void PatchMiscHooks(void)
		{
			_MemHdlr(CSInit).WriteJump();
			_MemHdlr(CSExit).WriteJump();
			_MemHdlr(CustomCSWindow).WriteJump();
		}

		void PatchEntryPointHooks(void)
		{
			// TODO anything that needs to be patched before the main window is created
		}

		void __stdcall MessageHandlerOverride(const char* Message)
		{
			BGSEECONSOLE->LogWarning("GECK", "%s", Message);
		}

		void PatchMessageHanders(void)
		{
			// TODO patch 0x00AA4830 to print to the console
		}

		void __stdcall DoCSInitHook()
		{
			if (*TESCSMain::WindowHandle == nullptr ||
				*TESObjectWindow::Initialized == 0 ||
				*TESCellViewWindow::WindowHandle == nullptr ||
				*TESRenderWindow::WindowHandle == nullptr)
			{
				return;								// prevents the hook from being called before the full init
			}

			_MemHdlr(CSInit).WriteBuffer();

			BGSEEDAEMON->ExecuteInitCallbacks(bgsee::Daemon::kInitCallback_Epilog);
		}

		#define _hhName	CSInit
		_hhBegin()
		{
			_hhSetVar(Retn, 0x004407BF);
			__asm
			{
				mov     eax, [ebp+0x838]
				pushad
				call	DoCSInitHook
				popad
				jmp		_hhGetVar(Retn)
			}
		}

		void __stdcall DoCSExitHook(HWND MainWindow)
		{
			bgsee::Main::Deinitialize();
		}

		#define _hhName	CSExit
		_hhBegin()
		{
			__asm
			{
				push    ebx
				call    DoCSExitHook
			}
		}

		bool __stdcall DoCustomCSWindowPatchHook(HWND Window)
		{
			if (BGSEEUI->GetWindowHandleCollection(bgsee::UIManager::kHandleCollection_MainWindowChildren)->GetExists(Window))
				return false;
			else
				return true;
		}

		#define _hhName		CustomCSWindow
		_hhBegin()
		{
			_hhSetVar(Retn, 0x0046572F);
			__asm
			{
				mov		edi, [TESCSMain::WindowHandle]
				cmp		eax, edi
				jnz		FAIL
				xor		edi, edi
				pushad
				push	esi
				call	DoCustomCSWindowPatchHook
				test	eax, eax
				jz		FAIL
				popad
				mov		edi, esi
			FAIL:
				jmp		_hhGetVar(Retn)
			}
		}
	}
}

#pragma warning(pop)