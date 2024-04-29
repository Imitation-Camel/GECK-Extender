#include "Script.h"
#include "Hooks\Hooks-ScriptEditor.h"

using namespace gecke;

TESScriptCompiler::_ShowMessage			TESScriptCompiler::ShowMessage = (TESScriptCompiler::_ShowMessage)0x005C5730;
bool									TESScriptCompiler::PreventErrorDetours = false;
bool									TESScriptCompiler::PrintErrorsToConsole = true;
TESScriptCompiler::CompilerMessageArrayT	TESScriptCompiler::LastCompilationMessages;

Script::VariableInfo* Script::LookupVariableInfoByName(const char* Name)
{
	for (VariableListT::Iterator Itr = varList.Begin(); !Itr.End(); ++Itr)
	{
		VariableInfo* Variable = Itr.Get();

		if (Variable && !Variable->name.Compare(Name))
			return Variable;
	}

	return nullptr;
}

Script::RefVariable* Script::LookupRefVariableByIndex(UInt32 Index)
{
	UInt32 Idx = 1;	// yes, really starts at 1

	for (RefVariableListT::Iterator Itr = refList.Begin(); !Itr.End(); ++Itr)
	{
		RefVariable* Variable = Itr.Get();

		if (Variable && Idx == Index)
			return Variable;

		Idx++;
	}

	return nullptr;
}

bool Script::Compile(bool AsResultScript)
{
	if (this->text == nullptr)
		return false;

	if (AsResultScript) {
		NOT_IMPLEMENTED;
	}
	else
		return thisCall<bool>(0x005C9800, 0x00ECFDF8, this, 0);
}

void Script::SetText(const char* Text)
{
	thisCall<UInt32>(0x005C27B0, this, Text);
}

bool Script::IsObjectScript() const
{
	return info.type == kScriptType_Object;
}

bool Script::IsQuestScript() const
{
	return info.type == kScriptType_Quest;
}

bool Script::IsMagicScript() const
{
	return info.type == kScriptType_Magic;
}

bool Script::IsUserDefinedFunctionScript() const
{
	// TODO : fix for NVSE
	return false;

	if (!IsObjectScript())
		return false;
	else if (info.dataLength < 15)
		return false;

	// need to SEH-wrap this to be safe
	__try
	{
		auto Data = (UInt8*)data;
		if (Data && *(Data + 8) == 7)
			return true;
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {}

	return false;
}

void TESScriptCompiler::ToggleScriptCompilation( bool State )
{
	if (!State)
		hooks::_MemHdlr(ToggleScriptCompilingNewData).WriteBuffer();
	else
		hooks::_MemHdlr(ToggleScriptCompilingOriginalData).WriteBuffer();
}


void Script::RemoveCompiledData(void)
{
	gecke_overrides::FormHeap_Free(data);
	data = nullptr;

	info.dataLength = 0;
	info.lastVarIdx = 0;
	info.refCount = 0;
	info.compileResult = 0;

	thisCall<void>(0x005C4F40, this);		// cleanup ref var list
	thisCall<void>(0x005C4EC0, this);		// cleanup var list
}

TESScriptCompiler::CompilerMessage::CompilerMessage(UInt32 Line, const char* Message)
{
	this->Line = Line;
	this->Message = Message;
	this->Type = MessageType::Error;
	this->MessageCode = 0;

	// by default, the vanilla compiler only returns error messages
	// OBSE, on the other hand, supports warning messages that don't result in an compilation failure
	// warning messages are prefixed with a tag to specify message type and an additional warning code

	static std::regex WarningRegEx("^\\[WARNING (\\d+)\\] (.*)$");

	std::smatch WarningMatches;
	if (std::regex_search(this->Message, WarningMatches, WarningRegEx))
	{
		SME_ASSERT(WarningMatches.size() == 3);

		try {
			this->MessageCode = std::stoul(WarningMatches[1]);
		} catch (...) {}
		this->Message = WarningMatches[2];
		this->Type = MessageType::Warning;
	}
}
