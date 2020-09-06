#include "Exports.h"
#include "UseInfoList.h"

using namespace componentDLLInterface;
using namespace cse;

extern componentDLLInterface::UseInfoListInterface g_InteropInterface;

extern "C"
{
	QUERYINTERFACE_EXPORT
	{
		return &g_InteropInterface;
	}
}

void InitializeComponents(void)
{
	return;
}

void ShowUseInfoListDialog(const char* FilterString)
{
	CentralizedUseInfoList::Open(gcnew String(FilterString));
}

void HideUseInfoListDialog(void)
{
	CentralizedUseInfoList::Close();
}

componentDLLInterface::UseInfoListInterface g_InteropInterface =
{
	InitializeComponents,
	ShowUseInfoListDialog,
	HideUseInfoListDialog
};