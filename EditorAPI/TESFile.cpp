#include "TESFile.h"


bool TESFile::IsActive( void ) const
{
	return fileFlags & kFileFlag_Active;
}

bool TESFile::IsMaster( void ) const
{
	return fileFlags & kFileFlag_Master;
}

bool TESFile::IsLoaded( void ) const
{
	return fileFlags & kFileFlag_Loaded;
}