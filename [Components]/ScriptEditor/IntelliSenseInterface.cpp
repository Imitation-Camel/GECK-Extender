#include "IntelliSenseItem.h"
#include "IntelliSenseInterface.h"
#include "IntelliSenseDatabase.h"
#include "Globals.h"
#include "ScriptEditorPreferences.h"

namespace cse
{
	namespace intellisense
	{
		IntelliSenseInterfaceModel::IntelliSenseInterfaceModel(textEditors::IScriptTextEditor^ Parent)
		{
			Debug::Assert(Parent != nullptr);

			ParentEditor = Parent;
			BoundParent = nullptr;

			LocalVariables = gcnew List < IntelliSenseItem^ > ;
			EnumeratedItems = gcnew List < IntelliSenseItem^ > ;
			CallingObjectIsRef = false;
			RemoteScript = Script::NullScript;
			LastOperation = IIntelliSenseInterfaceModel::Operation::Default;

			AutomaticallyPopup = PREFERENCES->FetchSettingAsInt("AutoSuggest", "IntelliSense") != 0;
			PopupThresholdLength = PREFERENCES->FetchSettingAsInt("ThresholdLength", "IntelliSense");
			UseSubstringFiltering = PREFERENCES->FetchSettingAsInt("SubstringSearch", "IntelliSense") != 0;
			InsertSuggestionOnEnterKey = PREFERENCES->FetchSettingAsInt("InsertOnEnter", "IntelliSense") != 0;

			OverrideThresholdCheck = false;
			Enabled = true;

			ScriptEditorPreferencesSavedHandler = gcnew EventHandler(this, &IntelliSenseInterfaceModel::ScriptEditorPreferences_Saved);
			ParentEditorKeyDown = gcnew textEditors::IntelliSenseKeyDownEventHandler(this, &IntelliSenseInterfaceModel::ParentEditor_KeyDown);
			ParentEditorKeyUp = gcnew textEditors::IntelliSenseKeyUpEventHandler(this, &IntelliSenseInterfaceModel::ParentEditor_KeyUp);
			ParentEditorShowInterface = gcnew textEditors::IntelliSenseShowEventHandler(this, &IntelliSenseInterfaceModel::ParentEditor_ShowInterface);
			ParentEditorHideInterface = gcnew textEditors::IntelliSenseHideEventHandler(this, &IntelliSenseInterfaceModel::ParentEditor_HideInterface);
			ParentEditorRelocateInterface = gcnew textEditors::IntelliSensePositionEventHandler(this, &IntelliSenseInterfaceModel::ParentEditor_RelocateInterface);
			ParentEditorBGAnalysisCompleteHandler = gcnew EventHandler(this, &IntelliSenseInterfaceModel::ParentEditor_BackgroundAnalysisComplete);
			BoundParentItemSelectedHandler = gcnew EventHandler(this, &IntelliSenseInterfaceModel::BoundParent_ItemSelected);

			PREFERENCES->PreferencesSaved += ScriptEditorPreferencesSavedHandler;
			ParentEditor->IntelliSenseKeyDown += ParentEditorKeyDown;
			ParentEditor->IntelliSenseKeyUp += ParentEditorKeyUp;
			ParentEditor->IntelliSenseShow += ParentEditorShowInterface;
			ParentEditor->IntelliSenseHide += ParentEditorHideInterface;
			ParentEditor->IntelliSenseRelocate += ParentEditorRelocateInterface;
			ParentEditor->BackgroundAnalysisComplete += ParentEditorBGAnalysisCompleteHandler;
		}

		IntelliSenseInterfaceModel::~IntelliSenseInterfaceModel()
		{
			Unbind();
			Reset();

			PREFERENCES->PreferencesSaved -= ScriptEditorPreferencesSavedHandler;
			ParentEditor->IntelliSenseKeyDown -= ParentEditorKeyDown;
			ParentEditor->IntelliSenseKeyUp -= ParentEditorKeyUp;
			ParentEditor->IntelliSenseShow -= ParentEditorShowInterface;
			ParentEditor->IntelliSenseHide -= ParentEditorHideInterface;
			ParentEditor->IntelliSenseRelocate -= ParentEditorRelocateInterface;
			ParentEditor->BackgroundAnalysisComplete -= ParentEditorBGAnalysisCompleteHandler;

			SAFEDELETE_CLR(ScriptEditorPreferencesSavedHandler);
			SAFEDELETE_CLR(ParentEditorKeyDown);
			SAFEDELETE_CLR(ParentEditorKeyUp);
			SAFEDELETE_CLR(ParentEditorShowInterface);
			SAFEDELETE_CLR(ParentEditorHideInterface);
			SAFEDELETE_CLR(ParentEditorRelocateInterface);
			SAFEDELETE_CLR(ParentEditorBGAnalysisCompleteHandler);

			RemoteScript = nullptr;

			LocalVariables->Clear();

			ParentEditor = nullptr;
		}

		void IntelliSenseInterfaceModel::ScriptEditorPreferences_Saved(Object^ Sender, EventArgs^ E)
		{
			Reset();
			if (Bound)
				BoundParent->Hide();

			LastOperation = IIntelliSenseInterfaceModel::Operation::Default;
			OverrideThresholdCheck = false;
			Enabled = true;

			AutomaticallyPopup = PREFERENCES->FetchSettingAsInt("AutoSuggest", "IntelliSense") != 0;
			PopupThresholdLength = PREFERENCES->FetchSettingAsInt("ThresholdLength", "IntelliSense");
			UseSubstringFiltering = PREFERENCES->FetchSettingAsInt("SubstringSearch", "IntelliSense") != 0;
			InsertSuggestionOnEnterKey = PREFERENCES->FetchSettingAsInt("InsertOnEnter", "IntelliSense") != 0;
		}

		void IntelliSenseInterfaceModel::ParentEditor_KeyDown(Object^ Sender, textEditors::IntelliSenseKeyDownEventArgs^ E)
		{
			Debug::Assert(Bound == true);
			Enabled = true;

			if (GetTriggered(E->KeyCode) && E->AllowForDisplay && E->Control == false && E->Shift == false && E->Alt == false)
			{
				switch (E->KeyCode)
				{
				case Keys::OemPeriod:
					{
						if (AutomaticallyPopup)
						{
							E->DisplayOperation = IIntelliSenseInterfaceModel::Operation::Dot;
							E->Display = true;
							E->PreventNextTextChangeEvent = true;
						}
						break;
					}
				case Keys::Space:
					{
						if (AutomaticallyPopup)
						{
							String^ Token = ParentEditor->GetTokenAtCaretPos()->Replace("\n", "");

							if (ScriptParser::GetScriptTokenType(Token) == obScriptParsing::ScriptTokenType::Call)
							{
								E->DisplayOperation = IIntelliSenseInterfaceModel::Operation::Call;
								E->Display = true;
								E->PreventNextTextChangeEvent = true;
							}
							else if (ScriptParser::GetScriptTokenType(Token) == obScriptParsing::ScriptTokenType::Set ||
									 ScriptParser::GetScriptTokenType(Token) == obScriptParsing::ScriptTokenType::Let)
							{
								E->DisplayOperation = IIntelliSenseInterfaceModel::Operation::Assign;
								E->Display = true;
								E->PreventNextTextChangeEvent = true;
							}
							else
								LastOperation = IIntelliSenseInterfaceModel::Operation::Default;
						}

						break;
					}
				case Keys::Oemtilde:
					E->DisplayOperation = IIntelliSenseInterfaceModel::Operation::Snippet;
					E->Display = true;
					E->PreventNextTextChangeEvent = true;

					break;
				default:
					if (AutomaticallyPopup)
						LastOperation = IIntelliSenseInterfaceModel::Operation::Default;

					break;
				}

				if (AutomaticallyPopup)
					OverrideThresholdCheck = false;
			}

			switch (E->KeyCode)
			{
			case Keys::Enter:
				if (E->Control)
				{
					OverrideThresholdCheck = true;
					if (BoundParent->Visible == false)
					{
						E->DisplayOperation = IIntelliSenseInterfaceModel::Operation::Default;
						E->Display = true;
						E->PreventNextTextChangeEvent = true;
					}

					E->Handled = true;
				}
				else if (BoundParent->Visible)
				{
					if (InsertSuggestionOnEnterKey)
						PickSelection();
					LastOperation = IIntelliSenseInterfaceModel::Operation::Default;
					OverrideThresholdCheck = false;
					BoundParent->Hide();

					if (InsertSuggestionOnEnterKey)
					{
						E->PreventNextTextChangeEvent = true;
						E->Handled = true;
					}
				}

				break;
			case Keys::Escape:
				if (BoundParent->Visible)
				{
					Enabled = false;
					LastOperation = IIntelliSenseInterfaceModel::Operation::Default;
					OverrideThresholdCheck = false;
					BoundParent->Hide();

					E->Handled = true;
				}

				break;
			case Keys::Tab:
				if (BoundParent->Visible)
				{
					PickSelection();
					LastOperation = IIntelliSenseInterfaceModel::Operation::Default;
					OverrideThresholdCheck = false;
					BoundParent->Hide();

					E->PreventNextTextChangeEvent = true;
					E->Handled = true;
				}

				break;
			case Keys::Up:
				if (BoundParent->Visible)
				{
					BoundParent->ChangeSelection(IIntelliSenseInterfaceView::MoveDirection::Up);

					E->Handled = true;
				}

				break;
			case Keys::Down:
				if (BoundParent->Visible)
				{
					BoundParent->ChangeSelection(IIntelliSenseInterfaceView::MoveDirection::Down);

					E->Handled = true;
				}

				break;
			case Keys::PageUp:
			case Keys::PageDown:
				if (BoundParent->Visible)
					E->Handled = true;

				break;
			case Keys::LControlKey:
			case Keys::RControlKey:
				if (BoundParent->Visible)
					BoundParent->DimOpacity();

				break;
			}
		}

		void IntelliSenseInterfaceModel::ParentEditor_KeyUp(Object^ Sender, KeyEventArgs^ E)
		{
			switch (E->KeyCode)
			{
			case Keys::LControlKey:
			case Keys::RControlKey:
				if (BoundParent->Visible)
					BoundParent->ResetOpacity();

				break;
			}
		}

		void IntelliSenseInterfaceModel::ParentEditor_ShowInterface(Object^ Sender, textEditors::IntelliSenseShowEventArgs^ E)
		{
			Debug::Assert(Bound == true);
			if (Enabled == false)
				return;

			Reset();

			if (E->UseActive)
				PopulateDataStore(LastOperation, false);
			else
				PopulateDataStore(E->NewOperation, true);

			BoundParent->Update();

			if (EnumeratedItems->Count)
			{
				BoundParent->Show(E->Location, E->WindowHandle);
				ParentEditor->FocusTextArea();
			}
			else if (BoundParent->Visible)
				BoundParent->Hide();
		}

		void IntelliSenseInterfaceModel::ParentEditor_HideInterface(Object^ Sender, textEditors::IntelliSenseHideEventArgs^ E)
		{
			Debug::Assert(Bound == true);

			if (BoundParent->Visible)
			{
				if (E->Reset)
				{
					LastOperation = IIntelliSenseInterfaceModel::Operation::Default;
					OverrideThresholdCheck = false;
					Enabled = true;
				}

				BoundParent->Hide();
			}
		}

		void IntelliSenseInterfaceModel::ParentEditor_RelocateInterface(Object^ Sender, textEditors::IntelliSensePositionEventArgs^ E)
		{
			Debug::Assert(Bound == true);

			if (BoundParent->Visible)
			{
				BoundParent->Show(E->Location, E->WindowHandle);
				ParentEditor->FocusTextArea();
			}
		}

		void IntelliSenseInterfaceModel::ParentEditor_BackgroundAnalysisComplete(Object^ Sender, EventArgs^ E)
		{
			UpdateLocalVars(ParentEditor->GetSemanticAnalysisCache(false, false));
		}

		void IntelliSenseInterfaceModel::BoundParent_ItemSelected(Object^ Sender, EventArgs^ E)
		{
			PickSelection();
		}

		void IntelliSenseInterfaceModel::Reset()
		{
			EnumeratedItems->Clear();
		}

		void IntelliSenseInterfaceModel::EnumerateItem(IntelliSenseItem^ Item)
		{
			EnumeratedItems->Add(Item);
		}

		void IntelliSenseInterfaceModel::PickSelection()
		{
			Debug::Assert(Bound == true);

			if (BoundParent->Selection)
			{
				IntelliSenseItem^ Item = (IntelliSenseItem^)BoundParent->Selection;

				Enabled = false;
				Item->Insert(ParentEditor);
				Enabled = true;
			}

			ParentEditor->FocusTextArea();
			Reset();
		}

		void IntelliSenseInterfaceModel::PopulateDataStore(IIntelliSenseInterfaceModel::Operation O, bool IgnoreFilter)
		{
			if (Enabled == false && OverrideThresholdCheck == false)
				return;

			String^ CurrentToken = ParentEditor->GetTokenAtCaretPos();

			switch (O)
			{
			case IIntelliSenseInterfaceModel::Operation::Default:
				if (OverrideThresholdCheck || (AutomaticallyPopup && CurrentToken->Length >= PopupThresholdLength))
				{
					for each (IntelliSenseItem^ Itr in LocalVariables)
					{
						if (Itr->GetShouldEnumerate(CurrentToken, UseSubstringFiltering))
							EnumerateItem(Itr);
					}

					for each (IntelliSenseItem^ Itr in ISDB->ItemRegistry)
					{
						if (Itr->GetItemType() == IntelliSenseItem::IntelliSenseItemType::Command ||
							Itr->GetItemType() == IntelliSenseItem::IntelliSenseItemType::Quest ||
							Itr->GetItemType() == IntelliSenseItem::IntelliSenseItemType::GlobalVar ||
							Itr->GetItemType() == IntelliSenseItem::IntelliSenseItemType::GMST ||
							Itr->GetItemType() == IntelliSenseItem::IntelliSenseItemType::Form ||
							Itr->GetItemType() == IntelliSenseItem::IntelliSenseItemType::UserFunction)
						{
							if (Itr->GetShouldEnumerate(CurrentToken, UseSubstringFiltering))
								EnumerateItem(Itr);
						}
					}
				}

				break;
			case IIntelliSenseInterfaceModel::Operation::Call:
				for each (IntelliSenseItem^ Itr in ISDB->ItemRegistry)
				{
					if (Itr->GetItemType() == IntelliSenseItem::IntelliSenseItemType::UserFunction)
					{
						if (IgnoreFilter || Itr->GetShouldEnumerate(CurrentToken, UseSubstringFiltering))
							EnumerateItem(Itr);
					}
				}

				break;
			case IIntelliSenseInterfaceModel::Operation::Dot:
				if (IgnoreFilter)
				{
					IntelliSenseItemVariable^ RefVar = GetLocalVar(CurrentToken);
					if (RefVar && RefVar->GetDataType() == obScriptParsing::Variable::DataType::Ref)
						CallingObjectIsRef = true;
					else if (ScriptParser::GetScriptTokenType(CurrentToken) == obScriptParsing::ScriptTokenType::Player)
						CallingObjectIsRef = true;
					else
					{
						CString CStr(CurrentToken);						// extract = calling ref
						componentDLLInterface::ScriptData* Data = nativeWrapper::g_CSEInterfaceTable->EditorAPI.LookupScriptableFormByEditorID(CStr.c_str());
						if (Data && Data->IsValid() == false)
						{
							LastOperation = IIntelliSenseInterfaceModel::Operation::Default;
							break;
						}
						else if (Data)
						{
							RemoteScript = ISDB->CacheRemoteScript(gcnew String(Data->ParentID), gcnew String(Data->Text));
							CallingObjectIsRef = nativeWrapper::g_CSEInterfaceTable->EditorAPI.GetIsFormReference(CStr.c_str());
						}
						else
						{
							CallingObjectIsRef = false;
							RemoteScript = Script::NullScript;
						}
						nativeWrapper::g_CSEInterfaceTable->DeleteInterOpData(Data, false);
					}
				}

				for (Script::VarListT::Enumerator^ RemoteVarItr = RemoteScript->GetVariableListEnumerator(); RemoteVarItr->MoveNext();)
				{
					if (RemoteVarItr->Current->GetItemType() == IntelliSenseItem::IntelliSenseItemType::RemoteVar)
					{
						if (IgnoreFilter || RemoteVarItr->Current->GetShouldEnumerate(CurrentToken, UseSubstringFiltering))
							EnumerateItem(RemoteVarItr->Current);
					}
				}

				for each (IntelliSenseItem^ Itr in ISDB->ItemRegistry)
				{
					if (Itr->GetItemType() == IntelliSenseItem::IntelliSenseItemType::Command && CallingObjectIsRef)
					{
						if (IgnoreFilter || Itr->GetShouldEnumerate(CurrentToken, UseSubstringFiltering))
							EnumerateItem(Itr);
					}
				}

				break;
			case IIntelliSenseInterfaceModel::Operation::Assign:
				for each (IntelliSenseItem^ Itr in LocalVariables)
				{
					if (IgnoreFilter || Itr->GetShouldEnumerate(CurrentToken, UseSubstringFiltering))
						EnumerateItem(Itr);
				}

				for each (IntelliSenseItem^ Itr in ISDB->ItemRegistry)
				{
					if (Itr->GetItemType() == IntelliSenseItem::IntelliSenseItemType::Quest ||
						Itr->GetItemType() == IntelliSenseItem::IntelliSenseItemType::GlobalVar)
					{
						if (IgnoreFilter || Itr->GetShouldEnumerate(CurrentToken, UseSubstringFiltering))
							EnumerateItem(Itr);
					}
				}

				break;
			case IIntelliSenseInterfaceModel::Operation::Snippet:
				{
					if (CurrentToken->Length > 1)
						CurrentToken = CurrentToken->Remove(0, 1);		// remove leading tilde character

					for each (IntelliSenseItem^ Itr in ISDB->ItemRegistry)
					{
						if (Itr->GetItemType() == IntelliSenseItem::IntelliSenseItemType::Snippet)
						{
							if (IgnoreFilter || CurrentToken == "`" || Itr->GetShouldEnumerate(CurrentToken, UseSubstringFiltering))
								EnumerateItem(Itr);
						}
					}
				}

				break;
			}

			if (EnumeratedItems->Count == 1)
			{
				IntelliSenseItem^ Item = EnumeratedItems->ToArray()[0];
				if (String::Compare(Item->GetIdentifier(), CurrentToken, true) == 0)
					EnumeratedItems->Clear();			// do not show when enumerable == current token
			}

			LastOperation = O;
		}

		IntelliSenseItemVariable^ IntelliSenseInterfaceModel::GetLocalVar(String^ Identifier)
		{
			for each (IntelliSenseItem^ Itr in LocalVariables)
			{
				if (Identifier->Length == Itr->GetIdentifier()->Length &&
					!String::Compare(Itr->GetIdentifier(), Identifier, true))
				{
					return (IntelliSenseItemVariable^)Itr;
				}
			}

			return nullptr;
		}

		void IntelliSenseInterfaceModel::Bind(IIntelliSenseInterfaceView^ To)
		{
			Debug::Assert(Bound == false);

			BoundParent = To;
			BoundParent->Bind(this);

			BoundParent->ItemSelected += BoundParentItemSelectedHandler;
		}

		void IntelliSenseInterfaceModel::Unbind()
		{
			if (Bound)
			{
				BoundParent->ItemSelected -= BoundParentItemSelectedHandler;
				BoundParent->Hide();

				BoundParent->Unbind();
				BoundParent = nullptr;
			}
		}

		bool IntelliSenseInterfaceModel::GetTriggered(Keys E)
		{
			switch (E)
			{
			case Keys::Oemtilde:
			case Keys::OemPeriod:
			case Keys::Oemcomma:
			case Keys::Space:
			case Keys::OemOpenBrackets:
			case Keys::OemCloseBrackets:
			case Keys::Tab:
			case Keys::Enter:
				return true;
			default:
				return false;
			}
		}

		void IntelliSenseInterfaceModel::UpdateLocalVars(obScriptParsing::AnalysisData^ Data)
		{
			if (Data)
			{
				LocalVariables->Clear();

				for each (obScriptParsing::Variable^ Itr in Data->Variables)
				{
					IntelliSenseItemVariable^ NewVar = gcnew IntelliSenseItemVariable(Itr->Name, Itr->Comment,
																					  Itr->Type, IntelliSenseItem::IntelliSenseItemType::LocalVar);
					LocalVariables->Add(NewVar);
				}
			}
		}

		IntelliSenseInterfaceView::IntelliSenseInterfaceView()
		{
			AttachedModel = nullptr;

			Form = gcnew NonActivatingImmovableAnimatedForm();
			ListView = gcnew BrightIdeasSoftware::FastObjectListView;

			ListViewSelectionChangedHandler = gcnew EventHandler(this, &IntelliSenseInterfaceView::ListView_SelectionChanged);
			ListViewKeyDownHandler = gcnew KeyEventHandler(this, &IntelliSenseInterfaceView::ListView_KeyDown);
			ListViewKeyUpHandler = gcnew KeyEventHandler(this, &IntelliSenseInterfaceView::ListView_KeyUp);
			ListViewItemActivateHandler = gcnew EventHandler(this, &IntelliSenseInterfaceView::ListView_ItemActivate);
			ScriptEditorPreferencesSavedHandler = gcnew EventHandler(this, &IntelliSenseInterfaceView::ScriptEditorPreferences_Saved);

			ListView->KeyDown += ListViewKeyDownHandler;
			ListView->KeyUp += ListViewKeyUpHandler;
			ListView->ItemActivate += ListViewItemActivateHandler;
			ListView->SelectedIndexChanged += ListViewSelectionChangedHandler;
			PREFERENCES->PreferencesSaved += ScriptEditorPreferencesSavedHandler;

			Form->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			Form->AutoScaleMode = AutoScaleMode::Font;
			Form->FormBorderStyle = FormBorderStyle::SizableToolWindow;
			Form->ShowInTaskbar = false;
			Form->ShowIcon = false;
			Form->ControlBox = false;
			Form->Controls->Add(ListView);

			ListView->View = View::Details;
			ListView->Dock = DockStyle::Fill;
			ListView->MultiSelect = false;
			ListView->SmallImageList = gcnew ImageList();
			ListView->SmallImageList->Images->Add(Globals::ScriptEditorImageResourceManager->CreateImage("IntelliSenseItemEmpty"));
			ListView->SmallImageList->Images->Add(Globals::ScriptEditorImageResourceManager->CreateImage("IntelliSenseItemCommand"));
			ListView->SmallImageList->Images->Add(Globals::ScriptEditorImageResourceManager->CreateImage("IntelliSenseItemLocalVar"));
			ListView->SmallImageList->Images->Add(Globals::ScriptEditorImageResourceManager->CreateImage("IntelliSenseItemRemoteVar"));
			ListView->SmallImageList->Images->Add(Globals::ScriptEditorImageResourceManager->CreateImage("IntelliSenseItemUDF"));
			ListView->SmallImageList->Images->Add(Globals::ScriptEditorImageResourceManager->CreateImage("IntelliSenseItemQuest"));
			ListView->SmallImageList->Images->Add(Globals::ScriptEditorImageResourceManager->CreateImage("IntelliSenseItemGlobalVar"));
			ListView->SmallImageList->Images->Add(Globals::ScriptEditorImageResourceManager->CreateImage("IntelliSenseItemGMST"));
			ListView->SmallImageList->Images->Add(Globals::ScriptEditorImageResourceManager->CreateImage("IntelliSenseItemForm"));
			ListView->SmallImageList->Images->Add(Globals::ScriptEditorImageResourceManager->CreateImage("IntelliSenseItemSnippet"));
			ListView->Location = Point(0, 0);
			ListView->LabelEdit = false;
			ListView->CheckBoxes = false;
			ListView->FullRowSelect = true;
			ListView->GridLines = false;
			ListView->HeaderStyle = ColumnHeaderStyle::None;
			ListView->HideSelection = false;

			BrightIdeasSoftware::OLVColumn^ Column = gcnew BrightIdeasSoftware::OLVColumn;
			Column->AspectGetter = gcnew BrightIdeasSoftware::AspectGetterDelegate(&IntelliSenseInterfaceView::ListViewAspectGetter);
			Column->ImageGetter = gcnew BrightIdeasSoftware::ImageGetterDelegate(&IntelliSenseInterfaceView::ListViewImageGetter);
			Column->Text = "IntelliSense Item";
			Column->Width = 203;
			ListView->AllColumns->Add(Column);
			ListView->Columns->Add(Column);

			Popup = gcnew ToolTip;
			Popup->AutoPopDelay = 500;
			Popup->InitialDelay = 500;
			Popup->ReshowDelay = 0;
			Popup->ToolTipIcon = ToolTipIcon::None;
			Popup->Tag = nullptr;

			MaximumVisibleItemCount = PREFERENCES->FetchSettingAsInt("MaxVisibleItems", "IntelliSense");
			PreventActivation = PREFERENCES->FetchSettingAsInt("NoFocusUI", "IntelliSense") == 0;

			Form->SetSize(Size(0, 0));
			Form->ShowForm(Point(0, 0), Form->Handle, false);
			Form->Hide();
		}

		IntelliSenseInterfaceView::~IntelliSenseInterfaceView()
		{
			Debug::Assert(Bound == false);

			ListView->KeyDown -= ListViewKeyDownHandler;
			ListView->KeyUp -= ListViewKeyUpHandler;
			ListView->ItemActivate -= ListViewItemActivateHandler;
			ListView->SelectedIndexChanged -= ListViewSelectionChangedHandler;
			PREFERENCES->PreferencesSaved -= ScriptEditorPreferencesSavedHandler;

			SAFEDELETE_CLR(ListViewKeyDownHandler);
			SAFEDELETE_CLR(ListViewKeyUpHandler);
			SAFEDELETE_CLR(ListViewItemActivateHandler);
			SAFEDELETE_CLR(ListViewSelectionChangedHandler);
			SAFEDELETE_CLR(ScriptEditorPreferencesSavedHandler);

			for each (Image^ Itr in ListView->SmallImageList->Images)
				delete Itr;

			HideToolTip();
			Form->Close();

			delete Popup;
			delete Form;
			delete ListView;
		}

		void IntelliSenseInterfaceView::ScriptEditorPreferences_Saved(Object^ Sender, EventArgs^ E)
		{
			MaximumVisibleItemCount = PREFERENCES->FetchSettingAsInt("MaxVisibleItems", "IntelliSense");
			PreventActivation = PREFERENCES->FetchSettingAsInt("NoFocusUI", "IntelliSense") == 0;
		}

		void IntelliSenseInterfaceView::ListView_SelectionChanged(Object^ Sender, EventArgs^ E)
		{
			if (Form->Visible)
			{
				IntelliSenseItem^ Current = (IntelliSenseItem^)ListView->SelectedObject;
				if (Current)
				{
					DisplayToolTip(Current->GetItemTypeID(),
								   Current->Describe(),
								   Point(ListView->Size.Width + 17, 0),
								   Form->Handle,
								   10000);
				}
			}
		}

		void IntelliSenseInterfaceView::ListView_ItemActivate(Object^ Sender, EventArgs^ E)
		{
			Debug::Assert(Bound == true);

			ItemSelected(this, E);
			Hide();
		}

		void IntelliSenseInterfaceView::ListView_KeyDown(Object^ Sender, KeyEventArgs^ E)
		{
			Debug::Assert(Bound == true);

			switch (E->KeyCode)
			{
			case Keys::Escape:
				Hide();
				break;
			case Keys::Tab:
			case Keys::Enter:
				ItemSelected(this, E);
				Hide();
				break;
			case Keys::ControlKey:
				DimOpacity();
				break;
			}
		}

		void IntelliSenseInterfaceView::ListView_KeyUp(Object^ Sender, KeyEventArgs^ E)
		{
			Debug::Assert(Bound == true);

			switch (E->KeyCode)
			{
			case Keys::ControlKey:
				ResetOpacity();
				break;
			}
		}

		Object^ IntelliSenseInterfaceView::ListViewAspectGetter(Object^ RowObject)
		{
			if (RowObject)
			{
				IntelliSenseItem^ Item = (IntelliSenseItem^)RowObject;
				Debug::Assert(Item != nullptr);

				return Item->GetIdentifier();
			}
			else
				return nullptr;
		}

		Object^ IntelliSenseInterfaceView::ListViewImageGetter(Object^ RowObject)
		{
			if (RowObject)
			{
				IntelliSenseItem^ Item = (IntelliSenseItem^)RowObject;
				Debug::Assert(Item != nullptr);

				return (int)Item->GetItemType();
			}
			else
				return nullptr;
		}

		void IntelliSenseInterfaceView::DisplayToolTip(String^ Title, String^ Message, Point Location, IntPtr ParentHandle, UInt32 Duration)
		{
			Popup->Tag = ParentHandle;
			Popup->ToolTipTitle = Title;
			Popup->Show(Message, Control::FromHandle(ParentHandle), Location, Duration);
		}

		void IntelliSenseInterfaceView::HideToolTip()
		{
			if (Popup->Tag)
			{
				Popup->Hide(Control::FromHandle((IntPtr)Popup->Tag));
				Popup->Tag = nullptr;
			}
		}

		void IntelliSenseInterfaceView::Bind(IIntelliSenseInterfaceModel^ To)
		{
			Debug::Assert(Bound == false);

			AttachedModel = To;
		}

		void IntelliSenseInterfaceView::Unbind()
		{
			if (Bound)
				AttachedModel = nullptr;
		}

		void IntelliSenseInterfaceView::ChangeSelection(IIntelliSenseInterfaceView::MoveDirection Direction)
		{
			int SelectedIndex = ListView->SelectedIndex;
			if (SelectedIndex == -1)
				return;

			switch (Direction)
			{
			case IIntelliSenseInterfaceView::MoveDirection::Down:
				if (SelectedIndex < ListView->GetItemCount() - 1)
				{
					SelectedIndex++;
					ListView->SelectedIndex = SelectedIndex;
					ListView->TopItemIndex = SelectedIndex;
				}

				break;
			case IIntelliSenseInterfaceView::MoveDirection::Up:
				if (SelectedIndex > 0)
				{
					SelectedIndex--;
					ListView->SelectedIndex = SelectedIndex;
					ListView->TopItemIndex = SelectedIndex;
				}

				break;
			}
		}

		void IntelliSenseInterfaceView::DimOpacity()
		{
			Form->Opacity = DimmedOpacity;
		}

		void IntelliSenseInterfaceView::ResetOpacity()
		{
			Form->Opacity = 1.f;
		}

		void IntelliSenseInterfaceView::Update()
		{
			Debug::Assert(Bound == true);

			ListView->SetObjects(AttachedModel->DataStore);

			if (AttachedModel->DataStore->Count)
			{
				int ItemCount = AttachedModel->DataStore->Count;
				if (ItemCount > MaximumVisibleItemCount)
					ItemCount = MaximumVisibleItemCount;
				int ItemHeight = ListView->Items[0]->Bounds.Height;

				// mucking about with the CS' compatibility settings (setting it to Windows 7, for instance) yields a different result than when the setting's disabled
				Size DisplaySize = Size(240, (MaximumVisibleItemCount * ItemHeight + ItemHeight) - ((MaximumVisibleItemCount - ItemCount) * ItemHeight));

				array<Object^>^ Params = gcnew array < Object^ > { Form, DisplaySize };
				Form->BeginInvoke(gcnew UIInvokeDelegate_FormSetSize(&IntelliSenseInterfaceView::UIInvoke_FormSetSize), Params);

				ListView->SelectedIndex = 0;
			}
		}

		void IntelliSenseInterfaceView::Show(Point Location, IntPtr Parent)
		{
			array<Object^>^ Params = gcnew array < Object^ > { Form, Location, Parent };
			Form->BeginInvoke(gcnew UIInvokeDelegate_FormShow(&IntelliSenseInterfaceView::UIInvoke_FormShow), Params);
		}

		void IntelliSenseInterfaceView::Hide()
		{
			if (Form->Visible)
			{
				array<Object^>^ Params = gcnew array < Object^ > { Form };
				Form->BeginInvoke(gcnew UIInvokeDelegate_FormHide(&IntelliSenseInterfaceView::UIInvoke_FormHide), Params);

				HideToolTip();
			}
		}

		// HACK!
		// The SetSize call crashes consistently under certain conditions (which are yet to be decoded) due to an invalid window handle
		// methinks it has something to do with how the call is invoked (multiple levels of interop b'ween WinForms and WPF)
		// delegating it to the UI thread through BeginInvoke seems to help apparently but we still need to wrap it in an exception handler

		void IntelliSenseInterfaceView::UIInvoke_FormShow(NonActivatingImmovableAnimatedForm^ ToInvoke, Point Location, IntPtr Parent)
		{
			try {
				ToInvoke->ShowForm(Location, Parent, (ToInvoke->Visible == false));
			}
			catch (Exception^ E) {
#ifndef NDEBUG
				DebugPrint("IntelliSenseInterfaceView::UIInvoke_FormShow Exception! Message - " + E->Message);
				Debugger::Break();
#endif // !NDEBUG
			}
		}

		void IntelliSenseInterfaceView::UIInvoke_FormSetSize(NonActivatingImmovableAnimatedForm^ ToInvoke, Size ToSet)
		{
			try {
				ToInvoke->SetSize(ToSet);
			}
			catch (Exception^ E) {
#ifndef NDEBUG
				DebugPrint("IntelliSenseInterfaceView::UIInvoke_FormSetSize Exception! Message - " + E->Message);
				Debugger::Break();
#endif // !NDEBUG
			}
		}

		void IntelliSenseInterfaceView::UIInvoke_FormHide(NonActivatingImmovableAnimatedForm^ ToInvoke)
		{
			try {
				ToInvoke->HideForm(true);
			}
			catch (Exception^ E) {
#ifndef NDEBUG
				DebugPrint("IntelliSenseInterfaceView::UIInvoke_FormHide Exception! Message - " + E->Message);
				Debugger::Break();
#endif // !NDEBUG
			}
		}
	}
}