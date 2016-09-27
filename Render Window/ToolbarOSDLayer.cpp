#include "ToolbarOSDLayer.h"
#include "Render Window\RenderWindowManager.h"
#include "IconFontCppHeaders\IconsMaterialDesign.h"
#include "RenderWindowActions.h"

namespace cse
{
	namespace renderWindow
	{
#define PUSH_TRANSPARENT_BUTTON_COLORS				ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0, 0, 0, 0)); \
													ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0, 0, 0, 0)); \
													ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0, 0, 0, 0))
#define POP_TRANSPARENT_BUTTON_COLORS				ImGui::PopStyleColor(3);
#define TOOLBAR_BUTTON_SIZE							ImVec2(30, 0)

		ToolbarOSDLayer				ToolbarOSDLayer::Instance;
		constexpr int				kFilterRefsReset = -9;

		int ToolbarOSDLayer::RefFilterCompletionCallback(ImGuiTextEditCallbackData* Data)
		{
			if ((Data->EventFlag & ImGuiInputTextFlags_CallbackCompletion))
			{
				if (Data->BufTextLen)
				{
					bool CycleBack = ImGui::GetIO().KeyShift;
					FormIDArrayT::const_iterator Bookend = CycleBack ? FilterRefs.begin() : FilterRefs.end();

					// cycle through the filtered refs
					while (FilterRefs.size())
					{
						if (CurrentFilterRefIndex == kFilterRefsReset)
						{
							// first selection, init the index
							if (FilterRefs.size() == 1)
								CurrentFilterRefIndex = 0;
							else if (CycleBack)
								CurrentFilterRefIndex = FilterRefs.size() - 1;		// last item
							else
								CurrentFilterRefIndex = 0;							// first item
						}
						else if (CycleBack)
							CurrentFilterRefIndex--;
						else
							CurrentFilterRefIndex++;

						if (CurrentFilterRefIndex > -1 && CurrentFilterRefIndex < FilterRefs.size())
						{
							// locate and select the reference
							TESForm* Form = TESForm::LookupByFormID(FilterRefs.at(CurrentFilterRefIndex));
							TESObjectREFR* NextRef = nullptr;
							if (Form && Form->IsDeleted() == false)
							{
								SME_ASSERT(Form->formType == TESForm::kFormType_ACHR || Form->formType == TESForm::kFormType_ACRE || Form->formType == TESForm::kFormType_REFR);

								NextRef = CS_CAST(Form, TESForm, TESObjectREFR);
							}
							else
							{
								// invalid ref, remove it and move to the next one
								FilterRefs.erase(FilterRefs.begin() + CurrentFilterRefIndex);
								continue;
							}

							SME_ASSERT(NextRef);
							_TES->LoadCellIntoViewPort(nullptr, NextRef);
						}
						else
						{
							CurrentFilterRefIndex = kFilterRefsReset;
							continue;
						}

						break;
					}
				}
			}

			return 0;
		}

		void ToolbarOSDLayer::HandleRefFilterChange()
		{
			if (strlen(RefFilter.InputBuf))
			{
				// refresh the filter
				FilterRefs.clear();
				RefFilter.Build();

				for (auto Itr : _RENDERWIN_MGR.GetActiveRefs())
				{
					std::string EditorID(Helpers::GetRefEditorID(Itr));
					UInt32 FormID = Itr->formID;
					const char* Type = TESForm::GetFormTypeIDLongName(Itr->baseForm->formType);

					char FilterBuffer[0x200] = { 0 };
					FORMAT_STR(FilterBuffer, "%s %08X %s", EditorID.c_str(), FormID, Type);
					if (RefFilter.PassFilter(FilterBuffer))
						FilterRefs.push_back(FormID);
				}
			}
			else
				FilterRefs.clear();

			CurrentFilterRefIndex = kFilterRefsReset;
		}

		void ToolbarOSDLayer::RenderBottomToolbars(ImGuiDX9* GUI)
		{
			static const int kRegularHeight = 45;
			static const int kMinWidthRefFilter = 250;

			int XSize = kMinWidthRefFilter;
			int XDel = *TESRenderWindow::ScreenWidth - XSize;
			int XPos = XDel / 2;
			int YPos = *TESRenderWindow::ScreenHeight - kRegularHeight - 8;
			int YSize = kRegularHeight;

			if (BeginToolbarWindow("Bottom Ref Filter", XPos, YPos, XSize, YSize, ImVec2(7, 10), ImVec2(5, 5), ImVec2(5, 5)) == false)
				return;
			else
			{
				TransparentButton(ICON_MD_SEARCH "##find_ref_mouseover", TOOLBAR_BUTTON_SIZE);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Type in the text box to find references matching the filter string.\nCycle through matches with the (SHIFT+)TAB key.");
				ImGui::SameLine(0, 10);

				if (SetRefFilterFocus)
				{
					SetRefFilterFocus = false;
					ImGui::SetKeyboardFocusHere();
				}

				bool HasFilter = strlen(RefFilter.InputBuf);
				ImGui::PushItemWidth(HasFilter ? 140 : 175);
				if (ImGui::InputText("##find_ref_textbox", RefFilter.InputBuf, sizeof(RefFilter.InputBuf),
									 ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CallbackCompletion,
									 [](ImGuiTextEditCallbackData* Data)->int { ToolbarOSDLayer* Parent = (ToolbarOSDLayer*)Data->UserData; return Parent->RefFilterCompletionCallback(Data); },
									 this))
				{
					HandleRefFilterChange();
				}
				ImGui::PopItemWidth();

				ImGui::SameLine(0, 10);
				if (HasFilter)
				{
					ImGui::Text(ICON_MD_FILTER_LIST " %d ", FilterRefs.size());
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("%d references match the filter.", FilterRefs.size());
				}
				else
					ImGui::Dummy(ImVec2(5, 0));

				EndToolbarWindow();
			}

			static const int kMinWidthMiscControls = 165;
			XSize = kMinWidthMiscControls;
			XPos = *TESRenderWindow::ScreenWidth - XSize - 10;

			if (BeginToolbarWindow("Bottom Misc Popups", XPos, YPos, XSize, YSize, ImVec2(7, 10), ImVec2(5, 5), ImVec2(5, 5)) == false)
				return;
			else
			{
				void* CurrentToolbarWindow = GUI->GetCurrentWindow();

				BottomToolbarPopupProvider.Draw(PopupSnapControls, GUI, CurrentToolbarWindow);
				ImGui::SameLine(0, 10);
				BottomToolbarPopupProvider.Draw(PopupMovementControls, GUI, CurrentToolbarWindow);
				ImGui::SameLine(0, 10);
				BottomToolbarPopupProvider.Draw(PopupVisibilityToggles, GUI, CurrentToolbarWindow);
				ImGui::SameLine(0, 10);
				BottomToolbarPopupProvider.Draw(PopupMiscControls, GUI, CurrentToolbarWindow);

				EndToolbarWindow();
			}
		}

		void ToolbarOSDLayer::RenderTopToolbar(ImGuiDX9* GUI)
		{
			int XSize = *TESRenderWindow::ScreenWidth;
			int Width = 5 + TopToolbarPopupIDs.size() * 40;

			if (BeginToolbarWindow("Top Dock", XSize - Width, 10, -1, -1, ImVec2(7, 7), ImVec2(5, 5), ImVec2(7, 7)) == false)
				return;
			else
			{
				for (auto Itr : TopToolbarPopupIDs)
				{
					TopToolbarPopupProvider.Draw(Itr, GUI, GUI->GetCurrentWindow());
					ImGui::SameLine(0, 10);
				}

				EndToolbarWindow();
			}
		}

		bool ToolbarOSDLayer::BeginToolbarWindow(const char* Name,
												 int XPos, int YPos, int Width, int Height,
												 const ImVec2& WindowPadding, const ImVec2& FramePadding, const ImVec2& ItemSpacing)
		{
			bool AutoResize = Width == -1 && Height == -1;
			ImGui::SetNextWindowPos(ImVec2(XPos, YPos));
			if (AutoResize == false)
				ImGui::SetNextWindowSize(ImVec2(Width, Height));

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ItemSpacing);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, WindowPadding);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, FramePadding);
			if (!ImGui::Begin(Name, nullptr,
							  ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus |
							  ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollbar |
							  (AutoResize ? ImGuiWindowFlags_AlwaysAutoResize : NULL)))
			{
				EndToolbarWindow();
				return false;
			}
			else
				return true;
		}

		void ToolbarOSDLayer::EndToolbarWindow()
		{
			ImGui::End();
			ImGui::PopStyleVar(3);
		}

		ToolbarOSDLayer::ToolbarOSDLayer() :
			IRenderWindowOSDLayer(&settings::renderWindowOSD::kShowToolbar)
		{
			PopupSnapControls = BottomToolbarPopupProvider.RegisterPopup("popup_snap_controls",
															[]() {
				PUSH_TRANSPARENT_BUTTON_COLORS;
				ImGui::Button(ICON_MD_GRID_ON "##popupbtn_snap_controls", TOOLBAR_BUTTON_SIZE);
				POP_TRANSPARENT_BUTTON_COLORS;
			},
															[]() {
				UInt32 Flags = *TESRenderWindow::StateFlags;
				bool SnapGrid = Flags & TESRenderWindow::kRenderWindowState_SnapToGrid;
				bool SnapAngle = Flags & TESRenderWindow::kRenderWindowState_SnapToAngle;
				int GridVal = *(UInt32*)TESRenderWindow::SnapGridDistance;
				int AngleVal = *(UInt32*)TESRenderWindow::SnapAngle;
				TESObjectREFR* SnapRef = *TESRenderWindow::SnapReference;

				if (ImGui::Checkbox("Snap Grid   ", &SnapGrid))
				{
					if (SnapGrid)
						Flags |= TESRenderWindow::kRenderWindowState_SnapToGrid;
					else
						Flags &= ~TESRenderWindow::kRenderWindowState_SnapToGrid;
				}
				ImGui::SameLine();
				ImGui::PushItemWidth(40);
				ImGui::DragInt("##gridDist", &GridVal, 1, 0, 5000);
				ImGui::PopItemWidth();

				ImGui::Separator();

				if (ImGui::Checkbox("Snap Angle ", &SnapAngle))
				{
					if (SnapAngle)
						Flags |= TESRenderWindow::kRenderWindowState_SnapToAngle;
					else
						Flags &= ~TESRenderWindow::kRenderWindowState_SnapToAngle;
				}
				ImGui::SameLine();
				ImGui::PushItemWidth(40);
				ImGui::DragInt("##anglVal", &AngleVal, 1, 0, 500);
				ImGui::PopItemWidth();

				ImGui::Separator();

				ImGui::Text("Snap Reference:");
				if (SnapRef)
				{
					const char* SnapRefID = SnapRef->GetEditorID();
					ImGui::TextWrapped("%s%s%08X%s", (SnapRefID ? SnapRefID : ""), (SnapRefID ? " (" : ""), SnapRef->formID, (SnapRefID ? ")" : ""));
				}
				else
					ImGui::Text("None");

				if (ImGui::Button("Set", ImVec2(70, 25)))
				{
					TESObjectREFR* NewSnap = RefSelectControl::ShowSelectReferenceDialog(*TESRenderWindow::WindowHandle, SnapRef, true);
					if (NewSnap)
						SnapRef = NewSnap;
				}
				ImGui::SameLine();
				if (ImGui::Button("Clear", ImVec2(70, 25)))
					SnapRef = nullptr;

				*(UInt32*)TESRenderWindow::SnapGridDistance = GridVal;
				*(UInt32*)TESRenderWindow::SnapAngle = AngleVal;
				*TESRenderWindow::StateFlags = Flags;
				*TESRenderWindow::SnapReference = SnapRef;
			},
				MouseOverPopupProvider::kPosition_Relative,
				ImVec2(-85, -165));

			PopupMovementControls= BottomToolbarPopupProvider.RegisterPopup("popup_movement_controls",
															   []() {
				const ImVec4 MainColor = ImColor::HSV(4 / 7.0f, 0.6f, 0.6f);

				PUSH_TRANSPARENT_BUTTON_COLORS;
				ImGui::Button(ICON_MD_ZOOM_OUT_MAP "##popupbtn_movement_controls", TOOLBAR_BUTTON_SIZE);
				POP_TRANSPARENT_BUTTON_COLORS;
			},
															   []() {
				float CamPan = *TESRenderWindow::CameraPanSpeed;
				float CamZoom = *TESRenderWindow::CameraZoomSpeed;
				float CamRot = *TESRenderWindow::CameraRotationSpeed;
				float RefMov = *TESRenderWindow::RefMovementSpeed;
				float RefRot = *TESRenderWindow::RefRotationSpeed;

				float AltCamPan = settings::renderer::kAltCamPanSpeed().f;
				float AltCamZoom = settings::renderer::kAltCamZoomSpeed().f;
				float AltCamRot = settings::renderer::kAltCamRotationSpeed().f;
				float AltRefMov = settings::renderer::kAltRefMovementSpeed().f;
				float AltRefRot = settings::renderer::kAltRefRotationSpeed().f;

				ImGui::PushItemWidth(125);
				ImGui::Text("Default"); ImGui::SameLine(160); ImGui::Text("Alternate");
				ImGui::Separator();

				ImGui::Text("Camera:");
				ImGui::DragFloat("##DefCam. Pan", &CamPan, 0.05f, 0.01, 10, "Pan: %.3f"); ImGui::SameLine(0, 20);
				ImGui::DragFloat("##AltCam. Pan", &AltCamPan, 0.05f, 0.01, 10, "Pan: %.3f");
				ImGui::DragFloat("##DefCam. Rotation", &CamRot, 0.05f, 0.01, 10, "Rotation: %.3f"); ImGui::SameLine(0, 20);
				ImGui::DragFloat("##AltCam. Rotation", &AltCamRot, 0.05f, 0.01, 10, "Rotation: %.3f");
				ImGui::DragFloat("##DefCam. Zoom", &CamZoom, 0.05f, 0.01, 10, "Zoom: %.3f"); ImGui::SameLine(0, 20);
				ImGui::DragFloat("##AltCam. Zoom", &AltCamZoom, 0.05f, 0.01, 10, "Zoom: %.3f");

				ImGui::Text("Reference:");
				ImGui::DragFloat("##DefRef. Move", &RefMov, 0.05f, 0.01, 10, "Movement: %.3f"); ImGui::SameLine(0, 20);
				ImGui::DragFloat("##AltRef. Move", &AltRefMov, 0.05f, 0.01, 10, "Movement: %.3f");
				ImGui::DragFloat("##DefRef. Rotation", &RefRot, 0.05f, 0.01, 10, "Rotation: %.3f"); ImGui::SameLine(0, 20);
				ImGui::DragFloat("##AltRef. Rotation", &AltRefRot, 0.05f, 0.01, 10, "Rotation: %.3f");
				ImGui::PopItemWidth();

				*TESRenderWindow::CameraPanSpeed = CamPan;
				*TESRenderWindow::CameraZoomSpeed = CamZoom;
				*TESRenderWindow::CameraRotationSpeed = CamRot;
				*TESRenderWindow::RefMovementSpeed = RefMov;
				*TESRenderWindow::RefRotationSpeed = RefRot;

				settings::renderer::kAltCamPanSpeed.SetFloat(AltCamPan);
				settings::renderer::kAltCamZoomSpeed.SetFloat(AltCamZoom);
				settings::renderer::kAltCamRotationSpeed.SetFloat(AltCamRot);
				settings::renderer::kAltRefMovementSpeed.SetFloat(AltRefMov);
				settings::renderer::kAltRefRotationSpeed.SetFloat(AltRefRot);
			},
				MouseOverPopupProvider::kPosition_Relative,
				ImVec2(-145, -245));

			PopupVisibilityToggles = BottomToolbarPopupProvider.RegisterPopup("popup_visibility_toggles",
																 []() {
				PUSH_TRANSPARENT_BUTTON_COLORS;
				ImGui::Button(ICON_MD_REMOVE_RED_EYE "##popupbtn_visibility_toggles", TOOLBAR_BUTTON_SIZE);
				POP_TRANSPARENT_BUTTON_COLORS;
			},
																 []() {
				ImGui::PushID("visibility_toggle_menu_item");
				{
					static const char* kNames[actions::ToggleVisibilityRWA::kType__MAX] =
					{
						"Objects",
						"Markers",
						"Wireframe",
						"Bright Light",
						"Sky",
						"Solid Subspaces",
						"Collision Geometry",
						"Leaves",
						"Trees",
						"Water",
						"Cell Borders",
						"Landscape",
						"Light Radius",
						"Parent Child Indicator",
						"Path Grid Linked Reference Indicator",
						"Initially Disabled References",
						"Initially Disabled References' Children",
						"Grass Overlay"
					};

					bool Toggles[actions::ToggleVisibilityRWA::kType__MAX] = { false };
					for (int i = 0; i < actions::ToggleVisibilityRWA::kType__MAX; i++)
					{

						Toggles[i] = actions::ToggleVisibilityRWA::IsVisible(i);
						if (ImGui::Checkbox(kNames[i], &Toggles[i]))
							actions::ToggleVisibility[i]();
					}
				}
				ImGui::PopID();
			},
				MouseOverPopupProvider::kPosition_Relative,
				ImVec2(-10, -585));

			PopupMiscControls = BottomToolbarPopupProvider.RegisterPopup("popup_misc_controls",
																		 []() {
				PUSH_TRANSPARENT_BUTTON_COLORS;
				ImGui::Button(ICON_MD_MORE_HORIZ "##popupbtn_misc_controls", TOOLBAR_BUTTON_SIZE);
				POP_TRANSPARENT_BUTTON_COLORS;
			},
																		 []() {

				float TOD = _TES->GetSkyTOD();
				float FOV = settings::renderer::kCameraFOV().f;

				PUSH_TRANSPARENT_BUTTON_COLORS;
				ImGui::Button(ICON_MD_ACCESS_TIME " ");
				ImGui::SameLine(0, 2);
				ImGui::PushItemWidth(50);
				ImGui::DragFloat("Time of Day##TOD", &TOD, 0.25f, 0.f, 24.f, "%.2f");
				ImGui::PopItemWidth();

				ImGui::Button(ICON_MD_PANORAMA_HORIZONTAL " ");
				ImGui::SameLine(0, 2);
				ImGui::PushItemWidth(50);
				ImGui::DragFloat("Field-of-Vision##FOV", &FOV, 1.f, 50.f, 120.f, "%.0f");
				ImGui::PopItemWidth();
				POP_TRANSPARENT_BUTTON_COLORS;

				if (TOD < 0 || TOD > 24)
					TOD = 10;

				if (TOD != _TES->GetSkyTOD())
					_TES->SetSkyTOD(TOD);

				if (FOV < 50 || FOV > 120)
					FOV = 75;

				if (settings::renderer::kCameraFOV().f != FOV)
				{
					settings::renderer::kCameraFOV.SetFloat(FOV);
					_RENDERWIN_MGR.RefreshFOV();
				}
			},
				MouseOverPopupProvider::kPosition_Relative,
				ImVec2(-25, -10));

			FilterRefs.reserve(100);
			CurrentFilterRefIndex = -1;
			SetRefFilterFocus = false;
		}

		ToolbarOSDLayer::~ToolbarOSDLayer()
		{
			;//
		}

		void ToolbarOSDLayer::Draw(RenderWindowOSD* OSD, ImGuiDX9* GUI)
		{
			BottomToolbarPopupProvider.Update();
			TopToolbarPopupProvider.Update();

			RenderBottomToolbars(GUI);
			RenderTopToolbar(GUI);
		}

		bool ToolbarOSDLayer::NeedsBackgroundUpdate()
		{
			return false;
		}

		void ToolbarOSDLayer::FocusOnRefFilter()
		{
			SetRefFilterFocus = true;
		}

		void ToolbarOSDLayer::RegisterTopToolbarButton(const char* PopupID,
													   MouseOverPopupProvider::RenderDelegateT DrawButton,
													   MouseOverPopupProvider::RenderDelegateT DrawPopup)
		{
			MouseOverPopupProvider::PopupIDT ID = TopToolbarPopupProvider.RegisterPopup(PopupID, DrawButton, DrawPopup,
																						MouseOverPopupProvider::kPosition_Relative,
																						ImVec2(0, 25));
			TopToolbarPopupIDs.push_back(ID);
		}

		void ToolbarOSDLayer::TransparentButton(const char* Name, const ImVec2& Size)
		{
			PUSH_TRANSPARENT_BUTTON_COLORS;
			ImGui::Button(Name, Size);
			POP_TRANSPARENT_BUTTON_COLORS;
		}

	}
}