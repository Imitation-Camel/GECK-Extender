#include "TESForm.h"
#include "Core.h"

bool TESForm::SetEditorID(const char* EditorID)
{
	if (IsTemporary() && EditorID == nullptr)
	{
		this->editorID.Clear();
		return true;
	}

	SME_ASSERT(EditorID);
	return thisCall<bool>(0x004FB450, this, EditorID);
}

TESFile* TESForm::GetOverrideFile(int Index) const
{
	int CurrentIndex = 0;
	TESFile* File = nullptr;
	for (auto Itr = fileList.Begin(); !Itr.End(); ++Itr)
	{
		File = Itr.Get();
		if (CurrentIndex == Index)
			break;

		++CurrentIndex;
	}
	return File;
}

bool TESForm::UpdateUsageInfo()
{
	return thisVirtualCall<bool>(0x138, this);
}

void TESForm::SetFromActiveFile(bool State)
{
	thisVirtualCall<UInt32>(0xA8, this, State);
}

void TESForm::SetDeleted(bool State)
{
	thisVirtualCall<UInt32>(0xA4, this, State);
}

TESForm* TESForm::LookupByFormID(UInt32 FormID)
{
	return cdeclCall<TESForm*>(0x004F9620, FormID);
}

TESForm* TESForm::LookupByEditorID(const char* EditorID)
{
	return cdeclCall<TESForm*>(0x004F9650, EditorID);
}

bool TESForm::IsReference() const
{
	return formType >= kFormType_TESObjectREFR && formType <= kFormType_Creature;
}

bool TESForm::IsActive() const
{
	return (formFlags & kFormFlags_FromActiveFile);
}

bool TESForm::IsQuestItem() const
{
	return (formFlags & kFormFlags_QuestItem);
}


const char* TESForm::GetFormTypeIDLongName( UInt8 TypeID )
{
	return cdeclCall<const char*>(0x00523860, TypeID);
}


TESForm* TESForm::CreateInstance(UInt8 TypeID)
{
	return _DATAHANDLER->CreateForm(TypeID);
}

void TESForm::DeleteInstance()
{
	thisVirtualCall<UInt32>(0x30, this, true);
}

const char* TESForm::GetEditorID() const
{
	return editorID.c_str();
}

bool TESForm::GetFromActiveFile() const
{
	return (formFlags & kFormFlags_FromActiveFile);
}

bool TESForm::IsDeleted() const
{
	return (formFlags & kFormFlags_Deleted);
}

bool TESForm::IsTemporary() const
{
	return (formFlags & kFormFlags_Temporary);
}

bool TESForm::IsVWD() const
{
	return (formFlags & kFormFlags_VisibleWhenDistant);
}

bool TESForm::IsInitiallyDisabled() const
{
	return (formFlags & kFormFlags_Disabled);
}
