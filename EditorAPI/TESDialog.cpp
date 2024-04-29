#include "TESForm.h"
#include "TESDialog.h"
#include "[Common]\CLIWrapper.h"

HINSTANCE*					TESCSMain::Instance = (HINSTANCE*)0x00ECFB34;
HWND*						TESCSMain::WindowHandle = (HWND*)0x00ECFB38;
HMENU*						TESCSMain::MainMenuHandle = (HMENU*)0x00ED05C0;
const char*					TESCSMain::INIFileParentFolder = (const char*)0x00F2CC80;

UInt8*						TESObjectWindow::Initialized = (UInt8*)0x00ECFB6C;
HWND*						TESObjectWindow::WindowHandle = (HWND*)0x00ECFB70;
HWND*						TESCellViewWindow::WindowHandle = (HWND*)0x00ECFB78;


void TESDialog::ShowScriptEditorDialog(TESForm* InitScript)
{
	Script* AuxScript = CS_CAST(InitScript, TESForm, Script);
	componentDLLInterface::ScriptData* Data = nullptr;

	if (AuxScript)
		Data = new componentDLLInterface::ScriptData(AuxScript);

	RECT ScriptEditorLoc;
	TESDialog::ReadBoundsFromINI("Script Edit", &ScriptEditorLoc);
	gecke::cliWrapper::interfaces::SE->InstantiateEditor(Data,
														ScriptEditorLoc.left,
														ScriptEditorLoc.top,
														ScriptEditorLoc.right,
														ScriptEditorLoc.bottom);

}

UInt32 TESDialog::WriteBoundsToINI(HWND Handle, const char* WindowClassName)
{
	return cdeclCall<UInt32>(0x0043E170, Handle, WindowClassName);
}

bool TESDialog::ReadBoundsFromINI(const char* WindowClassName, LPRECT OutRect)
{
	return cdeclCall<bool>(0x0043E3B0, WindowClassName, OutRect);
}

void TESCSMain::InitializeCSWindows()
{
	cdeclCall<bool>(0x00464C50);
}

void TESCSMain::DeinitializeCSWindows()
{
	cdeclCall<bool>(0x00464B90);
}

std::string TESCSMain::GetINIFilePath()
{
	return std::string(INIFileParentFolder) + "\\GECKPrefs.ini";
}

void* TESComboBox::GetSelectedItemData(HWND hWnd)
{
	return cdeclCall<void*>(0x00419D10);
}
