#include "MainWindowOverrides.h"
#include "GECK Extender_Resource.h"
#include "[Common]\CLIWrapper.h"
#include "WorkspaceManager.h"

#include "[BGSEEBase]\ToolBox.h"
#include "[BGSEEBase]\Script\CodaVM.h"

namespace gecke
{
	namespace uiManager
	{

		LRESULT CALLBACK MainWindowMenuInitSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
														bgsee::WindowSubclassProcCollection::SubclassProcExtraParams* SubclassParams)
		{
			LRESULT DlgProcResult = FALSE;

			switch (uMsg)
			{
			case WM_INITMENUPOPUP:
				{
					if (HIWORD(lParam) == FALSE)
					{
						HMENU Popup = (HMENU)wParam;

						for (int i = 0, j = GetMenuItemCount(Popup); i < j; i++)
						{
							MENUITEMINFO CurrentItem = { 0 };
							CurrentItem.cbSize = sizeof(MENUITEMINFO);
							CurrentItem.fMask = MIIM_ID | MIIM_STATE;

							if (GetMenuItemInfo(Popup, i, TRUE, &CurrentItem) == TRUE)
							{
								bool UpdateItem = true;
								bool CheckItem = false;
								bool DisableItem = false;

								switch (CurrentItem.wID)
								{
								case IDC_MAINMENU_SAVEOPTIONS_SAVEESPMASTERS:
									if (settings::plugins::kSaveLoadedESPsAsMasters.GetData().i)
										CheckItem = true;

									break;
								case IDC_MAINMENU_SAVEOPTIONS_PREVENTCHANGESTOFILETIMESTAMPS:
									if (settings::plugins::kPreventTimeStampChanges.GetData().i)
										CheckItem = true;

									break;
								case IDC_MAINMENU_SAVEOPTIONS_CREATEBACKUPBEFORESAVING:
									if (settings::versionControl::kBackupOnSave.GetData().i)
										CheckItem = true;

									break;
								case IDC_MAINMENU_CONSOLE:
									if (BGSEECONSOLE->IsVisible())
										CheckItem = true;

									break;
								//case IDC_MAINMENU_HIDEUNMODIFIEDFORMS:
								//	if (FormEnumerationManager::Instance.GetVisibleUnmodifiedForms() == false)
								//		CheckItem = true;

								//	break;
								//case IDC_MAINMENU_HIDEDELETEDFORMS:
								//	if (FormEnumerationManager::Instance.GetVisibleDeletedForms() == false)
								//		CheckItem = true;

								//	break;
								case IDC_MAINMENU_SORTACTIVEFORMSFIRST:
									if (settings::dialogs::kSortFormListsByActiveForm.GetData().i)
										CheckItem = true;

									break;
								case IDC_MAINMENU_COLORIZEACTIVEFORMS:
									if (settings::dialogs::kColorizeActiveForms.GetData().i)
										CheckItem = true;

									break;
								case IDC_MAINMENU_COLORIZEFORMOVERRIDES:
									if (settings::dialogs::kColorizeFormOverrides.GetData().i)
										CheckItem = true;

									break;
								case IDC_MAINMENU_CODABACKGROUNDER:
									if (CODAVM->GetBackgroundDaemon()->IsEnabled())
										CheckItem = true;

									break;
								case IDC_MAINMENU_PARENTCHILDINDICATORS:
									if (settings::renderer::kParentChildVisualIndicator().i)
										CheckItem = true;

									break;
								case IDC_MAINMENU_PATHGRIDLINKEDREFINDICATORS:
									if (settings::renderer::kPathGridLinkedRefIndicator().i)
										CheckItem = true;

									break;
								case IDC_MAINMENU_PATHGRIDLINKEDREFINDICATORSETTINGS_HIDEBOUNDINGBOX:
									if ((settings::renderer::kPathGridLinkedRefIndicatorFlags().u &
										 settings::renderer::kPathGridLinkedRefIndicatorFlag_HidePointBoundingBox))
									{
										CheckItem = true;
									}

									break;
								case IDC_MAINMENU_PATHGRIDLINKEDREFINDICATORSETTINGS_HIDELINKEDREFNODE:
									if ((settings::renderer::kPathGridLinkedRefIndicatorFlags().u &
										 settings::renderer::kPathGridLinkedRefIndicatorFlag_HideLinkedRefNode))
									{
										CheckItem = true;
									}

									break;
								case IDC_MAINMENU_PATHGRIDLINKEDREFINDICATORSETTINGS_HIDELINECONNECTOR:
									if ((settings::renderer::kPathGridLinkedRefIndicatorFlags().u &
										settings::renderer::kPathGridLinkedRefIndicatorFlag_HideLineConnector))
									{
										CheckItem = true;
									}

									break;
								case IDC_MAINMENU_CUSTOMCOLORTHEMEMODE:
									if (BGSEEUI->GetColorThemer()->IsEnabled())
										CheckItem = true;

									break;
								case IDC_MAINMENU_ONEDITORSTARTUP_LOADSTARTUPPLUGIN:
									if (settings::startup::kLoadPlugin().i)
										CheckItem = true;

									break;
								case IDC_MAINMENU_ONEDITORSTARTUP_SETSTARTUPWORKSPACE:
									if (settings::startup::kSetWorkspace().i)
										CheckItem = true;

									break;
								case IDC_MAINMENU_ONEDITORSTARTUP_OPENSTARTUPSCRIPT:
									if (settings::startup::kOpenScriptWindow().i)
										CheckItem = true;

									break;
								default:
									UpdateItem = false;
									break;
								}

								if (UpdateItem)
								{
									if (CheckItem)
									{
										CurrentItem.fState &= ~MFS_UNCHECKED;
										CurrentItem.fState |= MFS_CHECKED;
									}
									else
									{
										CurrentItem.fState &= ~MFS_CHECKED;
										CurrentItem.fState |= MFS_UNCHECKED;
									}

									if (DisableItem)
									{
										CurrentItem.fState &= ~MFS_ENABLED;
										CurrentItem.fState |= MFS_DISABLED;
									}
									else
									{
										CurrentItem.fState &= ~MFS_DISABLED;
										CurrentItem.fState |= MFS_ENABLED;
									}

									CurrentItem.fMask = MIIM_STATE;
									SetMenuItemInfo(Popup, i, TRUE, &CurrentItem);
								}
							}
						}
					}

					SubclassParams->Out.MarkMessageAsHandled = true;
				}

				break;
			}

			return DlgProcResult;
		}

		LRESULT CALLBACK MainWindowMenuSelectSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
														  bgsee::WindowSubclassProcCollection::SubclassProcExtraParams* SubclassParams)
		{
			LRESULT DlgProcResult = FALSE;

			switch (uMsg)
			{
			case WM_COMMAND:
				SubclassParams->Out.MarkMessageAsHandled = true;

				switch (LOWORD(wParam))
				{
				case IDC_MAINMENU_CONSOLE:
					BGSEECONSOLE->ToggleVisibility();

					break;
				//case IDC_MAINMENU_HIDEDELETEDFORMS:
				//	FormEnumerationManager::Instance.ToggleVisibilityDeletedForms();

				//	break;
				//case IDC_MAINMENU_HIDEUNMODIFIEDFORMS:
				//	FormEnumerationManager::Instance.ToggleVisibilityUnmodifiedForms();

				//	break;
				case IDC_MAINMENU_CSEPREFERENCES:
					BGSEEMAIN->ShowPreferencesGUI();

					break;
				case IDC_MAINMENU_LAUNCHGAME:
					{
						std::string AppPath = BGSEEMAIN->GetAPPPath();
						AppPath += "\\";

						IFileStream SteamLoader;
						if (SteamLoader.Open((std::string(AppPath + "nvse_steam_loader.dll")).c_str()) == false)
							AppPath += "nvse_loader.exe";
						else
							AppPath += "FalloutNV.exe";

						ShellExecute(nullptr, "open", (LPCSTR)AppPath.c_str(), nullptr, nullptr, SW_SHOW);
					}

					break;
				case IDC_MAINMENU_TAGBROWSER:
					cliWrapper::interfaces::TAG->ShowTagBrowserDialog(nullptr);

					break;
				case IDC_MAINMENU_SETWORKSPACE:
					BGSEEWORKSPACE->SelectCurrentWorkspace();

					break;
				case IDC_MAINMENU_TOOLS:
					BGSEETOOLBOX->ShowToolListMenu(BGSEEMAIN->GetExtenderHandle(), hWnd);

					break;
				case IDC_MAINMENU_CODAMANAGEGLOBALDATASTORE:
					CODAVM->ShowGlobalStoreEditDialog(BGSEEMAIN->GetExtenderHandle(), hWnd);

					break;
				case IDC_MAINMENU_CODABACKGROUNDER:
					if (CODAVM->GetBackgroundDaemon()->IsEnabled())
						CODAVM->GetBackgroundDaemon()->Suspend();
					else
						CODAVM->GetBackgroundDaemon()->Resume();

					break;
				case IDC_MAINMENU_SAVEOPTIONS_SAVEESPMASTERS:
					settings::plugins::kSaveLoadedESPsAsMasters.ToggleData();

					break;
				case IDC_MAINMENU_SAVEOPTIONS_PREVENTCHANGESTOFILETIMESTAMPS:
					settings::plugins::kPreventTimeStampChanges.ToggleData();

					break;
				case IDC_MAINMENU_USEINFOLISTING:
					cliWrapper::interfaces::USE->ShowUseInfoListDialog(nullptr);
				case IDC_MAINMENU_SAVEOPTIONS_CREATEBACKUPBEFORESAVING:
					settings::versionControl::kBackupOnSave.ToggleData();

					break;
				case IDC_MAINMENU_SORTACTIVEFORMSFIRST:
					settings::dialogs::kSortFormListsByActiveForm.ToggleData();

					break;
				case IDC_MAINMENU_COLORIZEACTIVEFORMS:
					settings::dialogs::kColorizeActiveForms.ToggleData();

					break;
				case IDC_MAINMENU_COLORIZEFORMOVERRIDES:
					settings::dialogs::kColorizeFormOverrides.ToggleData();

					break;
				case IDC_MAINMENU_CODAOPENSCRIPTREPOSITORY:
					CODAVM->OpenScriptRepository();

					break;
				//case IDC_MAINMENU_PARENTCHILDINDICATORS:
				//	settings::renderer::kParentChildVisualIndicator.ToggleData();
				//	TESRenderWindow::Redraw();

				//	break;
				//case IDC_MAINMENU_PATHGRIDLINKEDREFINDICATORS:
				//	settings::renderer::kPathGridLinkedRefIndicator.ToggleData();
				//	TESRenderWindow::Redraw(true);

				//	break;
				//case IDC_MAINMENU_PATHGRIDLINKEDREFINDICATORSETTINGS_HIDEBOUNDINGBOX:
				//case IDC_MAINMENU_PATHGRIDLINKEDREFINDICATORSETTINGS_HIDELINKEDREFNODE:
				//case IDC_MAINMENU_PATHGRIDLINKEDREFINDICATORSETTINGS_HIDELINECONNECTOR:
				//	{
				//		UInt32 Flags = settings::renderer::kPathGridLinkedRefIndicatorFlags().u;
				//		UInt32 Comperand = 0;

				//		switch (LOWORD(wParam))
				//		{
				//		case IDC_MAINMENU_PATHGRIDLINKEDREFINDICATORSETTINGS_HIDEBOUNDINGBOX:
				//			Comperand = settings::renderer::kPathGridLinkedRefIndicatorFlag_HidePointBoundingBox;
				//			break;
				//		case IDC_MAINMENU_PATHGRIDLINKEDREFINDICATORSETTINGS_HIDELINKEDREFNODE:
				//			Comperand = settings::renderer::kPathGridLinkedRefIndicatorFlag_HideLinkedRefNode;
				//			break;
				//		case IDC_MAINMENU_PATHGRIDLINKEDREFINDICATORSETTINGS_HIDELINECONNECTOR:
				//			Comperand = settings::renderer::kPathGridLinkedRefIndicatorFlag_HideLineConnector;
				//			break;
				//		}

				//		if ((Flags & Comperand))
				//			Flags &= ~Comperand;
				//		else
				//			Flags |= Comperand;

				//		settings::renderer::kPathGridLinkedRefIndicatorFlags.SetUInt(Flags);
				//		if (settings::renderer::kPathGridLinkedRefIndicator().i == 0)
				//			TESRenderWindow::Redraw(true);
				//	}

				//	break;
				case IDC_MAINMENU_CODARELOADBACKGROUNDSCRIPTS:
					CODAVM->GetBackgroundDaemon()->Rebuild();
					break;
				case IDC_MAINMENU_CODARESETSCRIPTCACHE:
					CODAVM->GetProgramCache()->Invalidate();
					break;
				case IDC_MAINMENU_SYNCSCRIPTSTODISK:
					cliWrapper::interfaces::SE->ShowDiskSyncDialog();
					break;
				case IDC_MAINMENU_CUSTOMCOLORTHEMEMODE:
					if (BGSEEUI->GetColorThemer()->IsEnabled())
						BGSEEUI->GetColorThemer()->Disable();
					else
						BGSEEUI->GetColorThemer()->Enable();

					break;
				case IDC_MAINMENU_ONEDITORSTARTUP_LOADSTARTUPPLUGIN:
					settings::startup::kLoadPlugin.ToggleData();
					break;
				case IDC_MAINMENU_ONEDITORSTARTUP_SETSTARTUPWORKSPACE:
					settings::startup::kSetWorkspace.ToggleData();
					break;
				case IDC_MAINMENU_ONEDITORSTARTUP_OPENSTARTUPSCRIPT:
					settings::startup::kOpenScriptWindow.ToggleData();
					break;
				case IDC_MAINMENU_CSEMANUAL:
				case IDC_MAINMENU_CODAMANUAL:
					{
						std::string ManualPath = BGSEEMAIN->GetAPPPath();
						ManualPath += "\\Data\\Docs\\GECK Extender\\";
						if (LOWORD(wParam) == IDC_MAINMENU_CODAMANUAL)
							ManualPath += "Coda Manual.pdf";
						else
							ManualPath += "GECK Extender Manual.pdf";

						IFileStream File;
						if (File.Open(ManualPath.c_str()) == false)
						{
							BGSEEUI->MsgBoxI(hWnd, NULL, "Could not find the manual file. Did you extract the bundled "
											 "documentation into the following directory?\n\n"
											 "Data\\Docs\\GECK Extender\\");
						}
						else
							ShellExecute(nullptr, "open", ManualPath.c_str(), nullptr, nullptr, SW_SHOW);
					}
					break;
				default:
					SubclassParams->Out.MarkMessageAsHandled = false;

					break;
				}

				break;
			}

			return DlgProcResult;
		}

#define ID_PATHGRIDTOOLBARBUTTION_TIMERID		0x99

		LRESULT CALLBACK MainWindowMiscSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
													bgsee::WindowSubclassProcCollection::SubclassProcExtraParams* SubclassParams)
		{
			LRESULT DlgProcResult = FALSE;

			switch (uMsg)
			{
			case TESCSMain::kWindowMessage_Unk413:
				// if multiple editor instances are disabled, the secondary instance will send this message to the primary
				// strangely, when the primary instance has an active/open preview control, the call crashes (presumably due to a corrupt lParam pointer)
				// as far as I can tell, the corresponding code path is never executed, so we can consume this message entirely

				SubclassParams->Out.MarkMessageAsHandled = true;
				break;
			case WM_MAINWINDOW_INIT_DIALOG:
				{
					SubclassParams->Out.MarkMessageAsHandled = true;
				}

				break;
			case WM_DESTROY:
				break;
			case WM_MAINWINDOW_INIT_EXTRADATA:
				{
				;//
				}

				break;
			}

			return DlgProcResult;
		}

		void InitializeMainWindowOverrides()
		{
			BGSEEUI->GetSubclasser()->RegisterSubclassForWindow(BGSEEUI->GetMainWindow(), uiManager::MainWindowMenuInitSubclassProc);
			BGSEEUI->GetSubclasser()->RegisterSubclassForWindow(BGSEEUI->GetMainWindow(), uiManager::MainWindowMenuSelectSubclassProc);
			BGSEEUI->GetSubclasser()->RegisterSubclassForWindow(BGSEEUI->GetMainWindow(), uiManager::MainWindowMiscSubclassProc);

			SendMessage(BGSEEUI->GetMainWindow(), WM_MAINWINDOW_INIT_DIALOG, NULL, NULL);
		}

	}
}