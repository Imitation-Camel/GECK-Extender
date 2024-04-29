#include "EventSinks.h"
#include "[Common]\CLIWrapper.h"

namespace gecke
{
	namespace events
	{
		DEFINE_BASIC_EVENT_SINK(PostPluginSaveTasks);
		DEFINE_BASIC_EVENT_SINK_HANDLER(PostPluginSaveTasks)
		{
			cliWrapper::interfaces::SE->UpdateIntelliSenseDatabase();
		}

		DEFINE_BASIC_EVENT_SINK(PrePluginLoadTasks);
		DEFINE_BASIC_EVENT_SINK_HANDLER(PrePluginLoadTasks)
		{
			cliWrapper::interfaces::SE->CloseAllOpenEditors();
			cliWrapper::interfaces::USE->HideUseInfoListDialog();
			cliWrapper::interfaces::TAG->HideTagBrowserDialog();
		}

		DEFINE_BASIC_EVENT_SINK(PostPluginLoadTasks);
		DEFINE_BASIC_EVENT_SINK_HANDLER(PostPluginLoadTasks)
		{
			cliWrapper::interfaces::SE->UpdateIntelliSenseDatabase();
		}


		void InitializeSinks()
		{
			ADD_BASIC_SINK_TO_SOURCE(PostPluginSaveTasks, plugin::kPostSave);
			ADD_BASIC_SINK_TO_SOURCE(PrePluginLoadTasks, plugin::kPreLoad);
			ADD_BASIC_SINK_TO_SOURCE(PostPluginLoadTasks, plugin::kPostLoad);
		}

		void DeinitializeSinks()
		{
			REMOVE_BASIC_SINK_FROM_SOURCE(PostPluginSaveTasks, plugin::kPostSave);
			REMOVE_BASIC_SINK_FROM_SOURCE(PrePluginLoadTasks, plugin::kPreLoad);
			REMOVE_BASIC_SINK_FROM_SOURCE(PostPluginLoadTasks, plugin::kPostLoad);
		}

	}
}