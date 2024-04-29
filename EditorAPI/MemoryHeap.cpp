#include "MemoryHeap.h"

MemoryHeap*				MemoryHeap::FormHeap = (MemoryHeap*)0x00F21B5C;

namespace gecke_overrides
{
	void* FormHeap_Allocate( UInt32 Size )
	{
		return cdeclCall<void*>(0x00401000, Size);
	}

	void FormHeap_Free( void* Ptr )
	{
		cdeclCall<void>(0x00401180, Ptr);
	}
}

// Since the NVSE source is an utterly broken mess...
const _FormHeap_Allocate FormHeap_Allocate = (_FormHeap_Allocate)0x00401000;
extern const _FormHeap_Free FormHeap_Free = (_FormHeap_Free)0x00401180;