#pragma once

#include "WorkspaceModelInterface.h"
#include "WorkspaceViewInterface.h"
#include "BackgroundAnalysis.h"
#include "AvalonEditTextEditor.h"

#define NEWSCRIPTID					"New Script"
#define FIRSTRUNSCRIPTID			"New Workspace"

namespace cse
{
	namespace scriptEditor
	{
		ref class ConcreteWorkspaceModel;

		ref class ConcreteWorkspaceModelFactory : public IWorkspaceModelFactory
		{
			List<ConcreteWorkspaceModel^>^				Allocations;

			ConcreteWorkspaceModelFactory();
		public:
			~ConcreteWorkspaceModelFactory();

			property UInt32								Count
			{
				virtual UInt32 get() { return Allocations->Count; }
				virtual void set(UInt32 e) {}
			}

			static ConcreteWorkspaceModelFactory^		Instance = gcnew ConcreteWorkspaceModelFactory;

			void										Remove(ConcreteWorkspaceModel^ Allocation);

			// IWorkspaceModelFactory
			virtual IWorkspaceModel^				CreateModel(componentDLLInterface::ScriptData* Data);
			virtual IWorkspaceModelController^		CreateController();
		};

		ref class ConcreteWorkspaceModelController : public IWorkspaceModelController
		{
		public:
			virtual void					Bind(IWorkspaceModel^ Model, IWorkspaceView^ To);
			virtual void					Unbind(IWorkspaceModel^ Model);

			virtual void					SetText(IWorkspaceModel^ Model, String^ Text, bool ResetUndoStack);
			virtual String^					GetText(IWorkspaceModel^ Model, bool Preprocess, bool% PreprocessResult);

			virtual int						GetCaret(IWorkspaceModel^ Model);
			virtual void					SetCaret(IWorkspaceModel^ Model, int Index);

			virtual String^					GetSelection(IWorkspaceModel^ Model);
			virtual String^					GetCaretToken(IWorkspaceModel^ Model);

			virtual void					AcquireInputFocus(IWorkspaceModel^ Model);

			virtual void					New(IWorkspaceModel^ Model);
			virtual void					Open(IWorkspaceModel^ Model, componentDLLInterface::ScriptData* Data);
			virtual bool					Save(IWorkspaceModel^ Model, IWorkspaceModel::SaveOperation Operation, bool% HasWarnings);
			virtual bool					Close(IWorkspaceModel^ Model, bool% OperationCancelled);
			virtual void					Next(IWorkspaceModel^ Model);
			virtual void					Previous(IWorkspaceModel^ Model);
			virtual void					CompileDepends(IWorkspaceModel^ Model);

			virtual void					SetType(IWorkspaceModel^ Model, IWorkspaceModel::ScriptType New);
			virtual void					GotoLine(IWorkspaceModel^ Model, UInt32 Line);
			virtual UInt32					GetLineCount(IWorkspaceModel^ Model);
			virtual bool					Sanitize(IWorkspaceModel^ Model);
			virtual void					BindToForm(IWorkspaceModel^ Model);

			virtual void					LoadFromDisk(IWorkspaceModel^ Model, String^ PathToFile);
			virtual void					SaveToDisk(IWorkspaceModel^ Model, String^ PathToFile, bool PathIncludesFileName, String^ Extension);

			virtual textEditors::IScriptTextEditor::FindReplaceResult^
											FindReplace(IWorkspaceModel^ Model, textEditors::IScriptTextEditor::FindReplaceOperation Operation,
														String^ Query, String^ Replacement, textEditors::IScriptTextEditor::FindReplaceOptions Options);

			virtual bool					GetOffsetViewerData(IWorkspaceModel^ Model, String^% OutText, UInt32% OutBytecode, UInt32% OutLength);

			virtual bool					ApplyRefactor(IWorkspaceModel^ Model, IWorkspaceModel::RefactorOperation Operation, Object^ Arg);
		};

		ref class ConcreteWorkspaceModel : public IWorkspaceModel
		{
			KeyEventHandler^									TextEditorKeyDownHandler;
			textEditors::TextEditorScriptModifiedEventHandler^	TextEditorScriptModifiedHandler;
			textEditors::TextEditorMouseClickEventHandler^		TextEditorMouseClickHandler;

			void									TextEditor_KeyDown(Object^ Sender, KeyEventArgs^ E);
			void									TextEditor_ScriptModified(Object^ Sender, textEditors::TextEditorScriptModifiedEventArgs^ E);
			void									TextEditor_MouseClick(Object^ Sender, textEditors::TextEditorMouseClickEventArgs^ E);

			EventHandler^							AutoSaveTimerTickHandler;
			EventHandler^							ScriptEditorPreferencesSavedHandler;

			void                                    ScriptEditorPreferences_Saved(Object^ Sender, EventArgs^ E);
			void									AutoSaveTimer_Tick(Object^ Sender, EventArgs^ E);

			void									CheckAutoRecovery();
			void									ClearAutoRecovery();
			void									JumpToScript(String^ TargetEditorID);
			String^									GetLineText(UInt32 Line);

			void									OnStateChangedDirty(bool Modified);
			void									OnStateChangedByteCodeSize(UInt32 Size);
			void									OnStateChangedType(IWorkspaceModel::ScriptType Type);
			void									OnStateChangedDescription();
		public:
			textEditors::IScriptTextEditor^					TextEditor;
			intellisense::IIntelliSenseInterfaceModel^		IntelliSenseModel;
			ScriptBackgroundAnalysis^						BackgroundAnalysis;
			Timer^											AutoSaveTimer;

			void*									CurrentScript;
			IWorkspaceModel::ScriptType				CurrentScriptType;
			String^									CurrentScriptEditorID;
			UInt32									CurrentScriptFormID;
			UInt32									CurrentScriptBytecode;
			UInt32									CurrentScriptBytecodeLength;
			bool									NewScriptFlag;
			bool									Closed;

			ConcreteWorkspaceModelController^		ModelController;
			ConcreteWorkspaceModelFactory^			ModelFactory;

			IWorkspaceView^							BoundParent;

			void					Setup(componentDLLInterface::ScriptData* Data, bool PartialUpdate, bool NewScript);
			bool					PerformHouseKeeping();
			bool					PerformHouseKeeping(bool% OperationCancelled);

			void					Bind(IWorkspaceView^ To);
			void					Unbind();

			void					NewScript();
			void					OpenScript(componentDLLInterface::ScriptData* Data);
			bool					SaveScript(IWorkspaceModel::SaveOperation Operation, bool% HasWarnings);
			bool					CloseScript(bool% OperationCancelled);
			void					NextScript();
			void					PreviousScript();

			void					SetType(IWorkspaceModel::ScriptType New);
			bool					Sanitize();

			ConcreteWorkspaceModel(ConcreteWorkspaceModelController^ Controller, ConcreteWorkspaceModelFactory^ Factory, componentDLLInterface::ScriptData* Data);
			~ConcreteWorkspaceModel();

#pragma region Interfaces
			virtual event IWorkspaceModel::StateChangeEventHandler^			StateChangedDirty;
			virtual event IWorkspaceModel::StateChangeEventHandler^			StateChangedByteCodeSize;
			virtual event IWorkspaceModel::StateChangeEventHandler^			StateChangedType;
			virtual event IWorkspaceModel::StateChangeEventHandler^			StateChangedDescription;

			property IWorkspaceModelFactory^		Factory
			{
				virtual IWorkspaceModelFactory^ get() { return ModelFactory; }
				virtual void set(IWorkspaceModelFactory^ e) {}
			}
			property IWorkspaceModelController^		Controller
			{
				virtual IWorkspaceModelController^ get() { return ModelController; }
				virtual void set(IWorkspaceModelController^ e) {}
			}
			property bool							Initialized
			{
				virtual bool get() { return CurrentScript != nullptr; }
				virtual void set(bool e) {}
			}
			property bool							New
			{
				virtual bool get() { return NewScriptFlag; }
				virtual void set(bool e) {}
			}
			property bool							Dirty
			{
				virtual bool get() { return TextEditor->Modified; }
				virtual void set(bool e) {}
			}
			property IWorkspaceModel::ScriptType	Type
			{
				virtual IWorkspaceModel::ScriptType get() { return CurrentScriptType; }
				virtual void set(IWorkspaceModel::ScriptType e) {}
			}
			property String^						ShortDescription
			{
				virtual String^ get() { return CurrentScriptEditorID; }
				virtual void set(String^ e) {}
			}
			property String^						LongDescription
			{
				virtual String^ get() { return CurrentScriptEditorID + " [" + CurrentScriptFormID.ToString("X8") + "]"; }
				virtual void set(String^ e) {}
			}
			property Control^						InternalView
			{
				virtual Control^ get() { return TextEditor->Container; }
				virtual void set(Control^ e) {}
			}
			property bool							Bound
			{
				virtual bool get() { return BoundParent != nullptr; }
				virtual void set(bool e) {}
			}
#pragma endregion
		};
	}
}