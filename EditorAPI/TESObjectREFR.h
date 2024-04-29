#pragma once

#include "nvse\NiNodes.h"

#include "TESForm.h"
#include "ExtraDataList.h"

//	EditorAPI: TESObjectREFR class.
//	A number of class definitions are directly derived from the COEF API; Credit to JRoush for his comprehensive decoding

/*
	TESObjectREFR is the parent for all 'instances' of base forms in the game world
	While each ref is a distinct form, it also points back to the 'base' form that it instantiates
	Refs store local data like position, size, flags, etc.
*/

// 84
class TESObjectREFR : public TESForm //, public TESChildCell, public TESMemContextForm TODO: fix hierarchy
{
public:
	enum
	{
		kSpecialFlags_3DInvisible				= 1 << 31,
		kSpecialFlags_Children3DInvisible		= 1 << 30,
		kSpecialFlags_Frozen					= 1 << 29,
	};

	enum
	{
		kNiNodeSpecialFlags_SpecialFade			= 1 << 14,
	};

	// members
	//     /*00*/ TESForm
	//     /*24*/ TESChildCell
	//     /*28*/ TESMemContextForm - empty, no members
	/*28*/ UInt32				unk28;
	/*2C*/ void*				childCell;
	/*30*/ UInt32				unk30;;
	/*34*/ TESForm*				baseForm;
	/*38*/ Vector3				rotation;
	/*44*/ Vector3				position;
	/*50*/ float				scale;
	/*54*/ TESObjectCELL*		parentCell;
	/*58*/ ExtraDataList		extraData;
	/*78*/ UInt32				unk78;
	/*7C*/ float				unk7C;
	/*80*/ void*				unk80;
};
static_assert(sizeof(TESObjectREFR) == 0x84);

typedef std::vector<TESObjectREFR*>	TESObjectREFRArrayT;

#define REFR_DEG2RAD			0.01745329238474369f
#define REFR_RAD2DEG			57.2957763671875f

