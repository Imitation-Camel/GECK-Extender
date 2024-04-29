#include "Coda.h"
#include "PluginAPIManager.h"


namespace gecke
{
	namespace script
	{
		void ExtractFormArguments( UInt32 Count, ... )
		{
			va_list Args;
			va_start(Args, Count);

			for (int i = 0; i < Count; i++)
			{
				TESForm** CurrentArg = va_arg(Args, TESForm**);
				UInt32 FormID = (UInt32)*CurrentArg;

				if (FormID)
					*CurrentArg = TESForm::LookupByFormID(FormID);
				else
					*CurrentArg = nullptr;
			}

			va_end(Args);
		}

		void BGSEEConsoleCmd_RunCodaScript_ExecuteHandler(UInt32 ParamCount, const char* Args)
		{
			SME::StringHelpers::Tokenizer ArgParser(Args, " ,");
			std::string CurrentArg;

			std::string ScriptName;
			std::string RunInBackground;

			for (int i = 1; i <= ParamCount; i++)
			{
				ArgParser.NextToken(CurrentArg);
				switch (i)
				{
				case 1:
					ScriptName = CurrentArg;
					break;
				case 2:
					RunInBackground = CurrentArg;
					break;
				}
			}

			bgsee::script::ICodaScriptVirtualMachine::ExecuteParams Input;
			bgsee::script::ICodaScriptVirtualMachine::ExecuteResult Output;

			Input.Filepath = ScriptName;
			Input.Recompile = true;
			Input.RunInBackground = RunInBackground == "1" ? true : false;

			CODAVM->RunScript(Input, Output);
		}

		DEFINE_BGSEECONSOLECMD(RunCodaScript, 2);

		void Initialize()
		{
			bgsee::script::CodaScriptCommandRegistrar::ListT ScriptCommands;

			bool ComponentInitialized = bgsee::script::CodaScriptVM::Initialize(bgsee::ResourceLocation(GECKE_CODADEPOT),
														"https://geckwiki.com/",
														BGSEEMAIN->INIGetter(),
														BGSEEMAIN->INISetter(),
														ScriptCommands);
			SME_ASSERT(ComponentInitialized);

			BGSEECONSOLE->RegisterConsoleCommand(&kBGSEEConsoleCmd_RunCodaScript);
		}

		void Deinitialize()
		{
			bgsee::script::CodaScriptVM::Deinitialize();
		}

	}
}