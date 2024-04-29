#include "Hooks-CompilerErrorDetours.h"
#include "[Common]/CLIWrapper.h"

#pragma warning(push)
#pragma warning(disable: 4005 4748)

namespace gecke
{
	namespace hooks
	{
		UInt32 ScriptCompileResultBuffer = 0;	// saves the result of a compile operation so as to allow it to go on unhindered

		_DefineNopHdlr(RidUnknownFunctionCodeMessage, 0x005C94A8, 5);
		_DefineHookHdlr(RerouteScriptErrors, 0x005C578D);
		_DefineHookHdlr(CompilerPrologReset, 0x005C96E0);
		_DefineHookHdlr(CompilerEpilogCheck, 0x005C97CF);
		_DefineHookHdlr(ParseScriptLineOverride, 0x005C97B7);
		_DefineHookHdlr(CheckLineLengthLineCount, 0x005C5929);
		_DefineHookHdlr(ResultScriptErrorNotification, 0x005C99AC);
		_DefineHookHdlr(MaxScriptSizeExceeded, 0x005C92FF);

		// ERROR HANDLERS
																		//  f_ScriptBuffer__ConstructLineBuffers
		DefineCompilerErrorOverrideHook(0x005C8AD2, 0x005C8B05, 0xC)
		DefineCompilerErrorOverrideHook(0x005C8B56, 0x005C8B21, 0xC)
		DefineCompilerErrorOverrideHook(0x005C8B41, 0x005C8B67, 0xC)
		DefineCompilerErrorOverrideHook(0x005C8C13, 0x005C8BCB, 0x8)
		DefineCompilerErrorOverrideHook(0x005C8BF9, 0x005C8C27, 0x8)
																		// f_ScriptCompiler__CheckSyntax
		// DefineCompilerErrorOverrideHook(0x00500B44, 0x00500B4C, 0x8)	// Mismatching quotes check - this one needs to return immediately , will CTD otherwise
		DefineCompilerErrorOverrideHook(0x005C633B, 0x005C6261, 0x8)
		DefineCompilerErrorOverrideHook(0x005C6354, 0x005C626E, 0x8)
		DefineCompilerErrorOverrideHook(0x005C636A, 0x005C628C, 0xC)
		DefineCompilerErrorOverrideHook(0x005C639C, 0x005C6309, 0x8)

		DefineCompilerErrorOverrideHook(0x005C6383, 0x005C62EF, 0x8)
		DefineCompilerErrorOverrideHook(0x005C63E7, 0x005C63F6, 0xC)
		DefineCompilerErrorOverrideHook(0x005C645F, 0x005C6494, 0xC)
		DefineCompilerErrorOverrideHook(0x005C6485, 0x005C6494, 0x8)
																		// f_ScriptBuffer__ConstructRefVariables
		DefineCompilerErrorOverrideHook(0x005C5DAD, 0x005C5DBA, 0xC)
		DefineCompilerErrorOverrideHook(0x005C5DD3, 0x005C5DE2, 0xC)
																		// f_ScriptCompiler__CheckScriptBlockStructure
		DefineCompilerErrorOverrideHook(0x005C5A25, 0x005C5A13, 0x8)
		DefineCompilerErrorOverrideHook(0x005C5A3F, 0x005C5A13, 0x8)
		DefineCompilerErrorOverrideHook(0x005C5A59, 0x005C5A13, 0x8)
																		// f_ScriptBuffer__CheckReferencedObjects
		//DefineCompilerErrorOverrideHook(0x005C59AC, 0x005001C9, 0xC) // this needs a custom hook as we need to spill eax to the stack

		void PatchCompilerErrorDetours()
		{
			_MemHdlr(RidUnknownFunctionCodeMessage).WriteNop();
			_MemHdlr(RerouteScriptErrors).WriteJump();
			_MemHdlr(CompilerPrologReset).WriteJump();
			_MemHdlr(CompilerEpilogCheck).WriteJump();
			_MemHdlr(ParseScriptLineOverride).WriteJump();
			_MemHdlr(CheckLineLengthLineCount).WriteJump();
			_MemHdlr(ResultScriptErrorNotification).WriteJump();
			_MemHdlr(MaxScriptSizeExceeded).WriteJump();

			GetErrorMemHdlr(0x005C8AD2).WriteJump();
			GetErrorMemHdlr(0x005C8B56).WriteJump();
			GetErrorMemHdlr(0x005C8B41).WriteJump();
			GetErrorMemHdlr(0x005C8BF9).WriteJump();

			GetErrorMemHdlr(0x005C633B).WriteJump();
			GetErrorMemHdlr(0x005C6354).WriteJump();
			GetErrorMemHdlr(0x005C636A).WriteJump();
			GetErrorMemHdlr(0x005C639C).WriteJump();

			GetErrorMemHdlr(0x005C6383).WriteJump();
			GetErrorMemHdlr(0x005C63E7).WriteJump();
			GetErrorMemHdlr(0x005C645F).WriteJump();
			GetErrorMemHdlr(0x005C6485).WriteJump();

			GetErrorMemHdlr(0x005C5DAD).WriteJump();
			GetErrorMemHdlr(0x005C5DD3).WriteJump();

			GetErrorMemHdlr(0x005C5A25).WriteJump();
			GetErrorMemHdlr(0x005C5A3F).WriteJump();
			GetErrorMemHdlr(0x005C5A59).WriteJump();
		}

		void __stdcall DoRerouteScriptErrorsHook(UInt32 Line, const char* Message)
		{
			TESScriptCompiler::CompilerMessage CurrentMessage(Line, Message);

			// flag the entire operation as a failure iff the message is a vanilla/OBSE compiler error
			if (CurrentMessage.IsError())
				ScriptCompileResultBuffer = 0;

			// don't handle when compiling result scripts or recompiling
			if (!TESScriptCompiler::PreventErrorDetours)
				TESScriptCompiler::LastCompilationMessages.emplace_back(std::move(CurrentMessage));
		}

		#define _hhName		RerouteScriptErrors
		_hhBegin()
		{
			_hhSetVar(Retn, 0x005C5795);
			__asm
			{
				mov     dword ptr [esp+14h], 0

				lea     edx, [esp + 0x1C]
				pushad
				push	edx
				push	[esi + 0x1C]
				call	DoRerouteScriptErrorsHook
				popad

				jmp		_hhGetVar(Retn)
			}
		}

		UInt32	MaxScriptSizeExceeded = 0;

		#define _hhName		CompilerPrologReset
		_hhBegin()
		{
			_hhSetVar(Retn, 0x005C96E6);
			__asm
			{
				mov		ScriptCompileResultBuffer, 1
				mov		MaxScriptSizeExceeded, 0
				pushad
			}
			TESScriptCompiler::LastCompilationMessages.clear();
			__asm
			{
				popad
				push    ebx
				push    ebp
				mov     ebp, [esp + 0xC]

				jmp		_hhGetVar(Retn)
			}
		}

		#define _hhName		CompilerEpilogCheck
		_hhBegin()
		{
			_hhSetVar(Retn, 0x005C97D4);
			_hhSetVar(Call, 0x005C5980);
			__asm
			{
				call	_hhGetVar(Call)
				mov		eax, ScriptCompileResultBuffer

				jmp		_hhGetVar(Retn)
			}
		}
		

		#define _hhName		ParseScriptLineOverride
		_hhBegin()
		{
			_hhSetVar(Retn, 0x005C97C0);
			_hhSetVar(Call, 0x005C8C40);
			_hhSetVar(Exit, 0x005C9779);
			__asm
			{
				call	_hhGetVar(Call)
				test	al, al
				jz		FAIL

				jmp		_hhGetVar(Retn)
			FAIL:
				mov		ScriptCompileResultBuffer, 0
				mov		eax, MaxScriptSizeExceeded
				test	eax, eax
				jnz		EXIT

				jmp		_hhGetVar(Retn)
			EXIT:
				jmp		_hhGetVar(Exit)
			}
		}

		#define _hhName		CheckLineLengthLineCount
		_hhBegin()
		{
			_hhSetVar(Retn, 0x005C5930);
			__asm
			{
				mov		eax, [esp + 0x18]
				add		[eax + 0x1C], 1

				add     dword ptr [esi], 1
				push    0x200

				jmp		_hhGetVar(Retn)
			}
		}

		void __stdcall DoResultScriptErrorNotificationHook(void)
		{
			BGSEEUI->MsgBoxE(nullptr,
							 MB_TASKMODAL|MB_TOPMOST|MB_SETFOREGROUND|MB_OK,
							 "Result script compilation failed. Check the console for error messages.");
		}

		#define _hhName		ResultScriptErrorNotification
		_hhBegin()
		{
			_hhSetVar(Retn, 0x005C99B1);
			_hhSetVar(Call, 0x005C96E0);
			__asm
			{
				mov		TESScriptCompiler::PreventErrorDetours, 1
				call	_hhGetVar(Call)
				mov		TESScriptCompiler::PreventErrorDetours, 0
				test	al, al
				jz		FAIL

				jmp		_hhGetVar(Retn)
			FAIL:
				pushad
				call	DoResultScriptErrorNotificationHook
				popad

				jmp		_hhGetVar(Retn)
			}
		}

		#define _hhName		MaxScriptSizeExceeded
		_hhBegin()
		{
			_hhSetVar(Retn, 0x005C8DE3);
			__asm
			{
				mov		MaxScriptSizeExceeded, 1
				push	0x00D61170
				jmp		_hhGetVar(Retn)
			}
		}
	}
}

#pragma warning(pop)