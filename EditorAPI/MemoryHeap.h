#pragma once

//	EditorAPI: MemoryHeap class.
//	A number of class definitions are directly derived from the COEF API; Credit to JRoush for his comprehensive decoding

/*
	Memory heap class and it's main global instance, the FormHeap.
	This appears to be Bethesda's answer to the global new & delete operators.
*/

// ?
class MemoryHeap
{
public:
	// memory panic callback
	typedef void (*Unk164Callback)(UInt32 unk0, UInt32 unk1, UInt32 unk2);

	// members
	//     /*000*/ void**        vtbl;

	static MemoryHeap*			FormHeap;
};

namespace gecke_overrides
{
	void*	FormHeap_Allocate(UInt32 Size);
	void	FormHeap_Free(void* Ptr);
}

// macro for overloading new & delete operators to use the FormHeap
#define USEFORMHEAP inline void* operator new (size_t size) {return FormHeap_Allocate(size);} \
					inline void* operator new[] (size_t size) {return FormHeap_Allocate(size);} \
					inline void operator delete (void *object) {FormHeap_Free(object);} \
					inline void operator delete[] (void *object) {FormHeap_Free(object);}