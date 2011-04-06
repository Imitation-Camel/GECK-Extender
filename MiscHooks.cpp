#include "MiscHooks.h"
#include "SEHooks.h"
#include "[Common]/CLIWrapper.h"
#include "Exports.h"
#include "ExtenderInternals.h"
#include "[Common]/HandshakeStructs.h"
#include "WindowManager.h"
#include "resource.h"
#include "HallofFame.h"
#include "CSInterop.h"
#include "WindowManager.h"
#include "obse/GameData.h"


const char*						g_AssetSelectorReturnPath = NULL;
const char*						g_DefaultWaterTextureStr = "Water\\dungeonwater01.dds";
bool							g_QuickLoadToggle = false;
char							g_NumericIDWarningBuffer[0x10] = {0};

MemHdlr							kSavePluginMasterEnum				(0x0047ECC6, SavePluginMasterEnumHook, 0, 0);
NopHdlr							kCheckIsActivePluginAnESM			(0x0040B65E, 2);
NopHdlr							kTESFormGetUnUsedFormID				(0x00486C08, 2);
MemHdlr							kLoadPluginsProlog					(0x00485252, LoadPluginsPrologHook, 0, 0);
MemHdlr							kLoadPluginsEpilog					(0x004856B2, LoadPluginsEpilogHook, 0, 0);
MemHdlr							kDataDialogPluginDescription		(0x0040CAB6, (UInt32)0, 0, 0);
MemHdlr							kDataDialogPluginAuthor				(0x0040CAFE, (UInt32)0, 0, 0);
MemHdlr							kSavePluginCommonDialog				(0x00446D51, SavePluginCommonDialogHook, 0, 0);
NopHdlr							kResponseEditorMic					(0x00407F3D, 5);
MemHdlr							kDataHandlerPostError				(0x004852F0, (UInt32)0, 0, 0);
MemHdlr							kExitCS								(0x00419354, ExitCSHook, 0, 0);
MemHdlr							kFindTextInit						(0x00419A42, FindTextInitHook, 0, 0);
MemHdlr							kMessagePumpInit					(0x0041CF6F, MessagePumpInitHook, MakeUInt8Array(5, 0x8B, 0x3D, 0x58, 0x43, 0x92), 5);
MemHdlr							kCSInit								(0x00419260, CSInitHook, MakeUInt8Array(5, 0xE8, 0xEB, 0xC5, 0x2C, 0), 5);
MemHdlr							kUseInfoListInit					(0x00419833, UseInfoListInitHook, 0, 0);
NopHdlr							kMissingTextureWarning				(0x0044F3AF, 14);
MemHdlr							kTopicResultScriptReset				(0x004F49A0, 0x004F49FA, 0, 0);
MemHdlr							kNPCFaceGen							(0x004D76AC, NPCFaceGenHook, 0, 0);
MemHdlr							kDefaultWaterTextureFix				(0x0047F792, (UInt32)0, 0, 0);
MemHdlr							kDataDlgInit						(0x0040C6D7, DataDlgInitHook, 0, 0);
MemHdlr							kQuickLoadPluginLoadHandlerPrologue	(0x0040D073, QuickLoadPluginLoadHandlerPrologueHook, 0, 0);
MemHdlr							kQuickLoadPluginLoadHandler			(0x004852E5, QuickLoadPluginLoadHandlerHook, 0, 0);
MemHdlr							kMissingMasterOverride				(0x00484FC9, 0x00484E8E, 0, 0);
MemHdlr							kAssertOverride						(0x004B5670, AssertOverrideHook, 0, 0);
MemHdlr							kTextureMipMapCheck					(0x0044F49B, (UInt32)0, 0, 0);
MemHdlr							kUnnecessaryDialogEdits				(0x004EDFF7, (UInt32)0, 0, 0);
MemHdlr							kUnnecessaryCellEdits				(0x005349A5, (UInt32)0, 0, 0);
MemHdlr							kCustomCSWindow						(0x004311E5, CustomCSWindowPatchHook, 0, 0);
MemHdlr							kRaceDescriptionDirtyEdit			(0x0049405C, (UInt32)0, 0, 0);
MemHdlr							kPluginSave							(0x0041BBCD, PluginSaveHook, 0, 0);
MemHdlr							kPluginLoad							(0x0041BEFA, PluginLoadHook, 0, 0);
MemHdlr							kAddListViewItem					(0x004038F0, AddListViewItemHook, 0, 0);
MemHdlr							kAddComboBoxItem					(0x00403540, AddComboBoxItemHook, 0, 0);
MemHdlr							kObjectListPopulateListViewItems	(0x00413980, ObjectListPopulateListViewItemsHook, 0, 0);
MemHdlr							kCellViewPopulateObjectList			(0x004087C0, CellViewPopulateObjectListHook, 0, 0);
MemHdlr							kDoorMarkerProperties				(0x00429EA1, DoorMarkerPropertiesHook, 0, 0);
MemHdlr							kAutoLoadActivePluginOnStartup		(0x0041A26A, AutoLoadActivePluginOnStartupHook, MakeUInt8Array(6, 0x8B, 0x0D, 0x44, 0xB6, 0xA0, 0x0), 6);
MemHdlr							kDataHandlerClearData				(0x0047AE76, DataHandlerClearDataHook, 0, 0);
MemHdlr							kCellObjectListShadeMeRefAppend		(0x00445128, CellObjectListShadeMeRefAppendHook, 0, 0);
MemHdlr							kDeathToTheCloseOpenDialogsMessage	(0x0041BAA7, (UInt32)0, 0, 0);
MemHdlr							kTopicInfoCopyProlog				(0x004F0738, 0x004F07C4, 0, 0);
MemHdlr							kTopicInfoCopyEpilog				(0x004F1280, TopicInfoCopyEpilogHook, 0, 0);
MemHdlr							kTESDialogPopupMenu					(0x004435A6, TESDialogPopupMenuHook, 0, 0);
MemHdlr							kResponseWindowLipButtonPatch		(0x004EC0E7, 0x004EC0F7, 0, 0);
MemHdlr							kResponseWindowInit					(0x004EBA81, ResponseWindowInitHook, 0, 0);
MemHdlr							kNumericEditorID					(0x00497670, NumericEditorIDHook, 0, 0);
MemHdlr							kDataHandlerConstructSpecialForms	(0x00481049, DataHandlerConstructSpecialFormsHook, 0, 0);
MemHdlr							kResultScriptSaveForm				(0x004FD258, ResultScriptSaveFormHook, 0, 0);
MemHdlr							kDataDlgZOrder						(0x0040C530, 0x0040C552, 0, 0);
MemHdlr							kFormIDListViewInit					(0x00448A8A, FormIDListViewInitHook, 0, 0);
MemHdlr							kFormIDListViewSaveChanges			(0x0044957A, FormIDListViewSaveChangesHook, 0, 0);
MemHdlr							kFormIDListViewItemChange			(0x00448DEC, FormIDListViewItemChangeHook, 0, 0);
MemHdlr							kFormIDListViewSelectItem			(0x00403B3D, FormIDListViewSelectItemHook, 0, 0);
MemHdlr							kFormIDListViewDuplicateSelection	(0x004492AE, FormIDListViewDuplicateSelectionHook, 0, 0);
MemHdlr							kTESRaceCopyHairEyeDataInit			(0x004E9735, TESRaceCopyHairEyeDataInitHook, 0, 0);
MemHdlr							kTESRaceCopyHairEyeDataMessageHandler
																	(0x004E8FE1, TESRaceCopyHairEyeDataMessageHandlerHook, 0, 0);
NopHdlr							kTESDialogSubwindowEnumChildCallback
																	(0x00404E69, 3);
MemHdlr							kTESObjectREFRDoCopyFrom			(0x0054763D, TESObjectREFRDoCopyFromHook, 0, 0);
NopHdlr							kLODLandTextureMipMapLevelA			(0x00411008, 2);
MemHdlr							kLODLandTextureMipMapLevelB			(0x005E0306, LODLandTextureMipMapLevelBHook, 0, 0);
MemHdlr							kLODLandTextureResolution			(0x00410D08, LODLandTextureResolutionHook, 0, 0);
MemHdlr							kDataHandlerSaveFormToFile			(0x00479181, DataHandlerSaveFormToFileHook, 0, 0);
MemHdlr							kTESFileUpdateHeader				(0x004894D0, TESFileUpdateHeaderHook, 0, 0);
MemHdlr							kTESObjectREFRGet3DData				(0x00542950, TESObjectREFRGet3DDataHook, 0, 0);
MemHdlr							kNiWindowRender						(0x00406442, NiWindowRenderHook, 0, 0);
MemHdlr							kNiDX9RendererRecreate				(0x006D7260, NiDX9RendererRecreateHook, 0, 0);
MemHdlr							kRenderWindowStats					(0x0042D3F4, RenderWindowStatsHook, 0, 0);
MemHdlr							kUpdateViewport						(0x0042CE70, UpdateViewportHook, 0, 0);
MemHdlr							kRenderWindowSelection				(0x0042AE71, RenderWindowSelectionHook, 0, 0);
MemHdlr							kDataHandlerSavePluginEpilog		(0x0047F136, DataHandlerSavePluginEpilogHook, 0, 0);
MemHdlr							kTESFileUpdateHeaderFlagBit			(0x00489570, TESFileUpdateHeaderFlagBitHook, 0, 0);
MemHdlr							kTESObjectCELLSaveReferencesProlog	(0x00538860, TESObjectCELLSaveReferencesPrologHook, 0, 0); 
MemHdlr							kTESObjectCELLSaveReferencesEpilog	(0x005389DB, TESObjectCELLSaveReferencesEpilogHook, 0,0);
MemHdlr							kTESDialogGetIsWindowDragDropRecipient
																	(0x004433FF, TESDialogGetIsWindowDragDropRecipientHook, 0, 0);
MemHdlr							kTESDialogShowDDSCommonDialogProlog		(0x004A414B, TESDialogShowDDSCommonDialogPrologHook, 0, 0);
MemHdlr							kTESDialogShowNIFCommonDialogProlog		(0x0049BDAB, TESDialogShowNIFCommonDialogPrologHook, 0, 0);
MemHdlr							kTESDialogShowDDSCommonDialogEpilog		(0x00446CDC, TESDialogShowDDSCommonDialogEpilogHook, 0, 0);
MemHdlr							kTESDialogShowNIFCommonDialogEpilog		(0x00446C94, TESDialogShowNIFCommonDialogEpilogHook, 0, 0);

bool PatchMiscHooks()
{
	kLoadPluginsProlog.WriteJump();
	kLoadPluginsEpilog.WriteJump();
	kSavePluginCommonDialog.WriteJump();
	kSavePluginMasterEnum.WriteJump();
	kExitCS.WriteJump();
	kFindTextInit.WriteJump();
	kUseInfoListInit.WriteJump();
	kCSInit.WriteJump();
	kQuickLoadPluginLoadHandlerPrologue.WriteJump();
	kQuickLoadPluginLoadHandler.WriteJump();
	kDataDlgInit.WriteJump();
	kNPCFaceGen.WriteJump();
	kMissingMasterOverride.WriteJump();
	if (g_INIManager->FetchSetting("LogCSWarnings")->GetValueAsInteger())
	PatchMessageHandler();
	if (g_INIManager->FetchSetting("LogAssertions")->GetValueAsInteger())
	kAssertOverride.WriteJump();
	kCustomCSWindow.WriteJump();
	kPluginSave.WriteJump();
	kPluginLoad.WriteJump();
	kAddListViewItem.WriteJump();
	kObjectListPopulateListViewItems.WriteJump();
	kCellViewPopulateObjectList.WriteJump();
	kTopicResultScriptReset.WriteJump();
	kDoorMarkerProperties.WriteJump();
	kDataHandlerPostError.WriteUInt8(0xEB);	
	kDataDialogPluginDescription.WriteUInt8(0xEB);
	kDataDialogPluginAuthor.WriteUInt8(0xEB);
	kDefaultWaterTextureFix.WriteUInt32((UInt32)g_DefaultWaterTextureStr);
	kTextureMipMapCheck.WriteUInt8(0xEB);
	kUnnecessaryCellEdits.WriteUInt8(0xEB);
	kUnnecessaryDialogEdits.WriteUInt8(0xEB);
	kRaceDescriptionDirtyEdit.WriteUInt8(0xEB);
	kCheckIsActivePluginAnESM.WriteNop();
	kMissingTextureWarning.WriteNop();
	kResponseEditorMic.WriteNop(); 
	kTESFormGetUnUsedFormID.WriteNop();
	kDataHandlerClearData.WriteJump();
	kCellObjectListShadeMeRefAppend.WriteJump();
	kDeathToTheCloseOpenDialogsMessage.WriteUInt8(0xEB);
	kTopicInfoCopyProlog.WriteJump();
	kTopicInfoCopyEpilog.WriteJump();
	kTESDialogPopupMenu.WriteJump();
	kResponseWindowLipButtonPatch.WriteJump();
	kResponseWindowInit.WriteJump();
	kNumericEditorID.WriteJump();
	kDataHandlerConstructSpecialForms.WriteJump();
	kResultScriptSaveForm.WriteJump();
	kDataDlgZOrder.WriteJump();
	kFormIDListViewInit.WriteJump();
	kFormIDListViewSaveChanges.WriteJump();
	kFormIDListViewItemChange.WriteJump();
	kFormIDListViewSelectItem.WriteJump();
	kFormIDListViewDuplicateSelection.WriteJump();
	kTESRaceCopyHairEyeDataInit.WriteJump();
	kTESRaceCopyHairEyeDataMessageHandler.WriteJump();
	kTESObjectREFRDoCopyFrom.WriteJump();
	kLODLandTextureMipMapLevelA.WriteNop();
	kLODLandTextureMipMapLevelB.WriteJump();
//	kLODLandTextureResolution.WriteJump();
	kDataHandlerSaveFormToFile.WriteJump();
	kTESFileUpdateHeader.WriteJump();
	kTESObjectREFRGet3DData.WriteJump();
	kNiWindowRender.WriteJump();
	kNiDX9RendererRecreate.WriteJump();
	kRenderWindowStats.WriteJump();
	kUpdateViewport.WriteJump();
	kRenderWindowSelection.WriteJump();
	kDataHandlerSavePluginEpilog.WriteJump();
	kTESFileUpdateHeaderFlagBit.WriteJump();
	kTESObjectCELLSaveReferencesProlog.WriteJump();
	kTESObjectCELLSaveReferencesEpilog.WriteJump();
	kTESDialogGetIsWindowDragDropRecipient.WriteJump();

//	PatchCommonDialogCancelHandler(Model);
	PatchCommonDialogCancelHandler(Animation);
	PatchCommonDialogCancelHandler(Sound);
//	PatchCommonDialogCancelHandler(Texture);
	PatchCommonDialogCancelHandler(SPT);

//	PatchCommonDialogPrologHandler(Model);
	PatchCommonDialogPrologHandler(Animation);
	PatchCommonDialogPrologHandler(Sound);
//	PatchCommonDialogPrologHandler(Texture);
	PatchCommonDialogPrologHandler(SPT);

//	PatchCommonDialogEpilogHandler(Model);
	PatchCommonDialogEpilogHandler(Animation);
	PatchCommonDialogEpilogHandler(Sound);
//	PatchCommonDialogEpilogHandler(Texture);
	PatchCommonDialogEpilogHandler(SPT);

	kTESDialogShowDDSCommonDialogProlog.WriteJump();
	kTESDialogShowNIFCommonDialogProlog.WriteJump();
	kTESDialogShowDDSCommonDialogEpilog.WriteJump();
	kTESDialogShowNIFCommonDialogEpilog.WriteJump();


	if (CreateDirectory(std::string(g_AppPath + "Data\\Backup").c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
		DebugPrint("Couldn't create the Backup folder in Data directory");

	OSVERSIONINFO OSInfo;
	GetVersionEx(&OSInfo);
	if (OSInfo.dwMajorVersion >= 6)		// if running on Windows Vista/7, fix the listview selection sound
		RegDeleteKey(HKEY_CURRENT_USER , "AppEvents\\Schemes\\Apps\\.Default\\CCSelect\\.Current");	
	
	return true;
}

void PatchMessageHandler(void)
{
	SafeWrite32(kVTBL_MessageHandler + 0, (UInt32)&MessageHandlerOverride);
	SafeWrite32(kVTBL_MessageHandler + 0x4, (UInt32)&MessageHandlerOverride);
	SafeWrite32(kVTBL_MessageHandler + 0x8, (UInt32)&MessageHandlerOverride);
	SafeWrite32(kVTBL_MessageHandler + 0x10, (UInt32)&MessageHandlerOverride);
	SafeWrite32(kVTBL_MessageHandler + 0x14, (UInt32)&MessageHandlerOverride);
	SafeWrite32(kVTBL_MessageHandler + 0x18, (UInt32)&MessageHandlerOverride);
	SafeWrite32(kVTBL_MessageHandler + 0x1C, (UInt32)&MessageHandlerOverride);
	SafeWrite32(kVTBL_MessageHandler + 0x20, (UInt32)&MessageHandlerOverride);
	SafeWrite32(kVTBL_MessageHandler + 0x24, (UInt32)&MessageHandlerOverride);

													// patch spammy subroutines
	NopHdlr kDataHandlerAutoSave(0x0043083B, 5);
	NopHdlr	kAnimGroupNote(0x004CA21D, 5);
	NopHdlr kTangentSpaceCreation(0x0076989C, 5);
	NopHdlr	kHeightMapGenA(0x005E0D9D, 5), kHeightMapGenB(0x005E0DB6, 5);
	NopHdlr kModelLoadError(0x0046C215, 5);

	SafeWrite8(0x00468597, 0xEB);					//		FileFinder::LogMessage
	kDataHandlerAutoSave.WriteNop();
	kAnimGroupNote.WriteNop();
	kTangentSpaceCreation.WriteNop();
	kHeightMapGenA.WriteNop();
	kHeightMapGenB.WriteNop();
	kModelLoadError.WriteNop();
}


UInt32 __stdcall IsControlKeyDown(void)
{
	return GetAsyncKeyState(VK_CONTROL);
}

bool __stdcall InitTESFileSaveDlg()
{
	return DialogBox(g_DLLInstance, MAKEINTRESOURCE(DLG_TESFILE), *g_HWND_CSParent, (DLGPROC)TESFileDlgProc);
}

void __stdcall MessageHandlerOverride(const char* Message)
{
	DebugPrint(Console::e_CS, "%s", Message);
}

void __declspec(naked) SavePluginCommonDialogHook(void)
{
	static const UInt32			kSavePluginCommonDialogESMRetnAddr = 0x00446D58;
	static const UInt32			kSavePluginCommonDialogESPRetnAddr = 0x00446D69;
	_asm
	{
		pushad
		call	InitTESFileSaveDlg
		test	eax, eax
		jnz		ESM

		popad
		jmp		[kSavePluginCommonDialogESPRetnAddr]
	ESM:
		popad
		jmp		[kSavePluginCommonDialogESMRetnAddr]
	}
}


void __stdcall DoLoadPluginsPrologHook(void)
{
	ModEntry::Data* ActiveFile = (*g_dataHandler)->unk8B8.activeFile;

	if (ActiveFile && (ActiveFile->flags & ModEntry::Data::kFlag_IsMaster))
	{
		ToggleFlag(&ActiveFile->flags, ModEntry::Data::kFlag_IsMaster, 0);
	}

	sprintf_s(g_NumericIDWarningBuffer, 0x10, "%s", g_INIManager->GET_INI_STR("ShowNumericEditorIDWarning"));
	static const char* Zero = "0";
	g_INIManager->FetchSetting("ShowNumericEditorIDWarning")->SetValue(Zero);
}

void __declspec(naked) LoadPluginsPrologHook(void)
{
	static const UInt32			kLoadPluginsPrologRetnAddr = 0x00485257;
	static const UInt32			kLoadPluginsPrologCallAddr = 0x00431310;
	__asm
	{
		pushad
		call	DoLoadPluginsPrologHook
		popad

		call	[kLoadPluginsPrologCallAddr]
		jmp		[kLoadPluginsPrologRetnAddr]
	}
}

void __stdcall DoLoadPluginsEpilogHook(void)
{
	g_INIManager->FetchSetting("ShowNumericEditorIDWarning")->SetValue(g_NumericIDWarningBuffer);
}

void __declspec(naked) LoadPluginsEpilogHook(void)
{
	static const UInt32			kLoadPluginsEpilogRetnAddr = 0x004856B7;
	static const UInt32			kLoadPluginsEpilogCallAddr = 0x0047DA60;
	__asm
	{
		pushad
		call	DoLoadPluginsEpilogHook
		popad

		call	[kLoadPluginsEpilogCallAddr]
		jmp		[kLoadPluginsEpilogRetnAddr]		
	}
}


bool __stdcall DoSavePluginMasterEnumHook(ModEntry::Data* CurrentFile)
{
	if ((CurrentFile->flags & ModEntry::Data::kFlag_Loaded) == 0)
		return false;
	else
		return true;
}

void __declspec(naked) SavePluginMasterEnumHook(void)
{
	static const UInt32			kSavePluginMasterEnumRetnPassAddr = 0x0047ECCF;
	static const UInt32			kSavePluginMasterEnumRetnFailAddr = 0x0047ECEB;
	__asm
	{
		pushad
		push	ecx
		call	DoSavePluginMasterEnumHook
		test	al, al
		jz		SKIP

		popad
		jmp		[kSavePluginMasterEnumRetnPassAddr]
	SKIP:
		popad
		jmp		[kSavePluginMasterEnumRetnFailAddr]
	}
}


void __stdcall DoExitCSHook(HWND MainWindow)
{
	WritePositionToINI(MainWindow, NULL);
	WritePositionToINI(*g_HWND_CellView, "Cell View");
	WritePositionToINI(*g_HWND_ObjectWindow, "Object Window");
	WritePositionToINI(*g_HWND_RenderWindow, "Render Window");

	RENDERTEXT->Release();
	CSIOM->Deinitialize();

	CONSOLE->Deinitialize();
	g_INIManager->SaveSettingsToINI();

	ExitProcess(0);
}

void __declspec(naked) ExitCSHook(void)
{
	static const UInt32			kExitCSJumpAddr = 0x004B52C1;
	__asm
	{
		push    ebx
		call    DoExitCSHook
	}
}


void __stdcall DoFindTextInitHook(HWND FindTextDialog)
{
	g_FindTextOrgWindowProc = (WNDPROC)SetWindowLong(FindTextDialog, GWL_WNDPROC, (LONG)FindTextDlgSubClassProc);
}

void __declspec(naked) FindTextInitHook(void)
{
	static const UInt32			kFindTextInitRetnAddr = 0x00419A48;
	__asm
	{
		call	CreateDialogParamAddress
		call	[g_WindowHandleCallAddr]			// CreateDialogParamA

		pushad
		push	eax	
		call	DoFindTextInitHook
		popad

		jmp		[kFindTextInitRetnAddr]
	}
}

void __declspec(naked) UseInfoListInitHook(void)
{
	static const UInt32			kUseInfoListInitRetnAddr = 0x00419848;
	__asm
	{
		push	0
		call	CLIWrapper::UseInfoList::OpenUseInfoBox
		jmp		[kUseInfoListInitRetnAddr]
	}
}

void __stdcall DoCSInitHook()
{
	if (!g_PluginPostLoad) 
		return;
											// prevents the hook from being called before the full init
											// perform deferred patching
											// remove hook rightaway to keep it from hindering the subclassing that follows
	kCSInit.WriteBuffer();

	InitializeWindowManager();
//	InitializeDefaultGMSTMap();
	CLIWrapper::ScriptEditor::InitializeDatabaseUpdateTimer();
	HallOfFame::Initialize(true);
	CONSOLE->InitializeConsole();
	CONSOLE->LoadINISettings();
	g_RenderTimeManager.Update();

	DebugPrint("Initializing RenderWindowTextPainter");
	RENDERTEXT->Initialize();

	RENDERTEXT->QueueDrawTask(RenderWindowTextPainter::kRenderChannel_2, "Construction Set Extender", 5);

	if (g_INIManager->GET_INI_INT("LoadPluginOnStartup"))
		LoadStartupPlugin();

	if (g_INIManager->GET_INI_INT("OpenScriptWindowOnStartup"))
	{
		const char* ScriptID = g_INIManager->GET_INI_STR("StartupScriptEditorID");
		if (strcmp(ScriptID, "") && GetFormByID(ScriptID))
			SpawnCustomScriptEditor(ScriptID);
		else
			SendMessage(*g_HWND_CSParent, WM_COMMAND, 0x9CE1, 0);
	}

	LoadedMasterArchives();
}


void __declspec(naked) CSInitHook(void)
{
	static const UInt32			kCSInitRetnAddr = 0x00419265;
	static const UInt32			kCSInitCallAddr = 0x006E5850;
	__asm
	{
		call	[kCSInitCallAddr]
		call	DoCSInitHook
		jmp		[kCSInitRetnAddr]
	}
}

void __declspec(naked) MessagePumpInitHook(void)
{
	static const UInt32			kMessagePumpInitRetnAddr = 0x0041CF75;
	__asm
	{
		mov		g_PluginPostLoad, 1

		_emit	0x8B
		_emit	0x3D
		_emit	0x58
		_emit	0x43
		_emit	0x92
		_emit	0

		jmp		[kMessagePumpInitRetnAddr]
	}
}



DefineCommonDialogCancelHandler(Model)
DefineCommonDialogCancelHandler(Animation)
DefineCommonDialogCancelHandler(Sound)
DefineCommonDialogCancelHandler(Texture)
DefineCommonDialogCancelHandler(SPT)

UInt32 __stdcall InitBSAViewer(UInt32 Filter)
{
	switch (Filter)
	{
	case e_NIF:
		g_AssetSelectorReturnPath = CLIWrapper::BSAViewer::InitializeViewer(g_AppPath.c_str(), "nif");
		break;
	case e_KF:
		g_AssetSelectorReturnPath = CLIWrapper::BSAViewer::InitializeViewer(g_AppPath.c_str(), "kf");
		break;
	case e_WAV:
		g_AssetSelectorReturnPath = CLIWrapper::BSAViewer::InitializeViewer(g_AppPath.c_str(), "wav");
		break;
	case e_DDS:
		g_AssetSelectorReturnPath = CLIWrapper::BSAViewer::InitializeViewer(g_AppPath.c_str(), "dds");
		break;
	case e_SPT:
		g_AssetSelectorReturnPath = CLIWrapper::BSAViewer::InitializeViewer(g_AppPath.c_str(), "spt");
		break;
	}

	if (!g_AssetSelectorReturnPath)
		return 0;
	else
		return e_FetchPath;
}

UInt32 __stdcall InitPathEditor(int ID, const char* ExistingPath, HWND Dialog)
{
	if (!ExistingPath)
		GetDlgItemText(Dialog, ID, g_Buffer, sizeof(g_Buffer));
	else
		PrintToBuffer("%s", ExistingPath);

	g_AssetSelectorReturnPath = (const char*)DialogBoxParam(g_DLLInstance, MAKEINTRESOURCE(DLG_TEXTEDIT), Dialog, (DLGPROC)TextEditDlgProc, (LPARAM)g_Buffer);

	if (!g_AssetSelectorReturnPath)
		return 0;
	else
		return e_FetchPath;
}

UInt32 __stdcall InitPathCopier(UInt32 Filter, HWND Dialog)
{
	g_AssetSelectorReturnPath = (const char*)DialogBoxParam(g_DLLInstance, MAKEINTRESOURCE(DLG_COPYPATH), Dialog, (DLGPROC)CopyPathDlgProc, (LPARAM)Filter);
	if (!g_AssetSelectorReturnPath)
		return 0;
	else
		return e_FetchPath;
}

UInt32 __stdcall InitAssetSelectorDlg(HWND Dialog)
{
	return DialogBox(g_DLLInstance, MAKEINTRESOURCE(DLG_ASSETSEL), Dialog, (DLGPROC)AssetSelectorDlgProc);
}

DefineCommonDialogPrologHandler(Model)
DefineCommonDialogPrologHandler(Animation)
DefineCommonDialogPrologHandler(Sound)
DefineCommonDialogPrologHandler(Texture)
DefineCommonDialogPrologHandler(SPT)

void __declspec(naked) ModelPostCommonDialogHook(void)     
{
    __asm
    {
		cmp		eax, e_FetchPath	
		jz		SELECT

		lea		eax, [esp + 0x14]
        jmp     [kModelPostCommonDialogRetnAddr]
	SELECT:
		mov		eax, g_AssetSelectorReturnPath
        jmp     [kModelPostCommonDialogRetnAddr]
    }
}
void __declspec(naked) AnimationPostCommonDialogHook(void)     
{
    __asm
    {
		mov		ebx, eax

		mov     eax, [esi + 0x24]
		push    ebx
		push    eax
		lea     ecx, [ebp - 0x14]
		mov     byte ptr [ebp - 0x4], 1
		call    kBSString_Set

		cmp		ebx, e_FetchPath
		jz		SELECT

		lea		edx, [ebp]
        jmp		POST
	SELECT:
		mov		edx, g_AssetSelectorReturnPath
	POST:
		push	edx
		lea		ecx, [esp + 0x24]
        jmp     [kAnimationPostCommonDialogRetnAddr]
    }
}
void __declspec(naked) SoundPostCommonDialogHook(void)     
{
    __asm
    {
		cmp		eax, e_FetchPath	
		jz		SELECT

		lea		ecx, [esp + 8]
		push	ecx
        jmp     [kSoundPostCommonDialogRetnAddr]
	SELECT:
		mov		ecx, g_AssetSelectorReturnPath
		push	ecx
        jmp     [kSoundPostCommonDialogRetnAddr]
    }
}
void __declspec(naked) TexturePostCommonDialogHook(void)     
{
    __asm
    {
		cmp		eax, e_FetchPath
		jz		SELECT

		lea		eax, [ebp]
        jmp		POST
	SELECT:
		mov		eax, g_AssetSelectorReturnPath
	POST:
		push	eax
		lea		ecx, [ebp - 0x14]
        jmp     [kTexturePostCommonDialogRetnAddr]
    }
}
void __declspec(naked) SPTPostCommonDialogHook(void)     
{
    __asm
    {
		cmp		eax, e_FetchPath
		jz		SELECT

		lea		ecx, [esp + 0x14]
        jmp     [kSPTPostCommonDialogRetnAddr]
	SELECT:
		mov		ecx, g_AssetSelectorReturnPath
        jmp     [kSPTPostCommonDialogRetnAddr]
    }
}

void __stdcall DoNPCFaceGenHook(HWND Dialog)
{
	SendMessageA(Dialog, WM_COMMAND, 1014, 0);
}

void __declspec(naked) NPCFaceGenHook(void)     
{
	static const UInt32			kNPCFaceGenRetnAddr = 0x004D76B1;
	static const UInt32			kNPCFaceGenCallAddr = 0x0049C230;
    __asm
    {
		call	[kNPCFaceGenCallAddr]
		push	esi
		call	DoNPCFaceGenHook
		jmp		[kNPCFaceGenRetnAddr]
	}
}

void __stdcall DoQuickLoadPluginLoadHandlerPrologueHook(HWND DataDlg)
{
	if (IsDlgButtonChecked(DataDlg, DATA_QUICKLOAD) == BST_CHECKED)
		g_QuickLoadToggle = true;
	else
		g_QuickLoadToggle = false;
}

void __declspec(naked) QuickLoadPluginLoadHandlerPrologueHook(void)
{
	static const UInt32			kQuickLoadPluginLoadHandlerPrologueCallAddr = 0x0040CA30;
	static const UInt32			kQuickLoadPluginLoadHandlerPrologueRetnAddr = 0x0040D078;
	__asm
	{
		pushad
		push	edi
		call	DoQuickLoadPluginLoadHandlerPrologueHook
		popad
		call	[kQuickLoadPluginLoadHandlerPrologueCallAddr]
		jmp		[kQuickLoadPluginLoadHandlerPrologueRetnAddr]
	}
}

bool __stdcall DoQuickLoadPluginLoadHandlerHook(ModEntry::Data* CurrentFile)
{
	return _stricmp(CurrentFile->name, (*g_dataHandler)->unk8B8.activeFile->name);
}

void __declspec(naked) QuickLoadPluginLoadHandlerHook(void)
{
	static const UInt32			kQuickLoadPluginLoadHandlerCallAddr = 0x00484A60;		// f_DataHandler::LoadTESFile
	static const UInt32			kQuickLoadPluginLoadHandlerRetnAddr = 0x004852EE;
	static const UInt32			kQuickLoadPluginLoadHandlerSkipAddr = 0x004852F0;
	__asm
	{
		pushad
		mov		al, g_QuickLoadToggle
		test	al, al
		jz		CONTINUE
		push	edx
		call	DoQuickLoadPluginLoadHandlerHook
		test	eax, eax
		jnz		SKIP
	CONTINUE:
		popad

		push	ecx
		push	edx
		mov		ecx, edi
		call	[kQuickLoadPluginLoadHandlerCallAddr]
		jmp		[kQuickLoadPluginLoadHandlerRetnAddr]
	SKIP:
		popad
		jmp		[kQuickLoadPluginLoadHandlerSkipAddr]
	}
}

void __stdcall DoDataDlgInitHook(HWND DataDialog)
{
	// create new controls
	RECT DlgRect;
	GetClientRect(DataDialog, &DlgRect);

	HWND QuickLoadCheckBox = CreateWindowEx(0, 
											"BUTTON", 
											"Quick-Load Plugin", 
											BS_AUTOCHECKBOX|WS_CHILD|WS_VISIBLE|WS_TABSTOP,
											DlgRect.right - 141, DlgRect.bottom - 82, 142, 15, 
											DataDialog, 
											(HMENU)DATA_QUICKLOAD, 
											GetModuleHandle(NULL), 
											NULL),
		 StartupPluginBtn = CreateWindowEx(0, 
											"BUTTON", 
											"Set As Startup Plugin", 
											WS_CHILD|WS_VISIBLE|WS_TABSTOP,
											DlgRect.right - 141, DlgRect.bottom - 64, 130, 20, 
											DataDialog, 
											(HMENU)DATA_SETSTARTUPPLUGIN, 
											GetModuleHandle(NULL), 
											NULL);

	CheckDlgButton(DataDialog, DATA_QUICKLOAD, (!g_QuickLoadToggle ? BST_UNCHECKED : BST_CHECKED));

	SendMessage(QuickLoadCheckBox, WM_SETFONT, (WPARAM)g_CSDefaultFont, TRUE);
	SendMessage(StartupPluginBtn, WM_SETFONT, (WPARAM)g_CSDefaultFont, TRUE);

	g_DataDlgOrgWindowProc = (WNDPROC)SetWindowLong(DataDialog, GWL_WNDPROC, (LONG)DataDlgSubClassProc);
	SetWindowPos(DataDialog, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
}

void __declspec(naked) DataDlgInitHook(void)
{
	static const UInt32			kDataDlgInitRetnAddr = 0x0040C6DC;
	static const UInt32			kDataDlgInitCallAddr = 0x00404A90;
	__asm
	{
		call	[kDataDlgInitCallAddr]

		pushad
		push	esi
		call	DoDataDlgInitHook
		popad

		jmp		[kDataDlgInitRetnAddr]
	}
}

void __stdcall DoAssertOverrideHook(UInt32 EIP)
{
	DebugPrint("\t\tAssert call handled at 0x%08X !", EIP);
	MessageBeep(MB_ICONHAND);
}

void __declspec(naked) AssertOverrideHook(void)
{
	static const UInt32			kAssertOverrideRetnAddr = 0x004B575E;
	__asm
	{
		mov		eax, [esp]
		sub		eax, 5
		pushad
		push	eax
		call	DoAssertOverrideHook
		popad

		jmp		[kAssertOverrideRetnAddr]
	}
}

bool __stdcall DoCustomCSWindowPatchHook(HWND Window)
{
	if (g_CustomMainWindowChildrenDialogs.GetHandleExists(Window))
		return false;
	else
		return true;
}

void __declspec(naked) CustomCSWindowPatchHook(void)
{
	static const UInt32			kCustomCSWindowPatchRetnAddr = 0x004311EF;
	__asm
	{
		mov		edi, [g_HWND_CSParent]
		cmp		eax, edi
		jnz		FAIL
		xor		edi, edi
		pushad
		push	esi
		call	DoCustomCSWindowPatchHook
		test	eax, eax
		jz		FAIL
		popad
		mov		edi, esi
	FAIL:
		jmp		[kCustomCSWindowPatchRetnAddr]
	}

}

void __declspec(naked) PluginSaveHook(void)
{
	static const UInt32			kPluginSaveRetnAddr	=	0x0041BBD3;
    __asm
    {
		call	SetWindowTextAddress
		call	[g_WindowHandleCallAddr]				// SetWindowTextA
		pushad
		push	10
		call	SendPingBack
		popad
		jmp		[kPluginSaveRetnAddr]
    }
}

void __declspec(naked) PluginLoadHook(void)
{
	static const UInt32			kPluginLoadRetnAddr	=	0x0041BEFF;
    __asm
    {
		call	InitializeCSWindows

		pushad
		push	9
		call	SendPingBack
		call	FormEnumerationWrapper::ResetFormVisibility
		popad

		jmp		[kPluginLoadRetnAddr]
    }
}

void __declspec(naked) AddListViewItemHook(void)
{
	static const UInt32			kAddListViewItemRetnAddr = 0x004038F7;
	static const UInt32			kAddListViewItemExitAddr = 0x0040396E;
    __asm
    {
		mov		eax, [esp]
		sub		eax, 5
		pushad
		push	eax
		call	FormEnumerationWrapper::PerformListViewPrologCheck
		test	al, al
		jz		SKIP
		popad

		mov		eax, [esp + 8]
		pushad
		push	eax
		call	FormEnumerationWrapper::GetShouldEnumerateForm
		test	al, al
		jz		EXIT
	SKIP:
		popad

		mov		ecx, [esp + 0x10]
		or		edx, 0x0FFFFFFFF
		jmp		[kAddListViewItemRetnAddr]
	EXIT:
		popad
		jmp		[kAddListViewItemExitAddr]
    }
}

void __declspec(naked) AddComboBoxItemHook(void)
{
	static const UInt32			kAddComboBoxItemRetnAddr = 0x00403548;
	static const UInt32			kAddComboBoxItemExitAddr = 0x004035F4;
    __asm
    {
		pushad
		push	[esp + 0xC]
		call	FormEnumerationWrapper::GetShouldEnumerateForm
		test	al, al
		jz		EXIT
		popad

		sub		esp, 8
		push	esi
		mov		esi, [esp + 0x10]
		jmp		[kAddComboBoxItemRetnAddr]
	EXIT:
		popad
		jmp		[kAddComboBoxItemExitAddr]
    }
}

void __declspec(naked) ObjectListPopulateListViewItemsHook(void)
{
	static const UInt32			kObjectListPopulateListViewItemsRetnAddr = 0x0041398A;
	static const UInt32			kObjectListPopulateListViewItemsExitAddr = 0x00413A50;
	__asm
	{
		mov		eax, [esp + 8]
		pushad
		push	eax
		call	FormEnumerationWrapper::GetShouldEnumerateForm
		test	al, al
		jz		EXIT2
		popad

		sub		esp, 0x28
		mov		eax, [0x00A0AF40]		// object window menu item checked state
		cmp		eax, 0

		jmp		[kObjectListPopulateListViewItemsRetnAddr]
	EXIT2:
		popad
		jmp		[kObjectListPopulateListViewItemsExitAddr]
	}
}

void __declspec(naked) CellViewPopulateObjectListHook(void)
{
	static const UInt32			kCellViewPopulateObjectListRetnAddr = 0x004087D3;
	static const UInt32			kCellViewPopulateObjectListExitAddr = 0x004088AF;
	__asm
	{
		mov		eax, [esp + 8]

		sub		esp, 0x28
		push	esi
		mov		esi, eax
		push	edi
		xor		edi, edi
		cmp		esi, edi

		jz		EXIT2

		pushad
		push	eax
		call	FormEnumerationWrapper::GetShouldEnumerateForm
		test	al, al
		jz		EXIT1
		popad

		jmp		[kCellViewPopulateObjectListRetnAddr]
	EXIT1:
		popad
	EXIT2:
		jmp		[kCellViewPopulateObjectListExitAddr]
	}
}

void __declspec(naked) DoorMarkerPropertiesHook(void)
{
	static const UInt32			kDoorMarkerPropertiesPropertiesAddr = 0x00429EAB;
	static const UInt32			kDoorMarkerPropertiesTeleportAddr = 0x00429EE8;
	__asm
	{
		mov		eax, [esi + 0x8]
		shr		eax, 0x0E
		test	al, 1
		jnz		DOORMARKER	

		jmp		[kDoorMarkerPropertiesPropertiesAddr]
	TELEPORT:
		popad
		jmp		[kDoorMarkerPropertiesTeleportAddr]
	DOORMARKER:
		pushad
		call	IsControlKeyDown
		test	eax, eax
		jz		TELEPORT
		popad

		jmp		[kDoorMarkerPropertiesPropertiesAddr]
	}
}

void __declspec(naked) AutoLoadActivePluginOnStartupHook(void)
{
	static const UInt32			kAutoLoadActivePluginOnStartupRetnAddr = 0x0041A284;
	__asm
	{
		mov		eax, 1
		jmp		[kAutoLoadActivePluginOnStartupRetnAddr]
	}
}

void __stdcall DestroyShadeMeRef(void)
{
	TESForm* Ref = GetFormByID("TheShadeMeRef");
	if (Ref)
		thisVirtualCall(kVTBL_TESObjectREFR, 0x34, Ref);
}
void __stdcall ClearRenderSelectionGroupMap(void)
{
	g_RenderSelectionGroupManager.Clear();
}

void __declspec(naked) DataHandlerClearDataHook(void)
{
	static const UInt32			kDataHandlerClearDataShadeMeRefDtorRetnAddr = 0x0047AE7B;
	__asm
	{
		lea     edi, [ebx+44h]
		mov     ecx, edi
		pushad
		call	DestroyShadeMeRef
		call	ClearRenderSelectionGroupMap
		popad

		jmp		[kDataHandlerClearDataShadeMeRefDtorRetnAddr]
	}
}

void __stdcall AppendShadeMeRefToComboBox(HWND hWnd)
{
	TESForm* Ref = GetFormByID("TheShadeMeRef");
	sprintf_s(g_Buffer, sizeof(g_Buffer), "'shadeMe' 'TheShadeMeRef'");
	TESDialog_AddComboBoxItem(hWnd, g_Buffer, (LPARAM)Ref, 1);
}

void __declspec(naked) CellObjectListShadeMeRefAppendHook(void)
{
	static const UInt32			kCellObjectListShadeMeRefAppendRetnAddr = 0x0044512D;
	__asm
	{
		pushad
		push	edx
		call	AppendShadeMeRefToComboBox
		popad

		call	TESDialog_AddComboBoxItem
		jmp		[kCellObjectListShadeMeRefAppendRetnAddr]
	}
}

void __declspec(naked) TopicInfoCopyEpilogHook(void)
{
	static const UInt32			kTopicInfoCopyEpilogHookRetnAddr = 0x004F1286;
	__asm
	{
		pushad
		mov		eax, [esi]
		mov		eax, [eax + 0x94]		// SetFromActiveFile
		push	1
		mov		ecx, esi
		call	eax
		popad

		mov     [esi + 0x30], bx
		mov     eax, [edi]
		jmp		[kTopicInfoCopyEpilogHookRetnAddr]
	}
}


void __stdcall InsertFormListPopupMenuItems(HMENU Menu, TESForm* SelectedForm)
{
	InsertMenu(Menu, -1, MF_BYPOSITION|MF_SEPARATOR, NULL, NULL);
	InsertMenu(Menu, -1, MF_BYPOSITION|MF_STRING, POPUP_SETFORMID, "Set FormID");
	InsertMenu(Menu, -1, MF_BYPOSITION|MF_STRING, POPUP_MARKUNMODIFIED, "Mark As Unmodified");
	InsertMenu(Menu, -1, MF_BYPOSITION|MF_STRING, POPUP_UNDELETE, "Undelete");
	InsertMenu(Menu, -1, MF_BYPOSITION|MF_STRING, POPUP_JUMPTOUSEINFOLIST, "Jump To Central Use Info List");
	if (GetDialogTemplate(SelectedForm->typeID) == 1 && SelectedForm->IsReference() == 0)
	{
		InsertMenu(Menu, -1, MF_BYPOSITION|MF_STRING, POPUP_ADDTOTAG, "Add to Active Tag");
	}
	
	InsertMenu(Menu, -1, MF_BYPOSITION|MF_SEPARATOR, NULL, NULL);
	if (SelectedForm->IsReference())
	{
		InsertMenu(Menu, -1, MF_BYPOSITION|MF_STRING, POPUP_EDITBASEFORM, "Edit Base Form");
		InsertMenu(Menu, -1, MF_BYPOSITION|MF_STRING, POPUP_TOGGLEVISIBILITY, "Toggle Visibility");
		InsertMenu(Menu, -1, MF_BYPOSITION|MF_STRING, POPUP_TOGGLECHILDRENVISIBILITY, "Toggle Children Visibility");
	}
	
}
void __stdcall HandleHookedPopup(HWND Parent, int MenuIdentifier, TESForm* SelectedObject)
{
	switch (MenuIdentifier)
	{
	case POPUP_SETFORMID:
	case POPUP_MARKUNMODIFIED:
	case POPUP_JUMPTOUSEINFOLIST:
	case POPUP_UNDELETE:
	case POPUP_EDITBASEFORM:
	case POPUP_TOGGLEVISIBILITY:
	case POPUP_TOGGLECHILDRENVISIBILITY:
	case POPUP_ADDTOTAG:
		EvaluatePopupMenuItems(Parent, MenuIdentifier, SelectedObject);
		break;
	default:
		SendMessage(Parent, WM_COMMAND, (WPARAM)MenuIdentifier, NULL);
		break;
	}
}
void __stdcall RemoveFormListPopupMenuItems(HMENU Menu)
{
	DeleteMenu(Menu, POPUP_SETFORMID, MF_BYCOMMAND);
	DeleteMenu(Menu, POPUP_MARKUNMODIFIED, MF_BYCOMMAND);
	DeleteMenu(Menu, POPUP_JUMPTOUSEINFOLIST, MF_BYCOMMAND);
	DeleteMenu(Menu, POPUP_UNDELETE, MF_BYCOMMAND);
	DeleteMenu(Menu, POPUP_EDITBASEFORM, MF_BYCOMMAND);
	DeleteMenu(Menu, POPUP_TOGGLEVISIBILITY, MF_BYCOMMAND);
	DeleteMenu(Menu, POPUP_TOGGLECHILDRENVISIBILITY, MF_BYCOMMAND);
	DeleteMenu(Menu, POPUP_ADDTOTAG, MF_BYCOMMAND);
	DeleteMenu(Menu, GetMenuItemCount(Menu) - 1, MF_BYPOSITION);
	DeleteMenu(Menu, GetMenuItemCount(Menu) - 1, MF_BYPOSITION);
}


void __declspec(naked) TESDialogPopupMenuHook(void)
{
	static const UInt32			kTESDialogPopupMenuHookRetnAddr = 0x004435C3;

	static HWND					ParentHWND = NULL;
	__asm
	{
		pushad
		call	TrackPopupMenuAddress
		popad

		mov		eax, [esp + 0x18]
		mov		ParentHWND, eax
		push	0
		push	eax
		mov		eax, [esp + 0x1C]
		mov		ecx, [eax + 4]
		mov		edx, [eax]
		push	0
		push	ecx
		push	edx

		test	ebx, ebx
		jz		SKIP

		pushad
		push	ebx
		push	esi
		call	InsertFormListPopupMenuItems
		popad

		push	0x102
		push	esi
		call	g_WindowHandleCallAddr

		pushad
		push	esi
		call	RemoveFormListPopupMenuItems
		popad

		push	ebx
		push	eax
		push	ParentHWND
		call	HandleHookedPopup
		jmp		[kTESDialogPopupMenuHookRetnAddr]
	SKIP:
		push	2
		push	esi
		call	g_WindowHandleCallAddr

		jmp		[kTESDialogPopupMenuHookRetnAddr]
	}
}

void __stdcall DoResponseWindowInitHook(HWND hWnd)
{
	g_ResponseWndOrgWindowProc = (WNDPROC)SetWindowLong(hWnd, GWL_WNDPROC, (LONG)ResponseWndSubClassProc);
	
	ShowWindow(GetDlgItem(hWnd, 2220), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, 2221), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, 2222), SW_HIDE);

	EnableWindow(GetDlgItem(hWnd, 2379), FALSE);
	EnableWindow(GetDlgItem(hWnd, 2380), FALSE);
	EnableWindow(GetDlgItem(hWnd, 1016), TRUE);

	CheckRadioButton(hWnd, 2379, 2380, 2379);
	SetWindowText(GetDlgItem(hWnd, 2379), "From MP3");

	SetWindowText(GetDlgItem(hWnd, 2223), "Copy External File");
	SetWindowPos(GetDlgItem(hWnd, 2223), HWND_TOP, 150, 550, 105, 20, SWP_NOZORDER|SWP_SHOWWINDOW);
}

void __declspec(naked) ResponseWindowInitHook(void)
{
	static UInt32 kResponseWindowInitHookRetnAddr = 0x004EBA92;
	__asm
	{
		push	ebp
		call	DoResponseWindowInitHook
		jmp		[kResponseWindowInitHookRetnAddr]
	}
}

void __stdcall DoNumericEditorIDHook(const char* EditorID)
{
	if (g_INIManager->FetchSetting("ShowNumericEditorIDWarning")->GetValueAsInteger() && 
		g_PluginPostLoad && 
		strlen(EditorID) > 0 && 
		isdigit((int)*EditorID))
	{
		sprintf_s(g_Buffer, sizeof(g_Buffer), "The editorID '%s' begins with an integer.\n\nWhile this is generally accepted by the engine, scripts referring this form might fail to run or compile as the script compiler can attempt to parse it as an integer.\n\nConsider starting the editorID with an alphabet.", EditorID);
		MessageBox(*g_HWND_CSParent, g_Buffer, "CSE", MB_OK|MB_ICONWARNING);
	}
}

void __declspec(naked) NumericEditorIDHook(void)
{
	static UInt32 kNumericEditorIDHookRetnAddr = 0x00497676;
	__asm
	{
		mov		eax, [esp + 0x4]
		pushad
		push	eax
		call	DoNumericEditorIDHook
		popad

		xor		eax, eax
		push	ebp
		mov		ebp, esp
		sub		esp, 0x10
		jmp		[kNumericEditorIDHookRetnAddr]
	}
}

void __declspec(naked) DataHandlerConstructSpecialFormsHook(void)
{
	static UInt32 kDataHandlerConstructSpecialFormsHookRetnAddr = 0x0048104E;
	static UInt32 kDataHandlerConstructSpecialFormsHookCallAddr = 0x00505070;
	__asm
	{
		pushad
		push	0
		call	HallOfFame::Initialize
		popad

		call	[kDataHandlerConstructSpecialFormsHookCallAddr]
		jmp		[kDataHandlerConstructSpecialFormsHookRetnAddr]
	}
}



void __declspec(naked) ResultScriptSaveFormHook(void)
{
	static UInt32 kResultScriptSaveFormHookRetnAddr = 0x004FD260;
	__asm
	{
		mov		eax, [ecx]
		mov		edx, [eax + 0x8]
		test	edx, edx
		jz		FAIL

		jmp		[kResultScriptSaveFormHookRetnAddr]
	FAIL:
		mov		eax, 0x004FD271
		jmp		eax
	}
}

void __stdcall DoFormIDListViewInitHook(HWND hWnd)
{
	if (hWnd != *g_HWND_QuestWindow)
	{
		SetWindowText(GetDlgItem(hWnd, 1), "Apply");
		SetWindowText(GetDlgItem(hWnd, 2), "Close");
	}
}

void __declspec(naked) FormIDListViewInitHook(void)
{
	static UInt32 kFormIDListViewInitHookRetnAddr = 0x00448A94;
	static UInt32 kByteAddr = 0x00A0BE45;
	__asm
	{
		pushad
		push	esi
		call	DoFormIDListViewInitHook
		popad

		push	5
		push	esi
		mov		[kByteAddr], 0

		jmp		[kFormIDListViewInitHookRetnAddr]
	}
}

UInt32 __stdcall DoFormIDListViewSaveChangesHookProlog(HWND Parent)
{
	return Parent != *g_HWND_QuestWindow;
}

void __stdcall DoFormIDListViewSaveChangesHookEpilog(HWND Parent)
{
	if (IsWindowEnabled(GetDlgItem(Parent, 1)))
	{
		TESForm* LocalCopy = TESDialog_GetDialogExtraLocalCopy(Parent);
		TESForm* WorkingCopy = TESDialog_GetDialogExtraParam(Parent);

		if (WorkingCopy)
		{
			thisVirtualCall(*((UInt32*)LocalCopy), 0x118, LocalCopy, Parent);						// GetDataFromDialog
			if (thisVirtualCall(*((UInt32*)WorkingCopy), 0xBC, WorkingCopy, LocalCopy))				// CompareTo
			{
				if (thisVirtualCall(*((UInt32*)WorkingCopy), 0x104, WorkingCopy, LocalCopy))		// UpdateUsageInfo
				{
					thisVirtualCall(*((UInt32*)WorkingCopy), 0x94, LocalCopy, 1);					// SetFromActiveFile
					thisVirtualCall(*((UInt32*)WorkingCopy), 0xB8, WorkingCopy, LocalCopy);			// CopyFrom
				}
			}
		}
	}
}

void __declspec(naked) FormIDListViewSaveChangesHook(void)
{
	static UInt32 kFormIDListViewSaveChangesHookQuestRetnAddr = 0x00449580;
	static UInt32 kFormIDListViewSaveChangesHookExitRetnAddr = 0x00448BF0;	
	__asm
	{
		push	esi
		call	TESDialog_GetDialogExtraParam

		pushad
		push	esi
		call	DoFormIDListViewSaveChangesHookProlog
		test	eax, eax
		jnz		NOTQUEST
		popad

		jmp		[kFormIDListViewSaveChangesHookQuestRetnAddr]
	NOTQUEST:
		popad

		pushad
		push	esi
		call	DoFormIDListViewSaveChangesHookEpilog
		popad

		jmp		[kFormIDListViewSaveChangesHookExitRetnAddr]
	}
}

int __stdcall DoFormIDListViewItemChangeHook(HWND Parent)
{
	return MessageBox(Parent, "Save Changes made to the active form?", "CSE", MB_YESNO);
}

void __declspec(naked) FormIDListViewItemChangeHook(void)
{
	static UInt32 kFormIDListViewItemChangeHookRetnAddr = 0x00448DF4;
	__asm
	{
		pushad
		push	esi
		call	DoFormIDListViewItemChangeHook
		cmp		eax, IDYES
		jnz		REVERT
		popad

		mov		eax, [edi]
		mov		edx, [eax + 0x104]

		jmp		[kFormIDListViewItemChangeHookRetnAddr]
	REVERT:
		popad

		mov		eax, 0x004494C9
		jmp		eax
	}
}

void __stdcall DoFormIDListViewSelectItemHook(HWND ListView, int ItemIndex)
{
	SetFocus(ListView);
	ListView_SetItemState(ListView, ItemIndex, LVIS_FOCUSED | LVIS_SELECTED, 0x000F); 
}

void __declspec(naked) FormIDListViewSelectItemHook(void)
{
	static UInt32 kFormIDListViewSelectItemHookRetnAddr = 0x00403B8A;
	__asm
	{
		pushad
		push	ebx
		push	esi
		call	DoFormIDListViewSelectItemHook
		popad

		jmp		[kFormIDListViewSelectItemHookRetnAddr]
	}
}

void __declspec(naked) FormIDListViewDuplicateSelectionHook(void)
{
	static UInt32 kFormIDListViewDuplicateSelectionHookRetnAddr = 0x004492B7;
	__asm
	{
		add		eax, 1
		push	eax
		push	0x810
		push	esi
		call	edi

		jmp		[kFormIDListViewDuplicateSelectionHookRetnAddr]
	}
}

void __stdcall DoTESRaceCopyHairEyeDataInitHook(Subwindow* RaceDialogSubWindow, UInt8 Template)
{
	if (Template == 2)
	{
		HWND Parent = RaceDialogSubWindow->hDialog;

		HWND CopyHairButton = CreateWindowEx(0, 
												"BUTTON", 
												"Copy Hair From Race", 
												WS_CHILD|WS_VISIBLE|WS_TABSTOP,
												RaceDialogSubWindow->position.x + 175 - 42, RaceDialogSubWindow->position.y + 285, 165, 25, 
												Parent, 
												(HMENU)RACE_COPYHAIR, 
												GetModuleHandle(NULL), 
												NULL);

		HWND CopyEyesButton = CreateWindowEx(0, 
												"BUTTON", 
												"Copy Eyes From Race", 
												WS_CHILD|WS_VISIBLE|WS_TABSTOP,
												RaceDialogSubWindow->position.x + 175 + 130, RaceDialogSubWindow->position.y + 285, 165, 25,
												Parent, 
												(HMENU)RACE_COPYEYES, 
												GetModuleHandle(NULL), 
												NULL);

		SendMessage(CopyHairButton, WM_SETFONT, (WPARAM)g_CSDefaultFont, TRUE);
		SendMessage(CopyEyesButton, WM_SETFONT, (WPARAM)g_CSDefaultFont, TRUE);	

		thisCall(kLinkedListNode_NewNode, &RaceDialogSubWindow->controls, CopyHairButton);
		thisCall(kLinkedListNode_NewNode, &RaceDialogSubWindow->controls, CopyEyesButton);
	}
}

void __declspec(naked) TESRaceCopyHairEyeDataInitHook(void)
{
	static UInt32 kTESRaceCopyHairEyeDataInitHookRetnAddr = 0x004E973A;
	static UInt32 kTESRaceCopyHairEyeDataInitHookCallAddr = 0x004E83B0;
	__asm
	{
		call	[kTESRaceCopyHairEyeDataInitHookCallAddr]

		pushad
		push	ebx
		push	edi
		call	DoTESRaceCopyHairEyeDataInitHook
		popad
		jmp		[kTESRaceCopyHairEyeDataInitHookRetnAddr]
	}
}

UInt32 __stdcall DoTESRaceCopyHairEyeDataMessageHandlerHook(HWND Dialog, INT Identifier, TESRace* WorkingRace)
{
	switch (Identifier)
	{
	case RACE_COPYEYES:
	case RACE_COPYHAIR:
		TESForm* Selection = (TESForm*)DialogBoxParam(g_DLLInstance, MAKEINTRESOURCE(DLG_TESCOMBOBOX), Dialog, (DLGPROC)TESComboBoxDlgProc, (LPARAM)kFormType_Race);
		if (Selection)
		{
			TESRace* SelectedRace = CS_CAST(Selection, TESForm, TESRace);

			if (WorkingRace)
			{
				int Count = 0;
				if (Identifier == RACE_COPYEYES)
				{
					GenericNode<TESEyes>* Source = (GenericNode<TESEyes>*)&SelectedRace->eyes;
					tList<TESEyes>* Destination = &WorkingRace->eyes;

					for (;Source && Source->data; Source = Source->next)
					{
						if (Destination->IndexOf(Source->data) == -1)
						{
							thisCall(kLinkedListNode_NewNode, Destination, Source->data);
							Count++;
						}
					}

					sprintf_s(g_Buffer, sizeof(g_Buffer), "Copied %d eye forms from race '%s'.", Count, SelectedRace->editorData.editorID.m_data);
					MessageBox(Dialog, g_Buffer, "CSE", MB_OK);
				}
				else if (Identifier == RACE_COPYHAIR)
				{
					GenericNode<TESHair>* Source = (GenericNode<TESHair>*)&SelectedRace->hairs;
					tList<TESHair>* Destination = &WorkingRace->hairs;

					for (;Source && Source->data; Source = Source->next)
					{
						if (Destination->IndexOf(Source->data) == -1)
						{
							thisCall(kLinkedListNode_NewNode, Destination, Source->data);
							Count++;
						}
					}

					sprintf_s(g_Buffer, sizeof(g_Buffer), "Copied %d hair forms from race '%s'.", Count, SelectedRace->editorData.editorID.m_data);
					MessageBox(Dialog, g_Buffer, "CSE", MB_OK);
				}
			}
		}
		return 1;
	}

	return 0;
}

void __declspec(naked) TESRaceCopyHairEyeDataMessageHandlerHook(void)
{
	static UInt32 kTESRaceCopyHairEyeDataMessageHandlerHookRetnAddr = 0x004E8FF2;
	static UInt32 kTESRaceCopyHairEyeDataMessageHandlerHookJumpAddr = 0x004E8E6D;
	__asm
	{
		movzx	eax, di
		add		eax, 0FFFFF78Ch
		cmp		eax, 0x24 
		ja		DEFAULT

		jmp		[kTESRaceCopyHairEyeDataMessageHandlerHookRetnAddr]
	DEFAULT:
		pushad
		push	ebp
		push	edi
		push	esi
		call	DoTESRaceCopyHairEyeDataMessageHandlerHook
		test	eax, eax
		jnz		HANDLED
		popad

		jmp		[kTESRaceCopyHairEyeDataMessageHandlerHookJumpAddr]
	HANDLED:
		popad

		xor		al, al
		mov		ecx, 0x004E9D5B
		jmp		ecx
	}
}

void __declspec(naked) TESObjectREFRDoCopyFromHook(void)
{
	static UInt32 kTESObjectREFRDoCopyFromHookRetnAddr = 0x00547668;
	__asm
	{
		pushad
		push	ebx
		mov		ecx, ebp
		call	kExtraDataList_CopyList
		popad

		pushad
		mov		ecx, edi
		call	[kTESObjectREFR_RemoveExtraTeleport]
		popad

		jmp		[kTESObjectREFRDoCopyFromHookRetnAddr]
	}
}

void __declspec(naked) LODLandTextureMipMapLevelBHook(void)
{
	static UInt32 kLODLandTextureMipMapLevelBHookRetnAddr = 0x005E030F;
	__asm
	{
		push    2
		push    0x14
		push    0
		mov		eax, 8
		push	eax
		push    ebx
		
		jmp		[kLODLandTextureMipMapLevelBHookRetnAddr]
	}
}

void __stdcall DoLODLandTextureResolutionHook(void)
{
	LPDIRECT3DDEVICE9 D3DDevice = (*g_CSRenderer)->device;

	D3DXCreateTexture(D3DDevice, 32, 32, 1, 0, D3DFMT_R8G8B8, D3DPOOL_SYSTEMMEM, g_LODD3DTexture32x);
	D3DXCreateTexture(D3DDevice, 128, 128, 1, 0, D3DFMT_R8G8B8, D3DPOOL_SYSTEMMEM, g_LODD3DTexture128x);
	D3DXCreateTexture(D3DDevice, 512, 512, 1, 0, D3DFMT_R8G8B8, D3DPOOL_SYSTEMMEM, g_LODD3DTexture512x);
	D3DXCreateTexture(D3DDevice, 2048, 2048, 1, 0, D3DFMT_R8G8B8, D3DPOOL_SYSTEMMEM, g_LODD3DTexture2048x);

	// increase size of the default textures used as buffers during LOD texture generation
	D3DXCreateTexture(D3DDevice, 64, 64, 1, 0, D3DFMT_R8G8B8, D3DPOOL_SYSTEMMEM, g_LODD3DTexture64x);
	D3DXCreateTexture(D3DDevice, 1024, 1024, 1, 0, D3DFMT_R8G8B8, D3DPOOL_SYSTEMMEM, g_LODD3DTexture1024x);
//	D3DXCreateTexture(D3DDevice, 256, 256, 1, 0, D3DFMT_R8G8B8, D3DPOOL_SYSTEMMEM, g_LODD3DTexture64x);
//	D3DXCreateTexture(D3DDevice, 4096, 4096, 1, 0, D3DFMT_R8G8B8, D3DPOOL_SYSTEMMEM, g_LODD3DTexture1024x);

	*g_LODBSTexture32x = (BSRenderedTexture*)thisCall(kBSTextureManager_CreateBSRenderedTexture, *g_TextureManager, *g_CSRenderer, 32, 21, 0, 0);
	*g_LODBSTexture128x = (BSRenderedTexture*)thisCall(kBSTextureManager_CreateBSRenderedTexture, *g_TextureManager, *g_CSRenderer, 128, 21, 0, 0);
	*g_LODBSTexture512x = (BSRenderedTexture*)thisCall(kBSTextureManager_CreateBSRenderedTexture, *g_TextureManager, *g_CSRenderer, 512, 21, 0, 0);
	*g_LODBSTexture2048x = (BSRenderedTexture*)thisCall(kBSTextureManager_CreateBSRenderedTexture, *g_TextureManager, *g_CSRenderer, 2048, 21, 0, 0);

	*g_LODBSTexture64x = (BSRenderedTexture*)thisCall(kBSTextureManager_CreateBSRenderedTexture, *g_TextureManager, *g_CSRenderer, 64, 21, 0, 0);
	*g_LODBSTexture1024x = (BSRenderedTexture*)thisCall(kBSTextureManager_CreateBSRenderedTexture, *g_TextureManager, *g_CSRenderer, 1024, 21, 0, 0);
//	*g_LODBSTexture64x = (BSRenderedTexture*)thisCall(kBSTextureManager_CreateBSRenderedTexture, *g_TextureManager, *g_CSRenderer, 256, 21, 0, 0);
//	*g_LODBSTexture1024x = (BSRenderedTexture*)thisCall(kBSTextureManager_CreateBSRenderedTexture, *g_TextureManager, *g_CSRenderer, 4096, 21, 0, 0);
}

void __declspec(naked) LODLandTextureResolutionHook(void)
{
	static UInt32 kLODLandTextureResolutionHookRetnAddr = 0x00410EB4;
	__asm
	{
		call	DoLODLandTextureResolutionHook
		jmp		[kLODLandTextureResolutionHookRetnAddr]
	}
}

bool __stdcall DoTESFileUpdateHeaderHook(TESFile* Plugin)
{
	PrintToBuffer("%s%s", Plugin->filepath, Plugin->name);
	HANDLE TempFile = CreateFile(g_Buffer, GENERIC_READ|GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (TempFile == INVALID_HANDLE_VALUE)
	{
		LogWinAPIErrorMessage(GetLastError());
		PrintToBuffer("Couldn't open TESFile '%s' for read/write access.\n\nError logged to the console.", Plugin->name);
		MessageBox(NULL, g_Buffer, "CSE", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	else
	{
		CloseHandle(TempFile);
		return true;
	}
}

void __declspec(naked) TESFileUpdateHeaderHook(void)
{
	static UInt32 kTESFileUpdateHeaderHookRetnAddr = 0x004894D6;
	static UInt32 kTESFileUpdateHeaderHookExitAddr = 0x0048957B;
	__asm
	{
		pushad
		xor		eax, eax
		push	ecx
		call	DoTESFileUpdateHeaderHook
		test	al, al
		jz		EXIT

		popad
		jmp		[kTESFileUpdateHeaderHookRetnAddr]
	EXIT:
		popad
		jmp		[kTESFileUpdateHeaderHookExitAddr]
	}
}

void __stdcall DoTESObjectREFRGet3DDataHook(TESObjectREFR* Object, NiNode* Node)
{
	ToggleFlag(&Node->m_flags, NiNode::kFlag_AppCulled, false);

	BSExtraData* xData = (BSExtraData*)thisCall(kBaseExtraList_GetExtraDataByType, &Object->baseExtraList, kExtraData_EnableStateParent);
	if (xData)
	{
		ExtraEnableStateParent* xParent = CS_CAST(xData, BSExtraData, ExtraEnableStateParent);
		if ((xParent->parent->flags & kTESObjectREFRSpecialFlags_Children3DInvisible))
			ToggleFlag(&Node->m_flags, NiNode::kFlag_AppCulled, true);
	}

	if ((Object->flags & kTESObjectREFRSpecialFlags_3DInvisible))
		ToggleFlag(&Node->m_flags, NiNode::kFlag_AppCulled, true);
}

void __declspec(naked) TESObjectREFRGet3DDataHook(void)
{
	__asm
	{
		push	esi
		push	ecx		// store
		push	0x56
		add		ecx, 0x4C
		xor		esi, esi
		call	[kBaseExtraList_GetExtraDataByType]
		test	eax, eax
		jz		NO3DDATA

		mov		eax, [eax + 0xC]
		pop		ecx		// restore
		push	ecx		// store again for epilog

		pushad
		push	eax
		push	ecx
		call	DoTESObjectREFRGet3DDataHook
		popad
		jmp		EXIT
	NO3DDATA:
		mov		eax, esi
	EXIT:
		pop		ecx
		pop		esi
		retn
	}
}

void __stdcall NiWindowRenderDrawHook(void)
{
	g_RenderTimeManager.Update();
	RENDERTEXT->Render();
}

void __declspec(naked) NiWindowRenderHook(void)
{
	static UInt32 kNiWindowRenderHookCallAddr = 0x0076A3B0;
	static UInt32 kNiWindowRenderHookRetnAddr = 0x00406447;
	__asm
	{
		call	[kNiWindowRenderHookCallAddr]

		pushad
		call	NiWindowRenderDrawHook
		popad

		jmp		[kNiWindowRenderHookRetnAddr]
	}
}

void __stdcall DoNiDX9RendererRecreateHook(void)
{
	RENDERTEXT->Recreate();
}

void __declspec(naked) NiDX9RendererRecreateHook(void)
{
	static UInt32 kNiDX9RendererRecreateHookRetnAddr = 0x006D7266;
	__asm
	{
		pushad
		call	DoNiDX9RendererRecreateHook
		popad

		sub     esp, 0x10
		push    ebx
		push    ebp
		push    esi

		jmp		[kNiDX9RendererRecreateHookRetnAddr]
	}
}


void __stdcall DoRenderWindowStatsHook(void)
{
	if (g_INIManager->GET_INI_INT("DisplaySelectionStats"))
	{
		if ((*g_TESRenderSelectionPrimary)->SelectionCount > 1)
		{
			PrintToBuffer("%d Objects Selected\nPosition Vector Sum: %.04f, %.04f, %.04f", 
						(*g_TESRenderSelectionPrimary)->SelectionCount,
						(*g_TESRenderSelectionPrimary)->x,
						(*g_TESRenderSelectionPrimary)->y,
						(*g_TESRenderSelectionPrimary)->z); 
			RENDERTEXT->QueueDrawTask(RenderWindowTextPainter::kRenderChannel_1, g_Buffer, 0);
		}
		else if ((*g_TESRenderSelectionPrimary)->SelectionCount)
		{
			TESObjectREFR* Selection = CS_CAST((*g_TESRenderSelectionPrimary)->RenderSelection->Data, TESForm, TESObjectREFR);
			char Buffer[0x50] = {0};
			sprintf_s(Buffer, 0x50, "");

			BSExtraData* xData = (BSExtraData*)thisCall(kBaseExtraList_GetExtraDataByType, &Selection->baseExtraList, kExtraData_EnableStateParent);
			if (xData)
			{
				ExtraEnableStateParent* xParent = CS_CAST(xData, BSExtraData, ExtraEnableStateParent);
				sprintf_s(Buffer, 0x50, "Parent: %s [%08X]  Opposite State: %d",
																((xParent->parent->editorData.editorID.m_dataLen)?(xParent->parent->editorData.editorID.m_data):("")),
																xParent->parent->refID, (UInt8)xParent->oppositeState);
			}
			PrintToBuffer("%s (%08X) BASE[%s (%08X)]\nP[%.04f, %.04f, %.04f]\nR[%.04f, %.04f, %.04f]\nS[%.04f]\nFlags: %s %s %s\n%s",
							((Selection->editorData.editorID.m_dataLen)?(Selection->editorData.editorID.m_data):("")), Selection->refID,
							((Selection->baseForm->editorData.editorID.m_dataLen)?(Selection->baseForm->editorData.editorID.m_data):("")), Selection->baseForm->refID,
							Selection->posX, Selection->posY, Selection->posZ, Selection->rotX, Selection->rotY, Selection->rotZ, Selection->scale,
							((Selection->flags & TESForm::kFormFlags_Essential)?("P"):("-")),
							((Selection->flags & TESForm::kFormFlags_InitiallyDisabled)?("D"):("-")),
							((Selection->flags & TESForm::kFormFlags_VisibleWhenDistant)?("V"):("-")),
							Buffer);

			RENDERTEXT->QueueDrawTask(RenderWindowTextPainter::kRenderChannel_1, g_Buffer, 0);
		}
		else
			RENDERTEXT->QueueDrawTask(RenderWindowTextPainter::kRenderChannel_1, NULL, 0);
	}
	else
		RENDERTEXT->QueueDrawTask(RenderWindowTextPainter::kRenderChannel_1, NULL, 0);
}

void __declspec(naked) RenderWindowStatsHook(void)
{
	static UInt32 kRenderWindowStatsHookCallAddr = 0x006F25E0;
	static UInt32 kRenderWindowStatsHookRetnAddr = 0x0042D3F9;
	__asm
	{
		call	[kRenderWindowStatsHookCallAddr]

		pushad
		call	DoRenderWindowStatsHook
		popad

		jmp		[kRenderWindowStatsHookRetnAddr]
	}
}

bool __stdcall DoUpdateViewportHook(void)
{
	if (RENDERTEXT->GetRenderChannelQueueSize(RenderWindowTextPainter::kRenderChannel_2) || g_INIManager->GET_INI_INT("UpdateViewPortAsync"))
		return true;
	else
		return false;		
}

void __declspec(naked) UpdateViewportHook(void)
{
	static UInt32 kUpdateViewportHookRetnAddr = 0x0042EF86;
	static UInt32 kUpdateViewportHookJumpAddr = 0x0042CE7D;
	__asm
	{
		mov		eax, [g_Flag_RenderWindowUpdateViewPort]
		mov		eax, [eax]
		cmp		al, 0
		jz		DONTUPDATE

		jmp		[kUpdateViewportHookJumpAddr]
	DONTUPDATE:
		pushad
		xor		eax, eax
		call	DoUpdateViewportHook
		test	al, al
		jz		EXIT
		
		popad
		jmp		[kUpdateViewportHookJumpAddr]
	EXIT:
		popad
		jmp		[kUpdateViewportHookRetnAddr]
	}
}



bool __stdcall DoRenderWindowSelectionHook(TESObjectREFR* Ref)
{
	bool Result = false;

	TESObjectCELL* CurrentCell = (*g_TES)->currentInteriorCell;
	if (CurrentCell == NULL)
		CurrentCell = (*g_TES)->currentExteriorCell;

	if (CurrentCell)
	{
		TESRenderSelection* Selection = g_RenderSelectionGroupManager.GetRefSelectionGroup(Ref, CurrentCell);
		if (Selection)
		{
			for (TESRenderSelection::SelectedObjectsEntry* Itr = Selection->RenderSelection; Itr && Itr->Data; Itr = Itr->Next)
				thisCall(kTESRenderSelection_AddFormToSelection, *g_TESRenderSelectionPrimary, Itr->Data, 1);

			RENDERTEXT->QueueDrawTask(RenderWindowTextPainter::kRenderChannel_2, "Selected object selection group", 3);
			Result = true;
		}
	}

	return Result;
}

void __declspec(naked) RenderWindowSelectionHook(void)
{
	static UInt32 kRenderWindowSelectionHookRetnAddr = 0x0042AE76;
	static UInt32 kRenderWindowSelectionHookJumpAddr = 0x0042AE84;
	__asm
	{
		call	[kTESRenderSelection_ClearSelection]
		xor		eax, eax

		pushad
		push	esi
		call	DoRenderWindowSelectionHook
		test	al, al
		jnz		GROUPED
		popad

		jmp		[kRenderWindowSelectionHookRetnAddr]
	GROUPED:
		popad
		jmp		[kRenderWindowSelectionHookJumpAddr]
	}
}

void __stdcall DoDataHandlerSavePluginEpilogHook(void)
{
	TESFile* ActiveFile = (*g_dataHandler)->unk8B8.activeFile;
	ToggleFlag(&ActiveFile->flags, ModEntry::Data::kFlag_IsMaster, 0);
}

void __declspec(naked) DataHandlerSavePluginEpilogHook(void)
{
	static UInt32 kDataHandlerSavePluginEpilogHookRetnAddr = 0x0047F13B;
	__asm
	{
		call	WriteToStatusBar

		pushad
		call	DoDataHandlerSavePluginEpilogHook
		popad

		jmp		[kDataHandlerSavePluginEpilogHookRetnAddr]
	}
}

void __stdcall DoTESFileUpdateHeaderFlagBitHook(TESFile* Plugin)
{
	TESFile* ActiveFile = (*g_dataHandler)->unk8B8.activeFile;
	if (ActiveFile)
		ToggleFlag(&ActiveFile->flags, ModEntry::Data::kFlag_IsMaster, 0);
}

void __declspec(naked) TESFileUpdateHeaderFlagBitHook(void)
{
	static UInt32 kTESFileUpdateHeaderFlagBitHookRetnAddr = 0x0048957B;
	__asm
	{
		pushad
		push	esi
		call	DoTESFileUpdateHeaderFlagBitHook
		popad
		jmp		[kTESFileUpdateHeaderFlagBitHookRetnAddr]
	}
}

bool __stdcall DoDataHandlerSaveFormToFileHook(TESForm* Form)
{
	ModEntry::Data* OverrideFile = (ModEntry::Data*)thisCall(kTESForm_GetOverrideFile, Form, -1);
	if (!OverrideFile || OverrideFile == (*g_dataHandler)->unk8B8.activeFile)
		return false;
	else
		return true;
}

void __declspec(naked) DataHandlerSaveFormToFileHook(void)
{
	static UInt32 kDataHandlerSaveFormToFileHookRetnAddr = 0x00479187;
	static UInt32 kDataHandlerSaveFormToFileHookJumpAddr = 0x0047919E;
	__asm
	{
		test	byte ptr [esi + 0x8], 1
		jz		FAIL
	EXIT:
		jmp		[kDataHandlerSaveFormToFileHookRetnAddr]	// TESForm::SaveForm
	FAIL:
		pushad
		xor		eax, eax
		push	esi
		call	DoDataHandlerSaveFormToFileHook
		test	al, al
		jnz		FIX
		popad

		jmp		[kDataHandlerSaveFormToFileHookJumpAddr]	// TESForm::SaveFormRecord
	FIX:
		popad
		jmp		EXIT
	}
}

bool __stdcall DoTESObjectCELLSaveReferencesPrologHook(TESObjectREFR* Reference, TESFile* SaveFile)
{
	TESFile* SourceFile = (ModEntry::Data*)thisCall(kTESForm_GetOverrideFile, Reference, 0);
	TESFile* ActiveFile = (ModEntry::Data*)thisCall(kTESForm_GetOverrideFile, Reference, -1);

	if (SourceFile == ActiveFile && ActiveFile == SaveFile)	
		return false;
	else
		return true;
}

void __declspec(naked) TESObjectCELLSaveReferencesPrologHook(void)
{
	static UInt32 kTESObjectCELLSaveReferencesPrologHookRetnAddr = 0x00538869;
	static UInt32 kTESObjectCELLSaveReferencesPrologHookJumpAddr = 0x0053886B;
	__asm
	{
		call	[kTESFile_GetIsESM]
		test	al, al
		jnz		PASS

		mov		eax, [esp + 0x44]		// TESFile* SaveFile
		pushad
		push	eax
		push	esi
		call	DoTESObjectCELLSaveReferencesPrologHook
		test	al, al
		jnz		FIX
		popad

		jmp		[kTESObjectCELLSaveReferencesPrologHookRetnAddr]
	FIX:
		popad
	PASS:
		jmp		[kTESObjectCELLSaveReferencesPrologHookJumpAddr]
	}
}

bool __stdcall DoTESObjectCELLSaveReferencesEpilogHook(TESObjectREFR* Reference, TESFile* SaveFile)
{
	if ((Reference->flags & TESForm::kFormFlags_Deleted))
	{
		TESFile* SourceFile = (ModEntry::Data*)thisCall(kTESForm_GetOverrideFile, Reference, 0);
		TESFile* ActiveFile = (ModEntry::Data*)thisCall(kTESForm_GetOverrideFile, Reference, -1);

		if ((SourceFile == ActiveFile && ActiveFile == SaveFile) ||
			(SourceFile == NULL && ActiveFile == NULL))
		{
			return false;
		}
		else if ((Reference->baseForm->flags & TESForm::kFormFlags_Deleted))
		{
			thisVirtualCall(*((UInt32*)Reference), 0x50, Reference, SaveFile);	// call SaveForm to dump an empty record
			return false;
		}
		else
			return true;
	}
	else
		return true;
}

void __declspec(naked) TESObjectCELLSaveReferencesEpilogHook(void)
{
	static UInt32 kTESObjectCELLSaveReferencesEpilogHookRetnAddr = 0x005389E1;
	__asm
	{
		pushad
		push	eax
		push	ecx
		call	DoTESObjectCELLSaveReferencesEpilogHook
		test	al, al
		jz		EXIT
		popad

		push	eax
		call	kTESForm_SaveFormRecord
		jmp		[kTESObjectCELLSaveReferencesEpilogHookRetnAddr]
EXIT:
		popad
		jmp		[kTESObjectCELLSaveReferencesEpilogHookRetnAddr]
	}
}

bool __stdcall DoTESDialogGetIsWindowDragDropRecipientHook(HWND Handle)
{
	return g_DragDropSupportDialogs.GetHandleExists(Handle);
}

void __declspec(naked) TESDialogGetIsWindowDragDropRecipientHook(void)
{
	static UInt32 kTESDialogGetIsWindowDragDropRecipientHookRetnAddr = 0x00443409;
	static UInt32 kTESDialogGetIsWindowDragDropRecipientHookJumpAddr = 0x0044341F;
	__asm
	{
		pushad
		call	SendMessageAddress
		popad

		call	[g_WindowHandleCallAddr]
		test	eax, eax
		jz		FAIL

		jmp		[kTESDialogGetIsWindowDragDropRecipientHookRetnAddr]
	FAIL:
		pushad
		push	edi
		call	DoTESDialogGetIsWindowDragDropRecipientHook
		test	al, al
		jnz		FIX
		popad

		jmp		[kTESDialogGetIsWindowDragDropRecipientHookJumpAddr]
	FIX:
		popad
		jmp		[kTESDialogGetIsWindowDragDropRecipientHookRetnAddr]
	}
}

void __stdcall DoTESDialogShowCommonDialogPrologHook(HWND Dialog, int Identifier, char* Buffer)
{
	GetDlgItemText(Dialog, Identifier, Buffer, 0x104);
}

void __declspec(naked) TESDialogShowDDSCommonDialogPrologHook(void)
{
	static UInt32 kTESDialogShowDDSCommonDialogPrologHookRetnAddr = 0x004A4150;
	static UInt32 kTESDialogShowDDSCommonDialogPrologHookCallAddr = 0x00446CA0;
	__asm
	{
		pushad
		push	ebp
		push	[esi + 0x10]
		push	edi
		call	DoTESDialogShowCommonDialogPrologHook
		popad

		call	[kTESDialogShowDDSCommonDialogPrologHookCallAddr]
		jmp		[kTESDialogShowDDSCommonDialogPrologHookRetnAddr]
	}
}

void __declspec(naked) TESDialogShowNIFCommonDialogPrologHook(void)
{
	static UInt32 kTESDialogShowNIFCommonDialogPrologHookRetnAddr = 0x0049BDB0;
	static UInt32 kTESDialogShowNIFCommonDialogPrologHookCallAddr = 0x00446C60;
	__asm
	{
		pushad
		push	edx
		push	[edi + 0x20]
		push	esi
		call	DoTESDialogShowCommonDialogPrologHook
		popad

		call	[kTESDialogShowNIFCommonDialogPrologHookCallAddr]
		jmp		[kTESDialogShowNIFCommonDialogPrologHookRetnAddr]
	}
}

void __stdcall DoMemCpy(void* Source, void* Dest, size_t Size)
{
	ZeroMemory(Dest, Size);
	memcpy(Dest, Source, Size);
}

static char* s_AssetSelectorPathBuffer = new char[0x104];		// flabbergastingly hacky, but that's the price of being a black sheep

void __declspec(naked) TESDialogShowDDSCommonDialogEpilogHook(void)
{
	static UInt32 kTESDialogShowDDSCommonDialogEpilogHookRetnAddr = 0x00446CE1;
	static UInt32 kTESDialogShowDDSCommonDialogEpilogHookCallAddr = 0x00446A30;
	__asm
	{
		mov		eax, [esp]	// parent HWND
		push	esi
		mov		esi, ebp 	// path buffer

		pushad  
		push	eax 
		call	InitAssetSelectorDlg  
		cmp		eax, e_Close 
		jz		CLOSE 
		cmp		eax, e_FileBrowser  
		jz		FILEB  
		cmp		eax, e_BSABrowser 
		jz		BSAB 
		cmp		eax, e_EditPath 
		jz		EDITP 
		cmp		eax, e_CopyPath 
		jz		COPYP 
		popad  

		// clear path
		xor		eax, eax
		jmp		EXIT
	CLOSE: 
		popad 
		mov		eax, 1
		jmp		EXIT
	FILEB: 
		push	0x104
		push	s_AssetSelectorPathBuffer
		push	esi
		call	DoMemCpy
		popad 

		pop		esi
		call	[kTESDialogShowDDSCommonDialogEpilogHookCallAddr]
		test	al, al
		jz		RESETPATH
		
		mov		eax, 1
		jmp		[kTESDialogShowDDSCommonDialogEpilogHookRetnAddr]
	RESETPATH:
		push	0x104
		push	ebp
		push	s_AssetSelectorPathBuffer
		call	DoMemCpy

		mov		eax, 1
		jmp		[kTESDialogShowDDSCommonDialogEpilogHookRetnAddr]
	BSAB:  
		popad  
		push	kTextureSelectorCommonDialogFilterType
		call	InitBSAViewer 
		jmp		FETCH
	EDITP: 
		popad 
		push	eax 
		push	esi 
		push	0
		call	InitPathEditor 
		jmp		FETCH   
	COPYP: 
		popad 
		push	eax 
		push	kTextureSelectorCommonDialogFilterType
		call	InitPathCopier 
		jmp		FETCH
	FETCH:
		cmp		eax, e_FetchPath
		mov		eax, 1
		jz		FIXPATH
		jmp		EXIT 	
	FIXPATH:
		push	0x104
		push	esi
		push	g_AssetSelectorReturnPath
		call	DoMemCpy
	EXIT:
		pop		esi
		jmp		[kTESDialogShowDDSCommonDialogEpilogHookRetnAddr]
	}
}

void __declspec(naked) TESDialogShowNIFCommonDialogEpilogHook(void)
{
	static UInt32 kTESDialogShowNIFCommonDialogEpilogHookRetnAddr = 0x00446C99;
	static UInt32 kTESDialogShowNIFCommonDialogEpilogHookCallAddr = 0x00446A30;
	__asm
	{
		mov		eax, [esp]
		push	esi
		mov		esi, [esp + 0x38]

		pushad  
		push	eax 
		call	InitAssetSelectorDlg  
		cmp		eax, e_Close 
		jz		CLOSE 
		cmp		eax, e_FileBrowser  
		jz		FILEB  
		cmp		eax, e_BSABrowser 
		jz		BSAB 
		cmp		eax, e_EditPath 
		jz		EDITP 
		cmp		eax, e_CopyPath 
		jz		COPYP 
		popad  

		// clear path
		xor		eax, eax
		jmp		EXIT
	CLOSE: 
		popad 
		mov		eax, 1
		jmp		EXIT
	FILEB: 
		push	0x104
		push	s_AssetSelectorPathBuffer
		push	esi
		call	DoMemCpy
		popad 

		pop		esi
		call	[kTESDialogShowNIFCommonDialogEpilogHookCallAddr] 
		test	al, al
		jz		RESETPATH

		mov		eax, 1
		jmp		[kTESDialogShowNIFCommonDialogEpilogHookRetnAddr]
	RESETPATH:
		push	0x104
		push	[esp + 0x54]
		push	s_AssetSelectorPathBuffer
		call	DoMemCpy

		mov		eax, 1
		jmp		[kTESDialogShowNIFCommonDialogEpilogHookRetnAddr]
	BSAB:  
		popad  
		push	kModelSelectorCommonDialogFilterType
		call	InitBSAViewer 
		jmp		FETCH
	EDITP: 
		popad 
		push	eax 
		push	esi 
		push	0
		call	InitPathEditor 
		jmp		FETCH   
	COPYP: 
		popad 
		push	eax 
		push	kModelSelectorCommonDialogFilterType
		call	InitPathCopier 
		jmp		FETCH
	FETCH:
		cmp		eax, e_FetchPath
		mov		eax, 1
		jz		FIXPATH
		jmp		EXIT  	
	FIXPATH:
		push	0x104
		push	esi
		push	g_AssetSelectorReturnPath
		call	DoMemCpy
	EXIT:
		pop		esi
		jmp		[kTESDialogShowNIFCommonDialogEpilogHookRetnAddr]
	}
}
