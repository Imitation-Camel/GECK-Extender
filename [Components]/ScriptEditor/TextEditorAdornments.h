#pragma once

#include "SemanticAnalysis.h"
#include "ScriptTextEditorInterface.h"
#include "BackgroundAnalysis.h"

namespace cse
{
	namespace textEditors
	{
		ref class ScopeBreadcrumbManager
		{
			static void									OnItemClick(Object^ Sender, EventArgs^ E);

			static int									InstanceCounter = 0;
			static Image^								DefaultIcon = nullptr;

			static Image^								GetScopeIcon(obScriptParsing::Structurizer::Node::NodeType Type);
		protected:
			ref struct CrumbData
			{
				IScriptTextEditor^						Parent;
				obScriptParsing::Structurizer::Node^	Scope;

				CrumbData(IScriptTextEditor^ Parent, obScriptParsing::Structurizer::Node^ Scope);

				void									Jump();
			};

			IScriptTextEditor^							BoundParent;
			scriptEditor::IBackgroundSemanticAnalyzer^	BackgroundAnalyzer;
			DevComponents::DotNetBar::CrumbBar^			Bar;
			obScriptParsing::Structurizer^				DataStore;
			Dictionary<obScriptParsing::Structurizer::Node^, DevComponents::DotNetBar::CrumbBarItem^>^
														ActiveCrumbs;
			DevComponents::DotNetBar::CrumbBarItem^		Root;

			EventHandler^								ParentLineChangedHandler;
			EventHandler^								ParentTextUpdatedHandler;
			EventHandler^								ScriptEditorPreferencesSavedHandler;
			scriptEditor::SemanticAnalysisCompleteEventHandler^
														ParentBGAnalysisCompleteHandler;

			void										Parent_LineChanged(Object^ Sender, EventArgs^ E);
			void										Parent_BackgroundAnalysisComplete(Object^ Sender, scriptEditor::SemanticAnalysisCompleteEventArgs^ E);
			void										Parent_TextUpdated(Object^ Sender, EventArgs^ E);
			void										ScriptEditorPreferences_Saved(Object^ Sender, EventArgs^ E);

			void										GenerateCrumbs(obScriptParsing::AnalysisData^ Data);
			void										EnumerateChildCrumbs(DevComponents::DotNetBar::CrumbBarItem^ Item);
			void										UpdateToCurrentLine();
			void										CreateNewCrumb(obScriptParsing::Structurizer::Node^ Source, DevComponents::DotNetBar::CrumbBarItem^ Parent, bool EnumerateChildren);
			void										ResetCrumbs();
		public:
			ScopeBreadcrumbManager(DevComponents::DotNetBar::CrumbBar^ Bar);
			~ScopeBreadcrumbManager();

			property bool								Bound
			{
				virtual bool get() { return BoundParent != nullptr; }
				virtual void set(bool e) {}
			}

			property bool								Visible
			{
				virtual bool get() { return Bar->Visible; }
				virtual void set(bool e) { Bar->Visible = e; }
			}

			void										Bind(IScriptTextEditor^ Parent, scriptEditor::IBackgroundSemanticAnalyzer^ BackgroundAnalyzer);
			void										Unbind();

			void										RefreshCrumbs();
		};
	}
}