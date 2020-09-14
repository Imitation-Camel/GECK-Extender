#pragma once

#include "[Common]\NativeWrapper.h"

namespace cse
{
	ref class Globals
	{
	public:
		static ImageResourceManager^								ScriptEditorImageResourceManager = gcnew ImageResourceManager("ScriptEditor.ImagesModern");
		static int													MainThreadID = -1;
		static System::Threading::Tasks::TaskScheduler^				MainThreadTaskScheduler = nullptr;
	};

#define SetupControlImage(Identifier)							Identifier##->Name = #Identifier;	\
																Identifier##->Image = Globals::ScriptEditorImageResourceManager->CreateImage(#Identifier);

#define DisposeControlImage(Identifier)							delete Identifier##->Image; \
																Identifier##->Image = nullptr
}
