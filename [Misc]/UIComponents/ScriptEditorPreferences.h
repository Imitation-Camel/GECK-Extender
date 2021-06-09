#pragma once

namespace UIComponents {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for ScriptEditorPreferences
	/// </summary>
	public ref class ScriptEditorPreferences : public DevComponents::DotNetBar::Metro::MetroForm
	{
	public:
		ScriptEditorPreferences(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~ScriptEditorPreferences()
		{
			if (components)
			{
				delete components;
			}
		}
	private: DevComponents::DotNetBar::AdvPropertyGrid^ PropertyGrid;
	protected:


	private: DevComponents::DotNetBar::Bar^ TopBar;
	private: DevComponents::DotNetBar::LabelItem^ LabelCategories;
	private: DevComponents::DotNetBar::LabelItem^ LabelCurrentCategory;
	private: DevComponents::DotNetBar::Bar^ SidebarSettingsCategories;
	private: DevComponents::DotNetBar::ButtonItem^ buttonItem1;
	private: DevComponents::DotNetBar::PanelEx^ panelEx1;
	private: System::ComponentModel::IContainer^ components;
	protected:

	protected:

	protected:


	protected:




	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->PropertyGrid = (gcnew DevComponents::DotNetBar::AdvPropertyGrid());
			this->TopBar = (gcnew DevComponents::DotNetBar::Bar());
			this->LabelCategories = (gcnew DevComponents::DotNetBar::LabelItem());
			this->LabelCurrentCategory = (gcnew DevComponents::DotNetBar::LabelItem());
			this->SidebarSettingsCategories = (gcnew DevComponents::DotNetBar::Bar());
			this->buttonItem1 = (gcnew DevComponents::DotNetBar::ButtonItem());
			this->panelEx1 = (gcnew DevComponents::DotNetBar::PanelEx());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->PropertyGrid))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->TopBar))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->SidebarSettingsCategories))->BeginInit();
			this->panelEx1->SuspendLayout();
			this->SuspendLayout();
			// 
			// PropertyGrid
			// 
			this->PropertyGrid->Dock = System::Windows::Forms::DockStyle::Fill;
			this->PropertyGrid->GridLinesColor = System::Drawing::Color::WhiteSmoke;
			this->PropertyGrid->Location = System::Drawing::Point(0, 0);
			this->PropertyGrid->Name = L"PropertyGrid";
			this->PropertyGrid->Size = System::Drawing::Size(378, 498);
			this->PropertyGrid->TabIndex = 0;
			this->PropertyGrid->Text = L"advPropertyGrid1";
			// 
			// TopBar
			// 
			this->TopBar->AntiAlias = true;
			this->TopBar->Dock = System::Windows::Forms::DockStyle::Top;
			this->TopBar->Font = (gcnew System::Drawing::Font(L"Segoe UI", 9));
			this->TopBar->IsMaximized = false;
			this->TopBar->Items->AddRange(gcnew cli::array< DevComponents::DotNetBar::BaseItem^  >(2) { this->LabelCategories, this->LabelCurrentCategory });
			this->TopBar->Location = System::Drawing::Point(0, 0);
			this->TopBar->Name = L"TopBar";
			this->TopBar->PaddingBottom = 8;
			this->TopBar->PaddingTop = 8;
			this->TopBar->Size = System::Drawing::Size(434, 33);
			this->TopBar->Stretch = true;
			this->TopBar->Style = DevComponents::DotNetBar::eDotNetBarStyle::StyleManagerControlled;
			this->TopBar->TabIndex = 1;
			this->TopBar->TabStop = false;
			this->TopBar->Text = L"bar1";
			// 
			// LabelCategories
			// 
			this->LabelCategories->Name = L"LabelCategories";
			this->LabelCategories->Text = L"CATEGORIES";
			// 
			// LabelCurrentCategory
			// 
			this->LabelCurrentCategory->ItemAlignment = DevComponents::DotNetBar::eItemAlignment::Far;
			this->LabelCurrentCategory->Name = L"LabelCurrentCategory";
			this->LabelCurrentCategory->Text = L"Current Category";
			// 
			// SidebarSettingsCategories
			// 
			this->SidebarSettingsCategories->AntiAlias = true;
			this->SidebarSettingsCategories->Dock = System::Windows::Forms::DockStyle::Left;
			this->SidebarSettingsCategories->DockOrientation = DevComponents::DotNetBar::eOrientation::Vertical;
			this->SidebarSettingsCategories->Font = (gcnew System::Drawing::Font(L"Segoe UI", 9));
			this->SidebarSettingsCategories->IsMaximized = false;
			this->SidebarSettingsCategories->Items->AddRange(gcnew cli::array< DevComponents::DotNetBar::BaseItem^  >(1) { this->buttonItem1 });
			this->SidebarSettingsCategories->Location = System::Drawing::Point(0, 33);
			this->SidebarSettingsCategories->Name = L"SidebarSettingsCategories";
			this->SidebarSettingsCategories->PaddingLeft = 15;
			this->SidebarSettingsCategories->PaddingRight = 15;
			this->SidebarSettingsCategories->Size = System::Drawing::Size(56, 498);
			this->SidebarSettingsCategories->Stretch = true;
			this->SidebarSettingsCategories->Style = DevComponents::DotNetBar::eDotNetBarStyle::StyleManagerControlled;
			this->SidebarSettingsCategories->TabIndex = 2;
			this->SidebarSettingsCategories->TabStop = false;
			this->SidebarSettingsCategories->Text = L"bar2";
			// 
			// buttonItem1
			// 
			this->buttonItem1->Name = L"buttonItem1";
			this->buttonItem1->Text = L"buttonItem1";
			// 
			// panelEx1
			// 
			this->panelEx1->CanvasColor = System::Drawing::SystemColors::Control;
			this->panelEx1->ColorSchemeStyle = DevComponents::DotNetBar::eDotNetBarStyle::StyleManagerControlled;
			this->panelEx1->Controls->Add(this->PropertyGrid);
			this->panelEx1->DisabledBackColor = System::Drawing::Color::Empty;
			this->panelEx1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->panelEx1->Location = System::Drawing::Point(56, 33);
			this->panelEx1->Name = L"panelEx1";
			this->panelEx1->Size = System::Drawing::Size(378, 498);
			this->panelEx1->Style->Alignment = System::Drawing::StringAlignment::Center;
			this->panelEx1->Style->BackColor1->ColorSchemePart = DevComponents::DotNetBar::eColorSchemePart::PanelBackground;
			this->panelEx1->Style->Border = DevComponents::DotNetBar::eBorderType::SingleLine;
			this->panelEx1->Style->BorderColor->ColorSchemePart = DevComponents::DotNetBar::eColorSchemePart::PanelBorder;
			this->panelEx1->Style->ForeColor->ColorSchemePart = DevComponents::DotNetBar::eColorSchemePart::PanelText;
			this->panelEx1->Style->GradientAngle = 90;
			this->panelEx1->TabIndex = 3;
			this->panelEx1->Text = L"panelEx1";
			// 
			// ScriptEditorPreferences
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(434, 531);
			this->Controls->Add(this->panelEx1);
			this->Controls->Add(this->SidebarSettingsCategories);
			this->Controls->Add(this->TopBar);
			this->DoubleBuffered = true;
			this->MinimumSize = System::Drawing::Size(450, 550);
			this->Name = L"ScriptEditorPreferences";
			this->ShowIcon = false;
			this->Text = L"Preferences";
			this->Load += gcnew System::EventHandler(this, &ScriptEditorPreferences::ScriptEditorPreferences_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->PropertyGrid))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->TopBar))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->SidebarSettingsCategories))->EndInit();
			this->panelEx1->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void ScriptEditorPreferences_Load(System::Object^ sender, System::EventArgs^ e) {
		PropertyGrid->SelectedObject = this;
	}
	};
}
