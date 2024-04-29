#pragma once

namespace gecke
{
	namespace hooks
	{
		void PatchCompilerErrorDetours();

		_DeclareNopHdlr(RidUnknownFunctionCodeMessage, "removes a redundant error message");
		_DeclareMemHdlr(RerouteScriptErrors, "reroutes script compilation error messages to the parent script editor");
		_DeclareMemHdlr(CompilerEpilogCheck, "adds support for the above");
		_DeclareMemHdlr(CompilerPrologReset, "");
		_DeclareMemHdlr(ParseScriptLineOverride, "");
		_DeclareMemHdlr(CheckLineLengthLineCount, "fixes a bug in the compiler that causes empty lines to be skipped when line numbers are counted");
		_DeclareMemHdlr(ResultScriptErrorNotification, "displays a notification when result scripts fail to compile");
		_DeclareMemHdlr(MaxScriptSizeExceeded, "ensures compilation breaks immediately");

#define GetErrorMemHdlr(hookaddr)								CompilerErrorOverrideHandler##hookaddr
#define DefineCompilerErrorOverrideHook(hookaddr, jmpaddr, stackoffset)		\
	static void CompilerErrorOverrideHandler##hookaddr##Hook(void);					\
	SME::MemoryHandler::MemHdlr CompilerErrorOverrideHandler##hookaddr##(##hookaddr##, CompilerErrorOverrideHandler##hookaddr##Hook, 0, 0);		\
	static void __declspec(naked) CompilerErrorOverrideHandler##hookaddr##Hook(void)	\
	{																			\
		static UInt32 CompilerErrorOverrideHandler##hookaddr##RetnAddr = jmpaddr##;		\
		{																	\
		__asm	call	TESScriptCompiler::ShowMessage						\
		__asm	mov		ScriptCompileResultBuffer, 0						\
		__asm	add		esp, stackoffset									\
		__asm	jmp		CompilerErrorOverrideHandler##hookaddr##RetnAddr	\
		}																	\
	}

		extern UInt32						ScriptCompileResultBuffer;
	}
}
