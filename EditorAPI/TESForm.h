#pragma once

#include "BaseFormComponent.h"

//	EditorAPI: TESForm class.
//	A number of class definitions are directly derived from the COEF API; Credit to JRoush for his comprehensive decoding

/*
	TESForm is the base class for all object 'types' (or 'Base forms') in the game.
	It provides a common, very sophisticated interface for Serialization, Revision Control,
	Inter-form references, and editing (in the CS).

	NOTE on the CS reference-tracking:
	A Form may point to other forms through it's fields; these are it's "Form", or "Component" references.
	These list of other forms that point to this one are it's "Cross" references.
	The exception is if this form is a base for a placed TESObjectREFR - refs that point to this form are it's "Object" references.
	The notation is admittedly awkward, a result of choosing names before a clear picture of the system emerged.
	Form and Cross references are tracked individually through a global table.  Object refs are not tracked individually, but
	every instance of TESBoundObject does maintain a cell-by-cell count.
*/

class   TESFile;
class   RecordInfo;
class   TESObjectREFR;
class	TESForm;

// Used by the built-in revision-control in the CS, which seems to be disabled in the public version
// This struct is public (not a subclass of TESForm) because it is also used by TESFile
// 08
struct TrackingData
{
	UInt32		vcMasterFormID;		// 00 - Version control 1 (looks to be a refID inside the Version Control master)
	UInt32		vcRevision;			// 04
};

// 24
class TESForm : public BaseFormComponent
{
public:
	typedef tList<TESForm>  VanillaFormCrossReferenceList;  // for reference tracking in the CS

	enum FormType
	{
		kFormType_None = 0,					// 00
		kFormType_TES4,
		kFormType_Group,
		kFormType_GMST,
		kFormType_BGSTextureSet,
		kFormType_BGSMenuIcon,
		kFormType_TESGlobal,
		kFormType_TESClass,
		kFormType_TESFaction,					// 08
		kFormType_BGSHeadPart,
		kFormType_TESHair,
		kFormType_TESEyes,
		kFormType_TESRace,
		kFormType_TESSound,
		kFormType_BGSAcousticSpace,
		kFormType_TESSkill,
		kFormType_EffectSetting,					// 10
		kFormType_Script,
		kFormType_TESLandTexture,
		kFormType_EnchantmentItem,
		kFormType_SpellItem,
		kFormType_TESObjectACTI,
		kFormType_BGSTalkingActivator,
		kFormType_BGSTerminal,
		kFormType_TESObjectARMO,					// 18	inv object
		kFormType_TESObjectBOOK,						// 19	inv object
		kFormType_TESObjectCLOT,					// 1A	inv object
		kFormType_TESObjectCONT,
		kFormType_TESObjectDOOR,
		kFormType_IngredientItem,				// 1D	inv object
		kFormType_TESObjectLIGH,					// 1E	inv object
		kFormType_TESObjectMISC,						// 1F	inv object
		kFormType_TESObjectSTAT,					// 20
		kFormType_BGSStaticCollection,
		kFormType_BGSMovableStatic,
		kFormType_BGSPlaceableWater,
		kFormType_TESGrass,
		kFormType_TESObjectTREE,
		kFormType_TESFlora,
		kFormType_TESFurniture,
		kFormType_TESObjectWEAP,					// 28	inv object
		kFormType_TESAmmo,						// 29	inv object
		kFormType_TESNPC,						// 2A
		kFormType_TESCreature,					// 2B
		kFormType_TESLevCreature,			// 2C
		kFormType_TESLevCharacter,			// 2D
		kFormType_TESKey,						// 2E	inv object
		kFormType_AlchemyItem,				// 2F	inv object
		kFormType_BGSIdleMarker,				// 30
		kFormType_BGSNote,						// 31	inv object
		kFormType_BGSConstructibleObject,		// 32	inv object
		kFormType_BGSProjectile,
		kFormType_TESLevItem,				// 34	inv object
		kFormType_TESWeather,
		kFormType_TESClimate,
		kFormType_TESRegion,
		kFormType_NavMeshInfoMap,						// 38
		kFormType_TESObjectCELL,
		kFormType_TESObjectREFR,				// 3A
		kFormType_Character,						// 3B
		kFormType_Creature,						// 3C
		kFormType_MissileProjectile,						// 3D
		kFormType_GrenadeProjectile,						// 3E
		kFormType_BeamProjectile,						// 3F
		kFormType_FlameProjectile,						// 40
		kFormType_TESWorldSpace,
		kFormType_TESObjectLAND,
		kFormType_NavMesh,
		kFormType_TLOD,
		kFormType_TESTopic,
		kFormType_TESTopicInfo,
		kFormType_TESQuest,
		kFormType_TESIdleForm,						// 48
		kFormType_TESPackage,
		kFormType_TESCombatStyle,
		kFormType_TESLoadScreen,
		kFormType_TESLevSpell,
		kFormType_TESObjectANIO,
		kFormType_TESWaterForm,
		kFormType_TESEffectShader,
		kFormType_TOFT,						// 50	table of Offset (see OffsetData in Worldspace)
		kFormType_BGSExplosion,
		kFormType_BGSDebris,
		kFormType_TESImageSpace,
		kFormType_TESImageSpaceModifier,
		kFormType_BGSListForm,					// 55
		kFormType_BGSPerk,
		kFormType_BGSBodyPartData,
		kFormType_BGSAddonNode,				// 58
		kFormType_ActorValueInfo,
		kFormType_BGSRadiationStage,
		kFormType_BGSCameraShot,
		kFormType_BGSCameraPath,
		kFormType_BGSVoiceType,
		kFormType_BGSImpactData,
		kFormType_BGSImpactDataSet,
		kFormType_TESObjectARMA,						// 60
		kFormType_BGSEncounterZone,
		kFormType_BGSMessage,
		kFormType_BGSRagdoll,
		kFormType_DOBJ,
		kFormType_BGSLightingTemplate,
		kFormType_BGSMusicType,
		kFormType_TESObjectIMOD,					// 67	inv object
		kFormType_TESReputation,				// 68
		kFormType_ContinuousBeamProjectile,						// 69 Continuous Beam
		kFormType_TESRecipe,
		kFormType_TESRecipeCategory,
		kFormType_TESCasinoChips,				// 6C	inv object
		kFormType_TESCasino,
		kFormType_TESLoadScreenType,
		kFormType_MediaSet,
		kFormType_MediaLocationController,	// 70
		kFormType_TESChallenge,
		kFormType_TESAmmoEffect,
		kFormType_TESCaravanCard,				// 73	inv object
		kFormType_TESCaravanMoney,				// 74	inv object
		kFormType_TESCaravanDeck,
		kFormType_BGSDehydrationStage,
		kFormType_BGSHungerStage,
		kFormType_BGSSleepDeprevationStage,	// 78
		kFormType__MAX
	};

	enum FormFlags
	{
		kFormFlags_FromMaster           = /*00*/ 0x00000001,   // form is from an esm file
		kFormFlags_FromActiveFile       = /*01*/ 0x00000002,   // form is overriden by active mod or save file
		kFormFlags_Linked               = /*03*/ 0x00000008,   // set after formids have been resolved into TESForm*
		kFormFlags_Deleted              = /*05*/ 0x00000020,   // set on deletion, not saved in CS or savegame
		kFormFlags_BorderRegion         = /*06*/ 0x00000040,   // ?? (from TES4Edit)
		kFormFlags_TurnOffFire          = /*07*/ 0x00000080,   // ?? (from TES4Edit)
		kFormFlags_CastShadows          = /*09*/ 0x00000200,   // ?? (from TES4Edit)
		kFormFlags_QuestItem            = /*0A*/ 0x00000400,   // aka Persistent Reference for TESObjectREFR
		kFormFlags_Disabled             = /*0B*/ 0x00000800,   // (TESObjectREFR)
		kFormFlags_Ignored              = /*0C*/ 0x00001000,   // (records)
		kFormFlags_Temporary            = /*0E*/ 0x00004000,   // not saved in CS, probably game as well
		kFormFlags_VisibleWhenDistant   = /*0F*/ 0x00008000,   // ?? (from TES4Edit)
		kFormFlags_OffLimits            = /*11*/ 0x00020000,   // (TESObjectCELL)
		kFormFlags_Compressed           = /*12*/ 0x00040000,   // (records)
		kFormFlags_CantWait             = /*13*/ 0x00080000,   // (TESObjectCELL/TESWorldSpace)
		kFormFlags_IgnoresFriendlyHits  = /*14*/ 0x00100000,
	};

	// 0C
	struct FormTypeInfo
	{
		UInt32          formType;   // 00 form type code, also offset in array
		const char*     shortName;  // 04 offset to 4-letter type descriptor: 'GRUP', 'ARMO', etc.
		UInt32          chunkType;  // 08 appears to be name string in byte-reversed order, used to mark form records
	};

	typedef tList<TESFile>			OverrideFileListT;

	// members
	//     /*00*/ void**            vtbl;
	/*04*/ UInt8					formType;
	/*05*/ UInt8					formPad05[3];
	/*08*/ UInt32					formFlags;
	/*0C*/ UInt32					formID;
	/*10*/ BSString					editorID;
	/*18*/ TrackingData				trackingData;
	/*1C*/ OverrideFileListT		fileList; // list of TESFiles that override this form

	// methods
	const char*						GetEditorID() const;
	bool							IsReference() const;
	bool							IsActive() const;
	bool							IsQuestItem() const;
	bool							IsDeleted() const;
	bool							IsTemporary() const;
	bool							IsVWD() const;
	bool							IsInitiallyDisabled() const;

	bool							UpdateUsageInfo();
	bool							SetEditorID(const char* EditorID);
	TESFile*						GetOverrideFile(int Index) const;

	void							SetFromActiveFile(bool State);
	bool							GetFromActiveFile() const;
	void							SetDeleted(bool State);

	static TESForm*					CreateInstance(UInt8 TypeID);
	void							DeleteInstance();

	static TESForm*					LookupByFormID(UInt32 FormID);
	static TESForm*					LookupByEditorID(const char* EditorID);
	static const char*				GetFormTypeIDLongName(UInt8 TypeID);
};
STATIC_ASSERT(sizeof(TESForm) == 0x24);

typedef std::vector<TESForm*>	TESFormArrayT;
typedef std::vector<UInt32>		FormIDArrayT;

/*
	This class is apparently used to centralize code for certain types of dialog windows in the CS.
	It does not appear in the RTTI structure of the game code.
*/
// 24
class TESFormIDListView : public TESForm
{
public:
	// no additional members

};
STATIC_ASSERT(sizeof(TESFormIDListView) == 0x24);

/*
	TESMemContextForm is a parent class for TESObjectCELL and TESObjectREFR.  It has no members and is not polymorphic,
	so it probably has only static methods & data.  It may have something to do with the 'Memory Usage' debugging code
	used by Bethesda, and it is possible that it has no use at all in the released game.
*/
// 00
class TESMemContextForm
{
	// no members
};