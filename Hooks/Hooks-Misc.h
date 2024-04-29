#pragma once
#include "Hooks-Common.h"

namespace gecke
{
	namespace hooks
	{
		// hooks that do odd jobs such as fixing bugs and shovelling dung
		void PatchMiscHooks(void);
		void PatchEntryPointHooks(void);
		void PatchMessageHanders(void);

		_DeclareMemHdlr(CSExit, "adds fast exit to the CS");
		_DeclareMemHdlr(CSInit, "adds an one-time only hook to the CS main windows wndproc as an alternative to WinMain()");
		_DeclareMemHdlr(CustomCSWindow, "keeps custom child windows of the CS main window from being closed on plugin load");


	}
}

