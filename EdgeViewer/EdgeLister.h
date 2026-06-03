#pragma once

#include "Globals.h"
#include "Toolbar.h"

//------------------------------------------------------------------------
class EdgeLister
{
public:
	static void RegisterClass(HINSTANCE hinst);

private:
	static LRESULT CALLBACK pluginWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static void showPopupMenu(HWND hWnd, const std::wstring& filename);
	static void HandleToolbarCommand(HWND hWnd, ViewPtr webview, UINT cmdId);
};
//------------------------------------------------------------------------
