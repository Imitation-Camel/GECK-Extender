#pragma once


#include "TESForm.h"

//	EditorAPI: Core classes.

class	BSFile;
struct	TrackingData;
class   TESObjectListHead;
class   TESFile;
class   TESForm;
class   TESObject;
class   TESPackage;
class   TESWorldSpace;
class   TESClimate;
class   TESWeather;
class   EnchantmentItem;
class   SpellItem;
class   TESHair;
class   TESEyes;
class   TESRace;
class   TESLandTexture;
class   TESClass;
class   TESFaction;
class   Script;
class   TESSound;
class   TESGlobal;
class   TESTopic;
class   TESQuest;
class   BirthSign;
class   TESCombatStyle;
class   TESLoadScreen;
class   TESWaterForm;
class   TESEffectShader;
class   TESObjectANIO;
class   TESRegionList;
class   TESObjectCELL;
class   TESRegionDataManager;
class   TESSoulGem;
class   TESObjectSTAT;
class   TESObjectCLOT;
class   TESObjectMISC;
class   TESObjectCONT;
class   TESObjectDOOR;
class   TESObjectREFR;
class   ContainerExtraData;
class	TESNPC;
class	GridDistantArray;
class	GridCellArray;
class	Sky;
class	BSTextureManager;
class	BSRenderedTexture;
class	NiDX9Renderer;
class	Setting;
class	NiBinaryStream;
class	NiFile;
class	NiRenderTargetGroup;
class	BSFileEntry;
class	BackgroundCloneThread;
class	Model;
class	KFModel;
class	QueuedReference;
class	QueuedAnimIdle;
class	QueuedHelmet;
class	AttachDistant3DTask;
class TESImageSpace;
class TESImageSpaceModifier;
class BGSHeadPart;
class BGSEncounterZone;
class BGSCameraShot;
class TESReputation;
class TESChallenge;

class TESRecipe;
class TESRecipeCategory;
class TESAmmoEffect;
class TESCasino;
class TESCaravanDeck;
class Script;
class TESSound;
class BGSAcousticSpace;
class BGSRagdoll;
class TESGlobal;
class BGSVoiceType;
class BGSImpactData;
class BGSImpactDataSet;
class TESTopic;
class TESTopicInfo;
class TESQuest;
class TESCombatStyle;
class TESLoadScreen;
class TESWaterForm;
class TESEffectShader;
class BGSProjectile;
class BGSExplosion;
class BGSRadiationStage;
class BGSDehydrationStage;
class BGSHungerStage;
class BGSSleepDeprivationStage;
class BGSDebris;
class BGSPerk;
class BGSBodyPartData;
class BGSNote;
class BGSListForm;
class BGSMenuIcon;
class TESObjectANIO;
class BGSMessage;
class BGSLightingTemplate;
class BGSMusicType;
class TESLoadScreenType;
class MediaSet;
class MediaLocationController;
class BGSAddonNode;
class TESRegionManager;

// 640
class TESDataHandler
{
public:
	// members
	UInt32							unk00;					// 000
	TESObjectListHead				* boundObjectList;		// 004
	tList<TESPackage>				packageList;			// 008
	tList<TESWorldSpace>			worldSpaceList;			// 010
	tList<TESClimate>				climateList;			// 019
	tList<TESImageSpace>			imageSpaceList;			// 020
	tList<TESImageSpaceModifier>	imageSpaceModList;		// 028
	tList<TESWeather>				weatherList;			// 030
	tList<EnchantmentItem>			enchantmentItemList;	// 038
	tList<SpellItem>				spellItemList;			// 040
	tList<BGSHeadPart>				headPartList;			// 048
	tList<TESHair>					hairList;				// 050
	tList<TESEyes>					eyeList;				// 058
	tList<TESRace>					raceList;				// 060
	tList<BGSEncounterZone>			encounterZoneList;		// 068
	tList<TESLandTexture>			landTextureList;		// 070
	tList<BGSCameraShot>			cameraShotList;			// 078
	tList<TESClass>					classList;				// 080
	tList<TESFaction>				factionList;			// 088
	tList<TESReputation>			reputationList;			// 090
	tList<TESChallenge>				challengeList;			// 098
	tList<TESRecipe>				recipeList;				// 0A0
	tList<TESRecipeCategory>		recipeCategoryList;		// 0A8
	tList<TESAmmoEffect>			ammoEffectList;			// 0B0
	tList<TESCasino>				casinoList;				// 0B8
	tList<TESCaravanDeck>			caravanDeckList;		// 0C0
	tList<Script>					scriptList;				// 0C8
	tList<TESSound>					soundList;				// 0D0
	tList<BGSAcousticSpace>			acousticSpaceList;		// 0D8
	tList<BGSRagdoll>				ragdollList;			// 0E0
	tList<TESGlobal>				globalList;				// 0E8
	tList<BGSVoiceType>				voiceTypeList;			// 0F0
	tList<BGSImpactData>			impactDataList;			// 0F8
	tList<BGSImpactDataSet>			impactDataSetList;		// 100
	tList<TESTopic>					topicList;				// 108
	tList<TESTopicInfo>				topicInfoList;			// 110
	tList<TESQuest>					questList;				// 118
	tList<TESCombatStyle>			combatStyleList;		// 120
	tList<TESLoadScreen>			loadScreenList;			// 128
	tList<TESWaterForm>				waterFormList;			// 130
	tList<TESEffectShader>			effectShaderList;		// 138
	tList<BGSProjectile>			projectileList;			// 140
	tList<BGSExplosion>				explosionList;			// 148
	tList<BGSRadiationStage>		radiationStageList;		// 150
	tList<BGSDehydrationStage>		dehydrationStageList;	// 158
	tList<BGSHungerStage>			hungerStageList;		// 160
	tList<BGSSleepDeprivationStage>	sleepDepriveStageList;	// 168
	tList<BGSDebris>				debrisList;				// 170
	tList<BGSPerk>					perkList;				// 178
	tList<BGSBodyPartData>			bodyPartDataList;		// 180
	tList<BGSNote>					noteList;				// 188
	tList<BGSListForm>				listFormList;			// 190
	tList<BGSMenuIcon>				menuIconList;			// 198
	tList<TESObjectANIO>			anioList;				// 1A0
	tList<BGSMessage>				messageList;			// 1A8
	tList<BGSLightingTemplate>		lightningTemplateList;	// 1B0
	tList<BGSMusicType>				musicTypeList;			// 1B8
	tList<TESLoadScreenType>		loadScreenTypeList;		// 1C0
	tList<MediaSet>					mediaSetList;			// 1C8
	tList<MediaLocationController>	mediaLocControllerList;	// 1D0
	TESRegionList					* regionList;			// 1D8
	NiTArray<TESObjectCELL*>		cellArray;				// 1DC
	NiTArray<BGSAddonNode*>			addonArray;				// 1EC

	UInt32							unk1FC[0x3];			// 1FC	208 looks like next created refID
	UInt32							nextCreatedRefID;		// 208	Init'd to FF000800 (in GECK init'd to nn000800)
	TESFile*						activeFile;
	tList<TESFile>					fileList;   // all files in Oblivion\Data\ directory
	UInt32							fileCount;  // loaded files
	TESFile*						filesByID[0xFF]; // loaded files
	UInt8							unk618;					// 618	5A4
	UInt8							unk619;					// 619
	UInt8							unk61A;					// 61A	referenced during LoadForm (ie TESSpellList). bit 1 might mean refID to pointer conversion not done. For GECK means save in progress
	UInt8							unk61B;					// 61B
	UInt8							unk61C;					// 61C	5A8
	UInt8							unk61D;					// 61D
	UInt8							unk61E;					// 61E
	UInt8							unk61F;					// 61F
	UInt8							unk620;					// 620	5AC
	UInt8							loading;				// 621	Init'd to 0 after loadForms
	UInt8							unk622;					// 622	referenced during loading of modules. Compared with type = GameSetting but seems to always end up equal to 1.
	UInt8							unk623;					// 623
	TESRegionManager				* regionManager;		// 624	5B0
	UInt32							unk628;					// 628	5B4
	UInt32							unk62C;					// 62C	
	UInt32							unk630;					// 630
	UInt32							unk634;					// 634
	UInt32							unk638;					// 638

	void SortScripts();
	TESForm* CreateForm(UInt8 TypeID);

	static TESDataHandler**							Singleton;

	static bool										PluginLoadSaveInProgress;			// managed by various hooks
};
STATIC_ASSERT(sizeof(TESDataHandler) == 0x640);

#define _DATAHANDLER			(*TESDataHandler::Singleton)

// C0
class TES
{
public:
	// members
	///*00*/ void**					vtbl;					// oddly, vtbl pointer is NULL in global TES object though c'tor initializes it...
	UInt32 unk[0xBC >> 2];

	virtual bool					VFn00(UInt32 arg1, UInt32 arg2, UInt32 arg3, UInt32 arg4, TESWorldSpace* worldspace = nullptr);		// calls worldspace->vtbl0x183, if worldspace == NULL, uses the currentWorldspace member


	static TES**					Singleton;
};
static_assert(sizeof(TES) == 0xC0);

#define _TES					(*TES::Singleton)
