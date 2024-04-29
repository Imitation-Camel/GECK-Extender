#pragma once
#include "[BGSEEBase]\WorkspaceManager.h"

namespace gecke
{
	namespace workspaceManager
	{
		class WorkspaceManagerOperator : public bgsee::WorkspaceManagerOperator
		{
		public:
			virtual ~WorkspaceManagerOperator();

			virtual void						ResetCurrentWorkspace(void);
			virtual void						ReloadPlugins(const char* WorkspacePath, bool ResetPluginList, bool LoadESPs);
		};

		void Initialize();
		void Deinitialize();
	}
}
