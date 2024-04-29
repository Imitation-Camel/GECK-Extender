#pragma once

#include "ExtraData.h"

//	EditorAPI: ExtraDataList class and its derivatives.
//	A number of class definitions are directly derived from the COEF API; Credit to JRoush for his comprehensive decoding

/*
	ExtraData is Bethesda's tool for attaching arbitrary information to other classes.
	BaseExtraList seems to be the generic extra data manager, and is used for DialogExtraData
	ExtraDataList seems to be targeted specifically to forms (?)
*/

class	TESForm;
class	TESObjectREFR;
class	TESGlobal;
class	TESObjectLAND;

// 20
class BaseExtraList
{
public:
	// members
	//     /*00*/ void**			vtbl
	/*04*/ BSExtraData*				extraList;				// LL of extra data nodes
	/*08*/ UInt8					extraTypes[0x15];		// if a bit is set, then the extralist should contain that extradata
															// bits are numbered starting from the lsb
	/*13*/ UInt8					pad13[3];

	// ### HACK - shitty workaround to allow instantiation
	inline virtual void					Dtor(void) {}
};
static_assert(sizeof(BaseExtraList) == 0x20);

// 20
class ExtraDataList : public BaseExtraList
{
public:
	// no additional members
};
static_assert(sizeof(ExtraDataList) == 0x20);