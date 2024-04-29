#pragma once

// misc event sinks that don't fit elsewhere

#define DECLARE_BASIC_EVENT_SINK(name)												\
		class name##EventSink : public SME::MiscGunk::IEventSink					\
		{																			\
		public:																		\
			virtual void					Handle(SME::MiscGunk::IEventData* Data);\
			static name##EventSink			Instance;								\
		}

#define DEFINE_BASIC_EVENT_SINK_HANDLER(name)				void name##EventSink::Handle(SME::MiscGunk::IEventData* Data)
#define DEFINE_BASIC_EVENT_SINK(name)						name##EventSink				name##EventSink::Instance
#define ADD_BASIC_SINK_TO_SOURCE(sink, source)				source.AddSink(&sink##EventSink::Instance)
#define REMOVE_BASIC_SINK_FROM_SOURCE(sink, source)			source.RemoveSink(&sink##EventSink::Instance)

namespace gecke
{
	namespace events
	{
		DECLARE_BASIC_EVENT_SINK(DestroyCustomDialogs);
		DECLARE_BASIC_EVENT_SINK(PostPluginSaveTasks);
		DECLARE_BASIC_EVENT_SINK(PrePluginLoadTasks);
		DECLARE_BASIC_EVENT_SINK(PostPluginLoadTasks);
		DECLARE_BASIC_EVENT_SINK(DataHandlerConstructSpecialForms);

		void InitializeSinks();
		void DeinitializeSinks();
	}
}
