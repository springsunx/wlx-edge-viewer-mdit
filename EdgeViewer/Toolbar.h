#pragma once

#include "Globals.h"
#include <commctrl.h>

//------------------------------------------------------------------------
// Navigation Toolbar for EdgeViewer
//------------------------------------------------------------------------
class Toolbar
{
public:
	Toolbar();
	~Toolbar();

	// Delete copy constructor and assignment operator
	Toolbar(const Toolbar&) = delete;
	Toolbar& operator=(const Toolbar&) = delete;

	// Move constructor and assignment operator
	Toolbar(Toolbar&& other) noexcept;
	Toolbar& operator=(Toolbar&& other) noexcept;

	bool Create(HWND hParent, HINSTANCE hInst);
	void Resize();
	void UpdateState(ViewPtr webview);

	HWND GetHwnd() const { return m_hToolbar; }
	int GetHeight() const { return m_height; }
	bool IsCreated() const { return m_hToolbar != nullptr; }

	// Toolbar button IDs
	enum ButtonID {
		ID_BACK = 1001,
		ID_FORWARD = 1002,
		ID_HOME = 1003,
		ID_REFRESH = 1004,
	};

private:
	HWND m_hToolbar = nullptr;
	HWND m_hParent = nullptr;
	int m_height = 32;
	bool m_isDarkMode = false;

	void ApplyTheme();
};
//------------------------------------------------------------------------
