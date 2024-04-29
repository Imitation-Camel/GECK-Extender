#pragma once
#include "ComponentDLLInterface.h"

struct OBSEInterface;
namespace gecke
{
	namespace cliWrapper
	{
		namespace interfaces
		{
			extern componentDLLInterface::ScriptEditorInterface*	SE;
			extern componentDLLInterface::UseInfoListInterface*		USE;
			extern componentDLLInterface::BSAViewerInterface*		BSA;
			extern componentDLLInterface::TagBrowserInterface*		TAG;
		}

		bool														ImportInterfaces(const NVSEInterface* obse);
		void														QueryInterfaces(void);
	}
}
