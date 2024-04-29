#include "Core.h"

using namespace gecke;

TESDataHandler**					TESDataHandler::Singleton = (TESDataHandler **)0x00ED3B0C;
bool								TESDataHandler::PluginLoadSaveInProgress = false;

TES**								TES::Singleton = (TES**)0x00ECF93C;

void TESDataHandler::SortScripts()
{
	thisCall<UInt32>(0x004CF0D0, this);
}

TESForm* TESDataHandler::CreateForm(UInt8 TypeID)
{
	return cdeclCall<TESForm*>(0x004CF460, TypeID);
}
