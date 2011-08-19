#pragma once

#include "[Common]\NativeWrapper.h"

public ref class BatchEditor
{
protected:
	Form^								BatchEditBox;
		Button^								ApplyButton;
		Button^								CancelButton;

	virtual void 						ApplyButton_Click(Object^ Sender, EventArgs^ E);
	virtual void 						CancelButton_Click(Object^ Sender, EventArgs^ E);

	virtual void						Cleanup() = 0;
	virtual bool						ScrubData() = 0;
	virtual void						Close(DialogResult Result);
};

public ref class RefBatchEditor : public BatchEditor
{
protected:
	static RefBatchEditor^				Singleton = nullptr;

	RefBatchEditor();

	ListView^						ObjectList;
		ColumnHeader^					ObjectList_CHEditorID;
		ColumnHeader^					ObjectList_CHFormID;
		ColumnHeader^					ObjectList_CHType;

	TabControl^						DataBox;
		TabPage^						DataBox_TB3DData;
			GroupBox^						PositionBox;
				Label^							LabelZ;
				Label^							LabelY;
				Label^							LabelX;
				TextBox^						PosZ;
				TextBox^						PosY;
				TextBox^						PosX;

				CheckBox^						UsePosX;
				CheckBox^						UsePosZ;
				CheckBox^						UsePosY;
			GroupBox^						RotationBox;
				Label^							LabelZEx;
				TextBox^						RotX;
				Label^							LabelYEx;
				TextBox^						RotY;
				Label^							LabelXEx;
				TextBox^						RotZ;

				CheckBox^						UseRotX;
				CheckBox^						UseRotZ;
				CheckBox^						UseRotY;

			Label^							LabelScale;
			TextBox^						Scale;
			CheckBox^						UseScale;
		TabPage^						DataBox_TBFlags;
			CheckBox^						FlagVWD;
			CheckBox^						FlagDisabled;
			CheckBox^						FlagPersistent;

			CheckBox^						UsePersistent;
			CheckBox^						UseDisabled;
			CheckBox^						UseVWD;
		TabPage^						DataBox_TBEnableParent;
			CheckBox^						UseEnableParent;

			Button^							SetParent;
			CheckBox^						OppositeState;
		TabPage^						DataBox_TBOwnership;
			CheckBox^						UseOwnership;

			Label^							LabelFaction;
			Label^							LabelNPC;
			ComboBox^						NPCList;
			Label^							LabelGlobal;
			ComboBox^						FactionList;
			Label^							LabelRank;
			ComboBox^						GlobalList;
			TextBox^						Rank;
			RadioButton^					UseNPCOwner;
			RadioButton^					UseFactionOwner;
		TabPage^						DataBox_TBExtra;
			Label^							LabelCount;
			TextBox^						Count;
			Label^							LabelHealth;
			TextBox^						Health;
			Label^							LabelCharge;
			TextBox^						Charge;
			Label^							LabelTimeLeft;
			TextBox^						TimeLeft;
			ComboBox^						SoulLevel;
			Label^							LabelSoul;

			CheckBox^						UseCount;
			CheckBox^						UseHealth;
			CheckBox^						UseTimeLeft;
			CheckBox^						UseSoulLevel;
			CheckBox^						UseCharge;

	ComponentDLLInterface::BatchRefData*	BatchData;

	void								ObjectList_ColumnClick(Object^ Sender, ColumnClickEventArgs^ E);
	void								SetParent_Click(Object^ Sender, EventArgs^ E);

	virtual void						Cleanup() override;
	void								PopulateObjectList(ComponentDLLInterface::BatchRefData* Data);
	void								PopulateFormLists();
	virtual bool						ScrubData() override;

	float								GetFloatFromString(String^ Text);
	int									GetIntFromString(String^ Text);
	UInt32								GetUIntFromString(String^ Text);
	void								SanitizeControls(Control^ Container);

	static array<String^>^								TypeIdentifier =			// uses TESForm::typeID as its index
															{
																"None",
																"TES4",
																"Group",
																"GMST",
																"Global",
																"Class",
																"Faction",
																"Hair",
																"Eyes",
																"Race",
																"Sound",
																"Skill",
																"Effect",
																"Script",
																"LandTexture",
																"Enchantment",
																"Spell",
																"BirthSign",
																"Activator",
																"Apparatus",
																"Armor",
																"Book",
																"Clothing",
																"Container",
																"Door",
																"Ingredient",
																"Light",
																"MiscItem",
																"Static",
																"Grass",
																"Tree",
																"Flora",
																"Furniture",
																"Weapon",
																"Ammo",
																"NPC",
																"Creature",
																"LeveledCreature",
																"SoulGem",
																"Key",
																"AlchemyItem",
																"SubSpace",
																"SigilStone",
																"LeveledItem",
																"SNDG",
																"Weather",
																"Climate",
																"Region",
																"Cell",
																"Reference",
																"Reference",			// ACHR
																"Reference",			// ACRE
																"PathGrid",
																"World Space",
																"Land",
																"TLOD",
																"Road",
																"Dialog",
																"Dialog Info",
																"Quest",
																"Idle",
																"AI Package",
																"CombatStyle",
																"LoadScreen",
																"LeveledSpell",
																"AnimObject",
																"WaterType",
																"EffectShader",
																"TOFT"
															};
public:
	static RefBatchEditor^%				GetSingleton();

	bool								InitializeBatchEditor(ComponentDLLInterface::BatchRefData* Data);
};

#define REFBE							RefBatchEditor::GetSingleton()