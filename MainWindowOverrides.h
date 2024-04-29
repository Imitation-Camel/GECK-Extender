#pragma once
#include "UIManager.h"

namespace gecke
{
	namespace uiManager
	{

		LRESULT CALLBACK		MainWindowMenuInitSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
															bgsee::WindowSubclassProcCollection::SubclassProcExtraParams* SubclassParams);
		LRESULT CALLBACK		MainWindowMenuSelectSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
															bgsee::WindowSubclassProcCollection::SubclassProcExtraParams* SubclassParams);
		LRESULT CALLBACK		MainWindowMiscSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
														bgsee::WindowSubclassProcCollection::SubclassProcExtraParams* SubclassParams);

		void InitializeMainWindowOverrides();
	}
}
// custom window messages
#define WM_MAINWINDOW_INIT_DIALOG				(WM_USER + 2003)
#define WM_MAINWINDOW_INIT_EXTRADATA			(WM_USER + 2004)

#define WM_MAINTOOLBAR_INIT						(WM_USER + 2005)

// wParam = position
#define WM_MAINTOOLBAR_SETTOD					(WM_USER + 2006)