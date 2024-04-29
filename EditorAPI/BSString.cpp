#include "BSString.h"
#include "Core.h"

bool BSString::Set(const char* string, SInt16 size)   // size determines allocated storage? 0 to allocate automatically
{
	return thisCall<bool>(0x00405B40, this, string, size);
}

void BSString::Clear()
{
	thisCall<UInt32>(0x0040CBF0, this);
}

SInt16 BSString::Size() const
{
	if (m_dataLen == 0xFFFF)
		return strlen(m_data);
	else
		return m_dataLen;
}

SInt16 BSString::Compare(const char* string, bool ignoreCase)
{
	if (ignoreCase)
		return _stricmp(m_data, string);
	else
		return strcmp(m_data, string);
}

BSString* BSString::CreateInstance(const char* String)
{
	BSString* NewInstance = (BSString*)gecke_overrides::FormHeap_Allocate(sizeof(BSString));
	thisCall<UInt32>(0x004070F0, NewInstance, String);
	return NewInstance;
}

void BSString::DeleteInstance(bool ReleaseMemory)
{
	Clear();
	if (ReleaseMemory)
		gecke_overrides::FormHeap_Free(this);
}