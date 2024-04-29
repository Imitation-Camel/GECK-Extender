#include "Console.h"

#include "[BGSEEBase]\BGSEditorExtenderBase_Resource.h"

namespace gecke
{
	namespace console
	{
		void BGSEEConsoleCmd_88MPH_ExecuteHandler(BGSEECONSOLECMD_ARGS)
		{
			BGSEECONSOLE_MESSAGE("Great Scott! We left Copernicus in the freezer once again!");
		}

		void BGSEEConsoleCmd_Exit_ExecuteHandler(BGSEECONSOLECMD_ARGS)
		{
			SendMessage(BGSEEUI->GetMainWindow(), WM_CLOSE, 0, 0);
		}

		void BGSEEConsoleCmd_Crash_ExecuteHandler(BGSEECONSOLECMD_ARGS)
		{
			SME::StringHelpers::Tokenizer ArgParser(Args, " ,");
			std::string CurrentArg;

			bool Assertion = false;

			for (int i = 1; i <= ParamCount; i++)
			{
				ArgParser.NextToken(CurrentArg);
				switch (i)
				{
				case 1:
					Assertion = CurrentArg != "0";
					break;
				}
			}

			if (Assertion)
			{
				SME_ASSERT(1 == 0);
			}
			else
			{
				*((int*)0) = 0;
			}
		}


		DEFINE_BGSEECONSOLECMD(88MPH, 0);
		DEFINE_BGSEECONSOLECMD(Exit, 0);
		DEFINE_BGSEECONSOLECMD(Crash, 1);


		bool Initialized = false;

		void Initialize()
		{
			if (settings::dialogs::kShowMainWindowsInTaskbar.GetData().i)
			{
				bgsee::WindowStyler::StyleData RegularAppWindow = {0};
				RegularAppWindow.Extended = WS_EX_APPWINDOW;
				RegularAppWindow.ExtendedOp = bgsee::WindowStyler::StyleData::kOperation_OR;

				BGSEEUI->GetWindowStyler()->RegisterStyle(IDD_BGSEE_CONSOLE, RegularAppWindow);
			}

			BGSEECONSOLE->InitializeUI(BGSEEUI->GetMainWindow(), BGSEEMAIN->GetExtenderHandle());

			BGSEECONSOLE->RegisterConsoleCommand(&kBGSEEConsoleCmd_88MPH);
			BGSEECONSOLE->RegisterConsoleCommand(&kBGSEEConsoleCmd_Exit);
			BGSEECONSOLE->RegisterConsoleCommand(&kBGSEEConsoleCmd_Crash);

			Initialized = true;
		}
	}
}