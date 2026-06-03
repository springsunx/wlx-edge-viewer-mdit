#include "Toolbar.h"
#include <string>
#include <commctrl.h>

#pragma comment(lib, "comctl32.lib")

//------------------------------------------------------------------------
Toolbar::Toolbar()
{
}

//------------------------------------------------------------------------
Toolbar::~Toolbar()
{
	// Don't destroy the toolbar window here - it's owned by the parent
	m_hToolbar = nullptr;
}

//------------------------------------------------------------------------
Toolbar::Toolbar(Toolbar&& other) noexcept
	: m_hToolbar(other.m_hToolbar)
	, m_hParent(other.m_hParent)
	, m_height(other.m_height)
	, m_isDarkMode(other.m_isDarkMode)
{
	other.m_hToolbar = nullptr;
}

//------------------------------------------------------------------------
Toolbar& Toolbar::operator=(Toolbar&& other) noexcept
{
	if (this != &other)
	{
		m_hToolbar = other.m_hToolbar;
		m_hParent = other.m_hParent;
		m_height = other.m_height;
		m_isDarkMode = other.m_isDarkMode;
		other.m_hToolbar = nullptr;
	}
	return *this;
}

//------------------------------------------------------------------------
bool Toolbar::Create(HWND hParent, HINSTANCE hInst)
{
	m_hParent = hParent;
	m_isDarkMode = gs_IsDarkMode;

	// Initialize common controls
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_BAR_CLASSES;
	InitCommonControlsEx(&icex);

	// Create toolbar window
	m_hToolbar = CreateWindowEx(
		0,
		TOOLBARCLASSNAME,
		NULL,
		WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | CCS_NODIVIDER | CCS_NOPARENTALIGN,
		0, 0, 200, m_height,
		hParent,
		NULL,
		hInst,
		NULL
	);

	if (!m_hToolbar)
	{
		OutputDebugString(L"Toolbar: CreateWindowEx failed");
		return false;
	}

	OutputDebugString(L"Toolbar: Window created");

	// IMPORTANT: Must call TB_BUTTONSTRUCTSIZE before TB_ADDBUTTONS
	SendMessage(m_hToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);

	// Set toolbar extended style
	SendMessage(m_hToolbar, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS);

	// Set button size
	SendMessage(m_hToolbar, TB_SETBUTTONSIZE, 0, MAKELONG(60, 28));
	SendMessage(m_hToolbar, TB_SETBITMAPSIZE, 0, MAKELONG(0, 0));

	// Add buttons
	TBBUTTON buttons[4] = {};

	// Back button
	buttons[0].iBitmap = I_IMAGENONE;
	buttons[0].idCommand = ID_BACK;
	buttons[0].fsState = TBSTATE_ENABLED;
	buttons[0].fsStyle = BTNS_BUTTON | BTNS_AUTOSIZE;
	buttons[0].iString = (INT_PTR)L" Back ";

	// Forward button
	buttons[1].iBitmap = I_IMAGENONE;
	buttons[1].idCommand = ID_FORWARD;
	buttons[1].fsState = TBSTATE_ENABLED;
	buttons[1].fsStyle = BTNS_BUTTON | BTNS_AUTOSIZE;
	buttons[1].iString = (INT_PTR)L" Fwd ";

	// Home button
	buttons[2].iBitmap = I_IMAGENONE;
	buttons[2].idCommand = ID_HOME;
	buttons[2].fsState = TBSTATE_ENABLED;
	buttons[2].fsStyle = BTNS_BUTTON | BTNS_AUTOSIZE;
	buttons[2].iString = (INT_PTR)L" Home ";

	// Refresh button
	buttons[3].iBitmap = I_IMAGENONE;
	buttons[3].idCommand = ID_REFRESH;
	buttons[3].fsState = TBSTATE_ENABLED;
	buttons[3].fsStyle = BTNS_BUTTON | BTNS_AUTOSIZE;
	buttons[3].iString = (INT_PTR)L" Refresh ";

	SendMessage(m_hToolbar, TB_BUTTONCOUNT, 0, 0);
	LRESULT result = SendMessage(m_hToolbar, TB_ADDBUTTONS, 4, (LPARAM)&buttons);
	
	if (result)
		OutputDebugString(L"Toolbar: Buttons added");
	else
		OutputDebugString(L"Toolbar: Failed to add buttons");

	// Force the toolbar to show
	ShowWindow(m_hToolbar, SW_SHOW);
	UpdateWindow(m_hToolbar);

	// Apply theme
	ApplyTheme();

	return true;
}

//------------------------------------------------------------------------
void Toolbar::Resize()
{
	if (!m_hToolbar)
		return;

	RECT rcParent;
	GetClientRect(m_hParent, &rcParent);

	// Set toolbar bounds (full width)
	SetWindowPos(m_hToolbar, HWND_TOP, 0, 0, rcParent.right, m_height, SWP_SHOWWINDOW);
}

//------------------------------------------------------------------------
void Toolbar::UpdateState(ViewPtr webview)
{
	if (!webview || !m_hToolbar)
		return;

	// Update back/forward button states
	BOOL canGoBack = FALSE;
	BOOL canGoForward = FALSE;
	webview->get_CanGoBack(&canGoBack);
	webview->get_CanGoForward(&canGoForward);

	SendMessage(m_hToolbar, TB_ENABLEBUTTON, ID_BACK, MAKELONG(canGoBack, 0));
	SendMessage(m_hToolbar, TB_ENABLEBUTTON, ID_FORWARD, MAKELONG(canGoForward, 0));

	// Update dark mode state
	if (gs_IsDarkMode != m_isDarkMode)
	{
		m_isDarkMode = gs_IsDarkMode;
		ApplyTheme();
	}
}

//------------------------------------------------------------------------
void Toolbar::ApplyTheme()
{
	if (!m_hToolbar)
		return;

	if (m_isDarkMode)
	{
		// Dark theme colors
		COLORREF darkBg = RGB(50, 50, 50);
		SetClassLongPtr(m_hToolbar, GCLP_HBRBACKGROUND, (LONG_PTR)CreateSolidBrush(darkBg));
	}
	else
	{
		// Light theme colors
		SetClassLongPtr(m_hToolbar, GCLP_HBRBACKGROUND, (LONG_PTR)GetSysColorBrush(COLOR_BTNFACE));
	}

	InvalidateRect(m_hToolbar, NULL, TRUE);
}
//------------------------------------------------------------------------
