#include "[Common]\CLIWrapper.h"

#include "Main.h"
#include "Hooks\Hooks-CompilerErrorDetours.h"
#include "Hooks\Hooks-Misc.h"
#include "Hooks\Hooks-ScriptEditor.h"
#include "GECK Extender_Resource.h"

#include "[BGSEEBase]\WorkspaceManager.h"

using namespace componentDLLInterface;
using namespace gecke;
using namespace gecke::hooks;

extern componentDLLInterface::CSEInterfaceTable g_InteropInterface;

extern "C"
{
	QUERYINTERFACE_EXPORT
	{
		return &g_InteropInterface;
	}
}

void DeleteInterOpData(IDisposableData* Pointer)
{
	delete Pointer;
}

/**** BEGIN EDITORAPI SUBINTERFACE ****/
#pragma region EditorAPI
void ComponentDLLDebugPrint(UInt8 Source, const char* Message)
{
	const char* Prefix = "";

	enum MessageSource
	{
		e_UL = 2,
		e_SE,
		e_BSA,
		e_TAG
	};

	bool InvalidPrefix = false;
	switch (Source)
	{
	case e_UL:
		Prefix = "UL";
		break;
	case e_SE:
		Prefix = "SE";
		break;
	case e_BSA:
		Prefix = "BSA";
		break;
	case e_TAG:
		Prefix = "TAG";
		break;
	default:
		SME_ASSERT(InvalidPrefix);
		break;
	}

	BGSEECONSOLE->LogMsg(Prefix, "%s", Message);
}

const char* GetAppPath(void)
{
	return BGSEEMAIN->GetAPPPath();
}

void WriteToStatusBar(int PanelIndex, const char* Message)
{
	// NOT_IMPLEMENTED;
}

HWND GetCSMainWindowHandle(void)
{
	return *TESCSMain::WindowHandle;
}

HWND GetRenderWindowHandle(void)
{
	NOT_IMPLEMENTED;
}

FormData* LookupFormByEditorID(const char* EditorID)
{
	TESForm* Form = TESForm::LookupByEditorID(EditorID);
	FormData* Result = nullptr;

	if (Form)
	{
		Result = new FormData();
		Result->FillFormData(Form);
	}

	return Result;
}

ScriptData* LookupScriptableByEditorID(const char* EditorID)
{
	ScriptData* Result = nullptr;
	TESForm* Form = TESForm::LookupByEditorID(EditorID);

	if (Form)
	{
		if (Form->IsReference())
		{
			TESObjectREFR* Ref =  CS_CAST(Form, TESForm, TESObjectREFR);
			if (Ref)
				Form = Ref->baseForm;
		}

		if (Form->formType == TESForm::kFormType_Script)
		{
			Result = new ScriptData();
			Result->FillScriptData(CS_CAST(Form, TESForm, Script));
		}
		else
		{
			TESScriptableForm* ScriptableForm = CS_CAST(Form, TESForm, TESScriptableForm);
			if (ScriptableForm)
			{
				Script* FormScript = ScriptableForm->script;
				if (FormScript)
				{
					Result = new ScriptData();
					Result->FillScriptData(FormScript);
				}
			}
		}
	}

	return Result;
}

const char* GetFormTypeIDLongName(UInt8 TypeID)
{
	return TESForm::GetFormTypeIDLongName(TypeID);
}

void LoadFormForEditByEditorID(const char* EditorID)
{
	NOT_IMPLEMENTED;
}

void LoadFormForEditByFormID(UInt32 FormID)
{
	NOT_IMPLEMENTED;
}

FormData* ShowPickReferenceDialog(HWND Parent)
{
	NOT_IMPLEMENTED;
}

void ShowUseReportDialog(const char* EditorID)
{
	NOT_IMPLEMENTED;
}

void SaveActivePlugin(void)
{
	SendMessage(*TESCSMain::WindowHandle, WM_COMMAND, TESCSMain::kToolbar_Save, NULL);
}

void ReadFromINI(const char* Setting, const char* Section, const char* Default, char* OutBuffer, UInt32 Size)
{
	BGSEEMAIN->INIGetter()(Setting, Section, Default, OutBuffer, Size);
}

void WriteToINI(const char* Setting, const char* Section, const char* Value)
{
	BGSEEMAIN->INISetter()(Setting, Section, Value);
}

UInt32 GetFormListActiveItemForegroundColor(void)
{
	 return SME::StringHelpers::GetRGB(settings::dialogs::kActiveFormForeColor.GetData().s);
}

UInt32 GetFormListActiveItemBackgroundColor(void)
{
	return SME::StringHelpers::GetRGB(settings::dialogs::kActiveFormBackColor.GetData().s);
}

bool GetShouldColorizeActiveForms(void)
{
	return settings::dialogs::kColorizeActiveForms.GetData().i;
}

bool GetShouldSortActiveFormsFirst(void)
{
	return settings::dialogs::kSortFormListsByActiveForm.GetData().i;
}

componentDLLInterface::CSEInterfaceTable::IEditorAPI::ConsoleContextObjectPtr RegisterConsoleContext(const char* Name)
{
	return BGSEECONSOLE->RegisterMessageLogContext(Name);
}

void DeregisterConsoleContext(void* ContextObject)
{
	BGSEECONSOLE->UnregisterMessageLogContext(ContextObject);
}

void PrintToConsoleContext(void* ContextObject, const char* Message, bool PrintTimestamp)
{
	BGSEECONSOLE->PrintToMessageLogContext(ContextObject, PrintTimestamp == false, "%s", Message);
}
#pragma endregion
/**** END EDITORAPI SUBINTERFACE ****/

/**** BEGIN SCRIPTEDITOR SUBINTERFACE ****/
#pragma region ScriptEditor
ScriptData* CreateNewScript(void)
{
	Script* NewInstance = CS_CAST(TESForm::CreateInstance(TESForm::kFormType_Script), TESForm, Script);
	ScriptData* Data = new ScriptData(NewInstance);
	NewInstance->SetFromActiveFile(true);
	_DATAHANDLER->scriptList.AddAt(NewInstance, eListEnd);
	_DATAHANDLER->SortScripts();

	return Data;
}

bool CompileScript(ScriptCompileData* Data)
{
	Script* ScriptForm = CS_CAST(Data->Script.ParentForm, TESForm, Script);
	char Buffer[0x200] = {0};

	if ((ScriptForm->formFlags & TESForm::kFormFlags_Deleted))
	{
		BGSEEUI->MsgBoxI(nullptr,
						MB_TASKMODAL|MB_TOPMOST|MB_SETFOREGROUND|MB_OK,
						"Script %s {%08X} has been deleted, ergo it cannot be compiled.", ScriptForm->editorID.c_str(), ScriptForm->formID);

		Data->CompilationSuccessful = false;
	}
	else
	{
		BSString* OldText = BSString::CreateInstance(ScriptForm->text);

		ScriptForm->info.type = Data->Script.Type;
		ScriptForm->UpdateUsageInfo();
		ScriptForm->SetText(Data->Script.Text);
		ScriptForm->SetFromActiveFile(true);

		SME_ASSERT(TESScriptCompiler::PrintErrorsToConsole == true);
		if (!Data->PrintErrorsToConsole)
			TESScriptCompiler::PrintErrorsToConsole = false;

		ScriptForm->info.compileResult = Data->CompilationSuccessful = ScriptForm->Compile();
		TESScriptCompiler::PrintErrorsToConsole = true;

		if (ScriptForm->info.compileResult)
		{
			_DATAHANDLER->SortScripts();
			Data->Script.FillScriptData(ScriptForm);
		}
		else
			ScriptForm->SetText(OldText->c_str());

		Data->CompilerMessages.Count = TESScriptCompiler::LastCompilationMessages.size();
		if (TESScriptCompiler::LastCompilationMessages.size())
		{
			Data->CompilerMessages.MessageListHead = new ScriptCompilerMessages::MessageData[Data->CompilerMessages.Count];

			for (int i = 0; i < Data->CompilerMessages.Count; i++)
			{
				auto CompilerMessage = &TESScriptCompiler::LastCompilationMessages[i];
				auto& NewMessage = Data->CompilerMessages.MessageListHead[i];
				NewMessage.Line = CompilerMessage->Line;
				NewMessage.Message = CompilerMessage->Message.c_str();
				NewMessage.MessageCode = CompilerMessage->MessageCode;

				if (CompilerMessage->IsWarning())
					NewMessage.Type = ScriptCompilerMessages::MessageData::kType_Warning;
				else if (CompilerMessage->IsError())
					NewMessage.Type = ScriptCompilerMessages::MessageData::kType_Error;
			}
		}
		else
			Data->CompilerMessages.MessageListHead = nullptr;

		OldText->DeleteInstance();
	}

	return Data->CompilationSuccessful;
}

void RecompileScripts(void)
{
	TESScriptCompiler::PreventErrorDetours = true;

	UInt32 ScriptCount = 0, Current = 0;

	for (tList<Script>::Iterator Itr = _DATAHANDLER->scriptList.Begin(); !Itr.End() && Itr.Get(); ++Itr)
	{
		Script* ScriptForm = Itr.Get();
		if ((ScriptForm->formFlags & TESForm::kFormFlags_Deleted) == 0 &&
			(ScriptForm->formFlags & TESForm::kFormFlags_FromActiveFile))
		{
			ScriptCount++;
		}
	}

	HWND NotificationDialog = CreateDialogParam(BGSEEMAIN->GetExtenderHandle(), MAKEINTRESOURCE(IDD_IDLE), BGSEEUI->GetMainWindow(), nullptr, NULL);
	Static_SetText(GetDlgItem(NotificationDialog, -1), "Please Wait");
	char Buffer[0x200] = {0};

	BGSEECONSOLE_MESSAGE("Recompiling active scripts...");
	BGSEECONSOLE->Indent();

	static const UInt32 kPreprocessorBufferSize = 2 * 1024 * 1024;
	char* PreprocessedTextBuffer = new char[kPreprocessorBufferSize];
	for (tList<Script>::Iterator Itr = _DATAHANDLER->scriptList.Begin(); !Itr.End() && Itr.Get(); ++Itr)
	{
		Script* ScriptForm = Itr.Get();
		if ((ScriptForm->formFlags & TESForm::kFormFlags_Deleted) == 0 &&
			(ScriptForm->formFlags & TESForm::kFormFlags_FromActiveFile))
		{
			BGSEECONSOLE_MESSAGE("Script '%s' {%08X}:", ScriptForm->editorID.c_str(), ScriptForm->formID);
			BGSEECONSOLE->Indent();

			Current++;
			FORMAT_STR(Buffer, "Please Wait\nCompiling Script %d/%d", Current, ScriptCount);
			Static_SetText(GetDlgItem(NotificationDialog, -1), Buffer);

			ZeroMemory(PreprocessedTextBuffer, kPreprocessorBufferSize);
			bool PreprocessResult = cliWrapper::interfaces::SE->PreprocessScript(ScriptForm->text, PreprocessedTextBuffer, kPreprocessorBufferSize);
			if (PreprocessResult)
			{
				BSString* OldText = BSString::CreateInstance(ScriptForm->text);

				ScriptForm->SetText(PreprocessedTextBuffer);
				ScriptForm->Compile();
				ScriptForm->SetText(OldText->c_str());

				OldText->DeleteInstance();
			}
			else
			{
				BGSEECONSOLE_MESSAGE("Preprocessing failed!");
			}

			BGSEECONSOLE->Outdent();
		}
	}

	delete [] PreprocessedTextBuffer;
	BGSEECONSOLE->Outdent();

	DestroyWindow(NotificationDialog);
	BGSEECONSOLE_MESSAGE("Recompile active scripts operation completed!");

	TESScriptCompiler::PreventErrorDetours = false;
}

void ToggleScriptCompilation(bool State)
{
	TESScriptCompiler::ToggleScriptCompilation(State);
}

bool DeleteScript(const char* EditorID)
{
	TESForm* Form = TESForm::LookupByEditorID(EditorID);
	if (Form)
	{
		Script* ScriptForm = CS_CAST(Form, TESForm, Script);
		if (ScriptForm)
		{
			ScriptForm->SetDeleted(true);
			ScriptForm->UpdateUsageInfo();

			return ScriptForm->IsDeleted();
		}
	}

	return false;
}

enum
{
	kDirection_Forward = 0,
	kDirection_Backward,
};

Script* GetScriptNeighbour(Script* Current, UInt8 Direction, bool OnlyFromActivePlugin)
{
	SME_ASSERT(_DATAHANDLER->scriptList.Count());

	int Index = _DATAHANDLER->scriptList.GetIndexOf(Current);
	Script* Result = nullptr;

	switch (Direction)
	{
	case kDirection_Forward:
		if (Index + 1 < _DATAHANDLER->scriptList.Count())
			Result = _DATAHANDLER->scriptList.GetNthItem(Index + 1);
		else
			Result = _DATAHANDLER->scriptList.GetNthItem(0);

		break;
	case kDirection_Backward:
		if (Index - 1 > -1)
			Result = _DATAHANDLER->scriptList.GetNthItem(Index - 1);
		else
			Result = _DATAHANDLER->scriptList.GetLastItem();

		break;
	}

	if (Result->GetEditorID() == nullptr)
		Result = GetScriptNeighbour(Result, Direction, OnlyFromActivePlugin);
	else if (OnlyFromActivePlugin && _DATAHANDLER->activeFile != nullptr && !Result->IsActive())
		Result = GetScriptNeighbour(Result, Direction, OnlyFromActivePlugin);

	return Result;
}

ScriptData* GetPreviousScriptInList(void* CurrentScript, bool OnlyFromActivePlugin)
{
	Script* ScriptForm = CS_CAST(CurrentScript, TESForm, Script);
	ScriptData* Result = nullptr;
	Script* Switch = nullptr;

	if (_DATAHANDLER->scriptList.Count())
	{
		if (ScriptForm == nullptr)
		{
			Switch = _DATAHANDLER->scriptList.GetLastItem();
			if (Switch->GetEditorID() == nullptr)
				Switch = GetScriptNeighbour(ScriptForm, kDirection_Backward, OnlyFromActivePlugin);
		}
		else
			Switch = GetScriptNeighbour(ScriptForm, kDirection_Backward, OnlyFromActivePlugin);
	}

	if (Switch && Switch->GetEditorID())
	{
		Result = new ScriptData();
		Result->FillScriptData(Switch);
	}

	return Result;
}

ScriptData* GetNextScriptInList(void* CurrentScript, bool OnlyFromActivePlugin)
{
	Script* ScriptForm = CS_CAST(CurrentScript, TESForm, Script);
	ScriptData* Result = nullptr;
	Script* Switch = nullptr;

	if (_DATAHANDLER->scriptList.Count())
	{
		if (ScriptForm == nullptr)
		{
			Switch = _DATAHANDLER->scriptList.GetNthItem(0);
			if (Switch->GetEditorID() == nullptr)
				Switch = GetScriptNeighbour(ScriptForm, kDirection_Forward, OnlyFromActivePlugin);
		}
		else
			Switch = GetScriptNeighbour(ScriptForm, kDirection_Forward, OnlyFromActivePlugin);
	}

	if (Switch && Switch->GetEditorID())
	{
		Result = new ScriptData();
		Result->FillScriptData(Switch);
	}

	return Result;
}

void RemoveScriptBytecode(void* CurrentScript)
{
	Script* ScriptForm = CS_CAST(CurrentScript, TESForm, Script);
	ScriptForm->RemoveCompiledData();
}

void DestroyScriptInstance(void* CurrentScript)
{
	Script* ScriptForm = CS_CAST(CurrentScript, TESForm, Script);
	thisCall<void>(0x004D1AE0, &_DATAHANDLER->scriptList, ScriptForm);	// remove
	ScriptForm->DeleteInstance();
}

bool IsUnsavedNewScript(void* CurrentScript)
{
	auto ScriptForm = CS_CAST(CurrentScript, TESForm, Script);

	return ScriptForm->GetEditorID() == nullptr && ScriptForm->data == nullptr;
}

void SaveEditorBoundsToINI(UInt32 Top, UInt32 Left, UInt32 Width, UInt32 Height)
{
	char Buffer[0x200] = {0};

	FORMAT_STR(Buffer, "%d", Left);
	WritePrivateProfileString("General", "Script Edit X", Buffer, TESCSMain::GetINIFilePath().c_str());
	FORMAT_STR(Buffer, "%d", Top);
	WritePrivateProfileString("General", "Script Edit Y", Buffer, TESCSMain::GetINIFilePath().c_str());
	FORMAT_STR(Buffer, "%d", Width);
	WritePrivateProfileString("General", "Script Edit W", Buffer, TESCSMain::GetINIFilePath().c_str());
	FORMAT_STR(Buffer, "%d", Height);
	WritePrivateProfileString("General", "Script Edit H", Buffer, TESCSMain::GetINIFilePath().c_str());
}

ScriptListData* GetScriptList(void)
{
	ScriptListData* Result = new ScriptListData();
	if (_DATAHANDLER->scriptList.Count())
	{
		Result->ScriptCount = _DATAHANDLER->scriptList.Count();
		Result->ScriptListHead = new ScriptData[Result->ScriptCount];
		int i = 0;
		for (tList<Script>::Iterator Itr = _DATAHANDLER->scriptList.Begin(); !Itr.End() && Itr.Get(); ++Itr)
		{
			Script* ScriptForm = Itr.Get();
			if (ScriptForm->GetEditorID() == nullptr)
				continue;

			Result->ScriptListHead[i].FillScriptData(ScriptForm);
			i++;
		}
	}

	return Result;
}

ScriptVarListData* GetScriptVarList(const char* EditorID)
{
	TESForm* Form = TESForm::LookupByEditorID(EditorID);
	ScriptVarListData* Result = new ScriptVarListData();

	if (Form)
	{
		Script* ScriptForm = CS_CAST(Form, TESForm, Script);
		if (ScriptForm && ScriptForm->varList.Count())
		{
			Result->ScriptVarListCount = ScriptForm->varList.Count();
			Result->ScriptVarListHead = new ScriptVarListData::ScriptVarInfo[Result->ScriptVarListCount];

			int i = 0;
			for (Script::VariableListT::Iterator Itr = ScriptForm->varList.Begin(); !Itr.End() && Itr.Get(); ++Itr)
			{
				Script::VariableInfo* Variable = Itr.Get();

				Result->ScriptVarListHead[i].Name = Variable->name.c_str();
				Result->ScriptVarListHead[i].Type = Variable->type;
				Result->ScriptVarListHead[i].Index = Variable->index;

				if (Result->ScriptVarListHead[i].Type == Script::kVariableTypes_Float)
				{
					for (Script::RefVariableListT::Iterator ItrEx = ScriptForm->refList.Begin(); !ItrEx.End() && ItrEx.Get(); ++ItrEx)
					{
						if (ItrEx.Get()->variableIndex == Variable->index)
						{
							Result->ScriptVarListHead[i].Type = 2;		// ref var
							break;
						}
					}
				}

				i++;
			}
		}
	}

	return Result;
}

bool UpdateScriptVarIndices(const char* EditorID, ScriptVarListData* Data)
{
	TESForm* Form = TESForm::LookupByEditorID(EditorID);
	bool Result = false;

	if (Form)
	{
		Script* ScriptForm = CS_CAST(Form, TESForm, Script);

		if (ScriptForm)
		{
			for (int i = 0; i < Data->ScriptVarListCount; i++)
			{
				ScriptVarListData::ScriptVarInfo* VarInfo = &Data->ScriptVarListHead[i];
				Script::VariableInfo* ScriptVar = ScriptForm->LookupVariableInfoByName(VarInfo->Name);

				if (ScriptVar)
				{
					if (VarInfo->Type == 2)
					{
						Script::RefVariable* RefVar = ScriptForm->LookupRefVariableByIndex(ScriptVar->index);
						if (RefVar)
							RefVar->variableIndex = VarInfo->Index;
					}

					ScriptVar->index = VarInfo->Index;
				}
			}

			Result = true;
			ScriptForm->SetFromActiveFile(true);
		}
	}

	return Result;
}

void CompileCrossReferencedForms(TESForm* Form)
{
	NOT_IMPLEMENTED;

	//BGSEECONSOLE_MESSAGE("Parsing object use list of %08X...", Form->formID);
	//BGSEECONSOLE->Indent();

	//std::list<Script*> ScriptDepends;		// updating usage info inside an use list loop invalidates the list.
	//std::list<TESTopicInfo*> InfoDepends;	// so store the objects ptrs and parse them later
	//std::list<TESQuest*> QuestDepends;

	//for (FormCrossReferenceListT::Iterator Itr = Form->GetCrossReferenceList()->Begin(); !Itr.End() && Itr.Get(); ++Itr)
	//{
	//	TESForm* Depends = Itr.Get()->GetForm();
	//	switch (Depends->formType)
	//	{
	//	case TESForm::kFormType_TopicInfo:
	//	{
	//		InfoDepends.push_back(CS_CAST(Depends, TESForm, TESTopicInfo));
	//		break;
	//	}
	//	case TESForm::kFormType_Quest:
	//	{
	//		QuestDepends.push_back(CS_CAST(Depends, TESForm, TESQuest));
	//		break;
	//	}
	//	case TESForm::kFormType_Script:
	//	{
	//		ScriptDepends.push_back(CS_CAST(Depends, TESForm, Script));
	//		break;
	//	}
	//	default:	// ### any other type that needs handling ?
	//		break;
	//	}
	//}

	//// scripts
	//for (std::list<Script*>::const_iterator Itr = ScriptDepends.begin(); Itr != ScriptDepends.end(); Itr++)
	//{
	//	BGSEECONSOLE_MESSAGE("Script %s {%08X}:", (*Itr)->editorID.c_str(), (*Itr)->formID);
	//	BGSEECONSOLE->Indent();

	//	if ((*Itr)->info.dataLength > 0)
	//	{
	//		if (!(*Itr)->Compile())
	//		{
	//			BGSEECONSOLE_MESSAGE("Script failed to compile due to errors!");
	//		}
	//	}

	//	BGSEECONSOLE->Outdent();
	//}

	//// quests
	//for (std::list<TESQuest*>::const_iterator Itr = QuestDepends.begin(); Itr != QuestDepends.end(); Itr++)
	//{
	//	BGSEECONSOLE_MESSAGE("Quest %s {%08X}:", (*Itr)->editorID.c_str(), (*Itr)->formID);
	//	BGSEECONSOLE->Indent();

	//	for (TESQuest::StageListT::Iterator i = (*Itr)->stageList.Begin(); !i.End(); ++i)
	//	{
	//		TESQuest::StageData* Stage = i.Get();
	//		if (!Stage)
	//			break;

	//		int Count = 0;
	//		for (TESQuest::StageData::StageItemListT::Iterator j = Stage->stageItemList.Begin(); !j.End(); ++j, ++Count)
	//		{
	//			TESQuest::StageData::QuestStageItem* StageItem = j.Get();
	//			if (!StageItem)
	//				break;

	//			if (StageItem->resultScript.info.dataLength > 0)
	//			{
	//				if (!StageItem->resultScript.Compile(true))
	//				{
	//					BGSEECONSOLE_MESSAGE("Result script in stage item %d-%d failed to compile due to errors!", Stage->index, Count);
	//				}
	//			}

	//			BGSEECONSOLE_MESSAGE("Found %d conditions in stage item %d-%d that referenced source script",
	//				TESConditionItem::GetScriptableFormConditionCount(&StageItem->conditions, Form), Stage->index, Count);
	//		}
	//	}

	//	for (TESQuest::TargetListT::Iterator j = (*Itr)->targetList.Begin(); !j.End(); ++j)
	//	{
	//		TESQuest::TargetData* Target = j.Get();
	//		if (!Target)
	//			break;

	//		BGSEECONSOLE_MESSAGE("Found %d conditions in target entry {%08X} that referenced source script",
	//			TESConditionItem::GetScriptableFormConditionCount(&Target->conditionList, Form), Target->target->formID);
	//	}

	//	(*Itr)->UpdateUsageInfo();
	//	BGSEECONSOLE->Outdent();
	//}

	//// topic infos
	//for (std::list<TESTopicInfo*>::const_iterator Itr = InfoDepends.begin(); Itr != InfoDepends.end(); Itr++)
	//{
	//	BGSEECONSOLE_MESSAGE("Topic info %08X:", (*Itr)->formID);
	//	BGSEECONSOLE->Indent();

	//	if ((*Itr)->resultScript.info.dataLength > 0)
	//	{
	//		if (!(*Itr)->resultScript.Compile(true))
	//		{
	//			BGSEECONSOLE_MESSAGE("Result script failed to compile due to errors!");
	//		}
	//	}

	//	BGSEECONSOLE_MESSAGE("Found %d conditions that referenced source script",
	//		TESConditionItem::GetScriptableFormConditionCount(&(*Itr)->conditions, Form));

	//	(*Itr)->UpdateUsageInfo();
	//	BGSEECONSOLE->Outdent();
	//}

	//BGSEECONSOLE->Outdent();
}

void CompileDependencies(const char* EditorID)
{
	NOT_IMPLEMENTED;

	//TESForm* Form = TESForm::LookupByEditorID(EditorID);
	//if (Form == nullptr)
	//	return;

	//Script* ScriptForm = CS_CAST(Form, TESForm, Script);
	//if (ScriptForm == nullptr)
	//	return;

	//BGSEECONSOLE_MESSAGE("Recompiling dependencies of script %s {%08X}...", ScriptForm->editorID.c_str(), ScriptForm->formID);
	//BGSEECONSOLE->Indent();

	//BGSEECONSOLE_MESSAGE("Resolving script parent...");
	//BGSEECONSOLE->Indent();
	//switch (ScriptForm->info.type)
	//{
	//case Script::kScriptType_Object:
	//{
	//	BGSEECONSOLE_MESSAGE("Script type = Object");
	//	for (FormCrossReferenceListT::Iterator Itr = Form->GetCrossReferenceList()->Begin(); !Itr.End() && Itr.Get(); ++Itr)
	//	{
	//		TESForm* Parent = Itr.Get()->GetForm();
	//		TESScriptableForm* ValidParent = CS_CAST(Parent, TESForm, TESScriptableForm);

	//		if (ValidParent)
	//		{
	//			BGSEECONSOLE_MESSAGE("Scriptable Form %s ; Type = %d:", Parent->editorID.c_str(), Parent->formType);
	//			BGSEECONSOLE_MESSAGE("Parsing cell use list...");
	//			BGSEECONSOLE->Indent();

	//			TESCellUseList* UseList = CS_CAST(Form, TESForm, TESCellUseList);
	//			for (TESCellUseList::CellUseInfoListT::Iterator Itr = UseList->cellUses.Begin(); !Itr.End(); ++Itr)
	//			{
	//				TESCellUseList::CellUseInfo* Data = Itr.Get();
	//				if (!Data)
	//					break;

	//				for (TESObjectCELL::ObjectREFRList::Iterator Itr = Data->cell->objectList.Begin(); !Itr.End(); ++Itr)
	//				{
	//					TESObjectREFR* ThisReference = Itr.Get();
	//					if (!ThisReference)
	//						break;

	//					if (ThisReference->baseForm == Parent)
	//						CompileCrossReferencedForms((TESForm*)ThisReference);
	//				}
	//			}

	//			BGSEECONSOLE->Outdent();
	//		}
	//	}
	//	break;
	//}
	//case Script::kScriptType_Quest:
	//{
	//	BGSEECONSOLE_MESSAGE("Script type = Quest");
	//	for (FormCrossReferenceListT::Iterator Itr = Form->GetCrossReferenceList()->Begin(); !Itr.End() && Itr.Get(); ++Itr)
	//	{
	//		TESForm* Parent = Itr.Get()->GetForm();
	//		if (Parent->formType == TESForm::kFormType_Quest)
	//		{
	//			BGSEECONSOLE_MESSAGE("Quest %s:", Parent->editorID.c_str());
	//			CompileCrossReferencedForms(Parent);
	//		}
	//	}
	//	break;
	//}
	//}
	//BGSEECONSOLE->Outdent();

	//BGSEECONSOLE_MESSAGE("Parsing direct dependencies...");
	//CompileCrossReferencedForms(Form);

	//BGSEECONSOLE->Outdent();
	//BGSEECONSOLE_MESSAGE("Recompile dependencies operation completed!");
}

IntelliSenseUpdateData* GetIntelliSenseUpdateData(void)
{
	IntelliSenseUpdateData* Data = new IntelliSenseUpdateData();

	UInt32 QuestCount = 0,
			ScriptCount = _DATAHANDLER->scriptList.Count(),
			GlobalCount = 0;

	ScriptData TestData;
	std::vector<TESForm*> MiscForms;


	Data->ScriptListHead = new ScriptData[ScriptCount];
	Data->ScriptCount = ScriptCount;

	QuestCount = 0, ScriptCount = 0, GlobalCount = 0;
	for (tList<Script>::Iterator Itr = _DATAHANDLER->scriptList.Begin(); !Itr.End() && Itr.Get(); ++Itr, ++ScriptCount)
	{
		TestData.FillScriptData(Itr.Get());
		Data->ScriptListHead[ScriptCount].FillScriptData(Itr.Get());
	}

	return Data;
}

void BindScript(const char* EditorID, HWND Parent)
{
	NOT_IMPLEMENTED;

	/*TESForm* Form = TESForm::LookupByEditorID(EditorID);
	if (Form == nullptr)
		return;

	Script* ScriptForm = CS_CAST(Form, TESForm, Script);
	if (ScriptForm == nullptr)
		return;

	if (ScriptForm->IsUserDefinedFunctionScript())
	{
		BGSEEUI->MsgBoxI(nullptr,
						 MB_TASKMODAL|MB_TOPMOST|MB_SETFOREGROUND|MB_OK,
						 "Script %s {%08X} is a user-defined function script and therefore cannot be bound to a form.",
						 ScriptForm->editorID.c_str(), ScriptForm->formID);
		return;
	}

	Form = reinterpret_cast<TESForm*>(BGSEEUI->ModalDialog(BGSEEMAIN->GetExtenderHandle(),
														   MAKEINTRESOURCE(IDD_BINDSCRIPT),
														   BGSEEUI->GetMainWindow(),
														   reinterpret_cast<DLGPROC>(uiManager::BindScriptDlgProc)));

	if (Form)
	{
		TESQuest* Quest = CS_CAST(Form, TESForm, TESQuest);
		TESBoundObject* BoundObj = CS_CAST(Form, TESForm, TESBoundObject);
		TESScriptableForm* ScriptableForm = CS_CAST(Form, TESForm, TESScriptableForm);

		if ((Quest && ScriptForm->info.type != Script::kScriptType_Quest) ||
			(BoundObj && ScriptForm->info.type != Script::kScriptType_Object))
		{
			BGSEEUI->MsgBoxW(Parent, 0, "Script type doesn't correspond to binding form.");
		}
		else if (ScriptableForm == nullptr)
			BGSEEUI->MsgBoxW(Parent, 0, "Binding form isn't scriptable.");
		else
		{
			ScriptableForm->script = ScriptForm;
			ScriptForm->AddCrossReference(Form);
			Form->SetFromActiveFile(true);

			BGSEEUI->MsgBoxI(Parent, 0, "Script '%s' bound to form '%s'", ScriptForm->editorID.c_str(), Form->editorID.c_str());
		}
	}*/
}

void SetScriptText(void* CurrentScript, const char* ScriptText)
{
	Script* ScriptForm = CS_CAST(CurrentScript, TESForm, Script);
	ScriptForm->SetText(ScriptText);
}

void UpdateScriptVarNames(const char* EditorID, componentDLLInterface::ScriptVarRenameData* Data)
{
	TESForm* Form = TESForm::LookupByEditorID(EditorID);

	if (Form)
	{
		Script* ScriptForm = CS_CAST(Form, TESForm, Script);

		if (ScriptForm)
		{
			BGSEECONSOLE_MESSAGE("Updating script '%s' variable names...", ScriptForm->editorID.c_str());
			BGSEECONSOLE->Indent();
			for (int i = 0; i < Data->ScriptVarListCount; i++)
			{
				ScriptVarRenameData::ScriptVarInfo* VarInfo = &Data->ScriptVarListHead[i];
				Script::VariableInfo* ScriptVar = ScriptForm->LookupVariableInfoByName(VarInfo->OldName);

				if (ScriptVar)
				{
					ScriptVar->name.Set(VarInfo->NewName);
					BGSEECONSOLE_MESSAGE("Variable '%s' renamed to '%s'", VarInfo->OldName, VarInfo->NewName);

					Script::RefVariable* RefVar = ScriptForm->LookupRefVariableByIndex(ScriptVar->index);
					if (RefVar && !RefVar->form)
						RefVar->name.Set(VarInfo->NewName);
				}
			}

			BGSEECONSOLE->Outdent();
			ScriptForm->SetFromActiveFile(true);
		}
	}
}

bool CanUpdateIntelliSenseDatabase(void)
{
	if (BGSEEDAEMON->GetFullInitComplete() == false)
		return false;
	else if (BGSEEDAEMON->IsCrashing())
		return false;
	else if (BGSEEDAEMON->IsDeinitializing())
		return false;
	else if (TESDataHandler::PluginLoadSaveInProgress)
		return false;

	return true;
}

const char* GetDefaultCachePath(void)
{
	static const std::string kBuffer = bgsee::ResourceLocation(GECKE_SEDEPOT)();
	return kBuffer.c_str();
}

const char* GetAutoRecoveryCachePath(void)
{
	static const std::string kBuffer = bgsee::ResourceLocation(GECKE_SEAUTORECDEPOT)();
	return kBuffer.c_str();
}

const char* GetPreprocessorBasePath(void)
{
	static const std::string kBuffer = bgsee::ResourceLocation(GECKE_SEPREPROCDEPOT)();
	return kBuffer.c_str();
}

const char* GetPreprocessorStandardPath(void)
{
	static const std::string kBuffer = bgsee::ResourceLocation(GECKE_SEPREPROCSTDDEPOT)();
	return kBuffer.c_str();
}

const char* GetSnippetCachePath(void)
{
	static const std::string kBuffer = bgsee::ResourceLocation(GECKE_SESNIPPETDEPOT)();
	return kBuffer.c_str();
}

ScriptVarRenameData* AllocateVarRenameData(UInt32 VarCount)
{
	ScriptVarRenameData* Data = new ScriptVarRenameData();
	Data->ScriptVarListHead = new ScriptVarRenameData::ScriptVarInfo[VarCount];
	Data->ScriptVarListCount = VarCount;

	return Data;
}

ScriptCompileData* AllocateCompileData(void)
{
	ScriptCompileData* Data = new ScriptCompileData();
	return Data;
}

#pragma endregion
/**** END SCRIPTEDITOR SUBINTERFACE ****/

/**** BEGIN USEINFOLIST SUBINTERFACE ****/
#pragma region UseInfoList
template <typename tData>
void AddLinkedListContentsToFormList(tList<tData>* List, FormListData* FormList, UInt32& CurrentIndex)
{
	NOT_IMPLEMENTED;
}

UseInfoListFormData* GetLoadedForms(void)
{
	NOT_IMPLEMENTED;
}

UseInfoListCrossRefData* GetCrossRefDataForForm(const char* EditorID)
{
	NOT_IMPLEMENTED;
}

UseInfoListCellItemListData* GetCellRefDataForForm(const char* EditorID)
{
	NOT_IMPLEMENTED;
}
/**** END USEINFOLIST SUBINTERFACE ****/
#pragma endregion


/**** BEGIN TAGBROWSER SUBINTERFACE ****/
#pragma region TagBrowser
void InstantiateObjects(TagBrowserInstantiationData* Data)
{
	NOT_IMPLEMENTED;
}

void InitiateDragonDrop(void)
{
	NOT_IMPLEMENTED;
}

TagBrowserInstantiationData* AllocateInstantionData(UInt32 FormCount)
{
	NOT_IMPLEMENTED;
}
#pragma endregion
/**** END TAGBROWSER SUBINTERFACE ****/

componentDLLInterface::CSEInterfaceTable g_InteropInterface =
{
	DeleteInterOpData,
	{
		ComponentDLLDebugPrint,
		WriteToStatusBar,
		GetAppPath,
		GetCSMainWindowHandle,
		GetRenderWindowHandle,
		LookupFormByEditorID,
		LookupScriptableByEditorID,
		GetFormTypeIDLongName,
		LoadFormForEditByEditorID,
		LoadFormForEditByFormID,
		ShowPickReferenceDialog,
		ShowUseReportDialog,
		SaveActivePlugin,
		ReadFromINI,
		WriteToINI,
		GetFormListActiveItemForegroundColor,
		GetFormListActiveItemBackgroundColor,
		GetShouldColorizeActiveForms,
		GetShouldSortActiveFormsFirst,
		RegisterConsoleContext,
		DeregisterConsoleContext,
		PrintToConsoleContext,
	},
	{
		CreateNewScript,
		DestroyScriptInstance,
		IsUnsavedNewScript,
		CompileScript,
		RecompileScripts,
		ToggleScriptCompilation,
		DeleteScript,
		GetPreviousScriptInList,
		GetNextScriptInList,
		RemoveScriptBytecode,
		SaveEditorBoundsToINI,
		GetScriptList,
		GetScriptVarList,
		UpdateScriptVarIndices,
		CompileDependencies,
		GetIntelliSenseUpdateData,
		BindScript,
		SetScriptText,
		UpdateScriptVarNames,
		CanUpdateIntelliSenseDatabase,
		GetDefaultCachePath,
		GetAutoRecoveryCachePath,
		GetPreprocessorBasePath,
		GetPreprocessorStandardPath,
		GetSnippetCachePath,
		AllocateVarRenameData,
		AllocateCompileData,
	},
	{
		GetLoadedForms,
		GetCrossRefDataForForm,
		GetCellRefDataForForm,
	},
	{
		InstantiateObjects,
		InitiateDragonDrop,
		AllocateInstantionData,
	}
};