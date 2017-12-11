#pragma once

#include "[Common]\AuxiliaryWindowsForm.h"
#include "ScriptTextEditorInterface.h"

namespace cse
{
	namespace intellisense
	{
		ref class IntelliSenseItemVariable;

		ref class IntelliSenseInterfaceModel : public IIntelliSenseInterfaceModel
		{
			textEditors::IScriptTextEditor^						ParentEditor;
			IIntelliSenseInterfaceView^							BoundParent;

			List<IntelliSenseItem^>^							LocalVariables;
			List<IntelliSenseItem^>^							EnumeratedItems;
			bool												CallingObjectIsRef;
			Script^												RemoteScript;
			IIntelliSenseInterfaceModel::Operation				LastOperation;

			property bool										AutomaticallyPopup;
			property UInt32										PopupThresholdLength;
			property bool										UseSubstringFiltering;
			property bool										InsertSuggestionOnEnterKey;

			property bool										OverrideThresholdCheck;
			property bool										Enabled;

			void												ScriptEditorPreferences_Saved(Object^ Sender, EventArgs^ E);
			void												ParentEditor_KeyDown(Object^ Sender, textEditors::IntelliSenseKeyDownEventArgs^ E);
			void												ParentEditor_KeyUp(Object^ Sender, KeyEventArgs^ E);
			void												ParentEditor_ShowInterface(Object^ Sender, textEditors::IntelliSenseShowEventArgs^ E);
			void												ParentEditor_HideInterface(Object^ Sender, textEditors::IntelliSenseHideEventArgs^ E);
			void												ParentEditor_RelocateInterface(Object^ Sender, textEditors::IntelliSensePositionEventArgs^ E);
			void												ParentEditor_BackgroundAnalysisComplete(Object^ Sender, EventArgs^ E);
			void												BoundParent_ItemSelected(Object^ Sender, EventArgs^ E);

			EventHandler^										ScriptEditorPreferencesSavedHandler;
			textEditors::IntelliSenseKeyDownEventHandler^		ParentEditorKeyDown;
			textEditors::IntelliSenseKeyUpEventHandler^			ParentEditorKeyUp;
			textEditors::IntelliSenseShowEventHandler^			ParentEditorShowInterface;
			textEditors::IntelliSenseHideEventHandler^			ParentEditorHideInterface;
			textEditors::IntelliSensePositionEventHandler^		ParentEditorRelocateInterface;
			EventHandler^										BoundParentItemSelectedHandler;
			EventHandler^										ParentEditorBGAnalysisCompleteHandler;

			property bool Bound
			{
				bool get() { return BoundParent != nullptr; }
			}

			void						Reset();
			void						EnumerateItem(IntelliSenseItem^ Item);
			void						PickSelection();
			void						PopulateDataStore(IIntelliSenseInterfaceModel::Operation O, bool IgnoreFilter);
			bool						GetTriggered(Keys E);

			IntelliSenseItemVariable^	GetLocalVar(String^ Identifier);
			void						UpdateLocalVars(obScriptParsing::AnalysisData^ Data);
		public:
			IntelliSenseInterfaceModel(textEditors::IScriptTextEditor^ Parent);
			~IntelliSenseInterfaceModel();

#pragma region Interfaces
			virtual property List<IntelliSenseItem^>^ DataStore
			{
				virtual List<IntelliSenseItem^>^ get() { return EnumeratedItems; }
				virtual void set(List<IntelliSenseItem^>^ e) {}
			}

			virtual void				Bind(IIntelliSenseInterfaceView^ To);
			virtual void				Unbind();
#pragma endregion
		};

		ref class IntelliSenseInterfaceView : public IIntelliSenseInterfaceView
		{
			IIntelliSenseInterfaceModel^					AttachedModel;

			NonActivatingImmovableAnimatedForm^				Form;
			BrightIdeasSoftware::ObjectListView^			ListView;
			ToolTip^										Popup;

			property UInt32									MaximumVisibleItemCount;
			property bool									PreventActivation
			{
				virtual bool get() sealed { return Form->PreventActivation; }
				virtual void set(bool value) sealed { Form->PreventActivation = value; }
			}

			property bool Bound
			{
				bool get() { return AttachedModel != nullptr; }
			}

			void											ScriptEditorPreferences_Saved(Object^ Sender, EventArgs^ E);
			void											ListView_SelectionChanged(Object^ Sender, EventArgs^ E);
			void											ListView_ItemActivate(Object^ Sender, EventArgs^ E);
			void											ListView_KeyDown(Object^ Sender, KeyEventArgs^ E);
			void											ListView_KeyUp(Object^ Sender, KeyEventArgs^ E);

			static Object^									ListViewAspectGetter(Object^ RowObject);
			static Object^									ListViewImageGetter(Object^ RowObject);

			EventHandler^									ScriptEditorPreferencesSavedHandler;
			EventHandler^									ListViewSelectionChangedHandler;
			EventHandler^									ListViewItemActivateHandler;
			KeyEventHandler^								ListViewKeyDownHandler;
			KeyEventHandler^								ListViewKeyUpHandler;

			void											DisplayToolTip(String^ Title,
																		   String^ Message,
																		   Point Location,
																		   IntPtr ParentHandle,
																		   UInt32 Duration);
			void											HideToolTip();

			delegate void									UIInvokeDelegate_FormShow(NonActivatingImmovableAnimatedForm^, Point, IntPtr);
			delegate void									UIInvokeDelegate_FormSetSize(NonActivatingImmovableAnimatedForm^, Size);
			delegate void									UIInvokeDelegate_FormHide(NonActivatingImmovableAnimatedForm^);

			static void										UIInvoke_FormShow(NonActivatingImmovableAnimatedForm^ ToInvoke, Point Location, IntPtr Parent);
			static void										UIInvoke_FormSetSize(NonActivatingImmovableAnimatedForm^ ToInvoke, Size ToSet);
			static void										UIInvoke_FormHide(NonActivatingImmovableAnimatedForm^ ToInvoke);

			static const float								DimmedOpacity = 0.1f;
		public:
			IntelliSenseInterfaceView();
			~IntelliSenseInterfaceView();

#pragma region Interfaces
			virtual event EventHandler^		ItemSelected;

			property bool Visible
			{
				virtual bool get() { return Form->Visible; }
				virtual void set(bool e) {}
			}
			property IntelliSenseItem^ Selection
			{
				virtual IntelliSenseItem^ get() { return (IntelliSenseItem^)ListView->SelectedObject; }
				virtual void set(IntelliSenseItem^ e) {}
			}

			virtual void				Bind(IIntelliSenseInterfaceModel^ To);
			virtual void				Unbind();

			virtual void				ChangeSelection(IIntelliSenseInterfaceView::MoveDirection Direction);
			virtual void				DimOpacity();
			virtual void				ResetOpacity();

			virtual void				Update();
			virtual void				Show(Point Location, IntPtr Parent);
			virtual void				Hide();
#pragma endregion
		};
	}
}
