#include "EdgeLister.h"
#include "Navigator.h"

#include <ShlObj.h>
#include <wil/com.h>
#include <windows.h>
#include <string>
#include <Shlwapi.h>

// Global toolbar instance (one per window)
std::map<HWND, Toolbar> gs_Toolbars;
bool gs_ShowToolbar = true;

//------------------------------------------------------------------------
void EdgeLister::RegisterClass(HINSTANCE hinst)
{
	// Check if toolbar should be shown
	gs_ShowToolbar = to_int(GlobalSettings()["Chromium"]["ShowToolbar"]) != 0;

	WNDCLASSA wc = {};
	wc.hInstance = hinst;
	wc.lpfnWndProc = pluginWndProc;
	wc.lpszClassName = EDGE_LISTER_CLASS;
	RegisterClassA(&wc);
}
//------------------------------------------------------------------------
void EdgeLister::HandleToolbarCommand(HWND hWnd, ViewPtr webview, UINT cmdId)
{
	if (!webview)
		return;

	Navigator nav(webview);

	switch (cmdId)
	{
	case Toolbar::ID_BACK:
		nav.GoBack();
		break;
	case Toolbar::ID_FORWARD:
		nav.GoForward();
		break;
	case Toolbar::ID_REFRESH:
		webview->Reload();
		break;
	}
}
//------------------------------------------------------------------------
LRESULT EdgeLister::pluginWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		{
			OutputDebugString(L"EdgeLister: WM_CREATE - Creating toolbar");
			
			// Create toolbar
			auto [it, inserted] = gs_Toolbars.try_emplace(hWnd);
			if (inserted)
			{
				if (it->second.Create(hWnd, GetModuleHandle(NULL)))
				{
					it->second.Resize();
					OutputDebugString(L"EdgeLister: Toolbar created and resized");
					// Post WM_SIZE to adjust WebView bounds after toolbar is created
					PostMessage(hWnd, WM_SIZE, 0, 0);
				}
				else
				{
					OutputDebugString(L"EdgeLister: Toolbar creation failed!");
				}
			}
		}
		break;

	case WM_SIZE:
		{
			// Resize toolbar
			auto toolbarIt = gs_Toolbars.find(hWnd);
			if (toolbarIt != gs_Toolbars.end())
			{
				toolbarIt->second.Resize();
			}

			// Resize WebView if exists
			auto viewIt = gs_Views.find(hWnd);
			if (viewIt != gs_Views.end())
			{
				RECT bounds;
				GetClientRect(hWnd, &bounds);

				// Adjust for toolbar
				int toolbarHeight = 0;
				if (toolbarIt != gs_Toolbars.end())
				{
					toolbarHeight = toolbarIt->second.GetHeight();
				}

				bounds.top += toolbarHeight;
				viewIt->second->put_Bounds(bounds);
			}
		}
		break;

	case WM_COMMAND:
		{
			UINT cmdId = LOWORD(wParam);
			if (cmdId >= Toolbar::ID_BACK && cmdId <= Toolbar::ID_REFRESH)
			{
				auto it = gs_Views.find(hWnd);
				if (it != gs_Views.end())
				{
					ViewPtr webview;
					it->second->get_CoreWebView2(&webview);
					HandleToolbarCommand(hWnd, webview, cmdId);
				}
			}
		}
		break;

	case WM_NOTIFY:
		{
			// Handle toolbar tooltips
			LPNMHDR pnmh = (LPNMHDR)lParam;
			if (pnmh->code == TTN_GETDISPINFO)
			{
				LPNMTTDISPINFO pttv = (LPNMTTDISPINFO)lParam;
				switch (pttv->hdr.idFrom)
				{
				case Toolbar::ID_BACK:
					wcscpy_s(pttv->szText, L"Back (Alt+Left)");
					break;
				case Toolbar::ID_FORWARD:
					wcscpy_s(pttv->szText, L"Forward (Alt+Right)");
					break;
				case Toolbar::ID_REFRESH:
					wcscpy_s(pttv->szText, L"Refresh (F5)");
					break;
				}
			}
		}
		break;

	case WM_COPYDATA:	// generic "data received" event
		{
			auto it = gs_Views.find(hWnd);
			if (it != gs_Views.end())
			{
				ViewPtr webview;
				it->second->get_CoreWebView2(&webview);
				auto pcds = (COPYDATASTRUCT*)lParam;
				auto strData = std::wstring((wchar_t*)pcds->lpData);

				// command: navigate to the specified resource
				if (pcds->dwData == CMD_NAVIGATE)
				{
					Navigator(webview).Open(strData);

					// Update toolbar after navigation
					auto toolbarIt = gs_Toolbars.find(hWnd);
					if (toolbarIt != gs_Toolbars.end())
					{
						toolbarIt->second.UpdateState(webview);
					}
				}

				// print the current file
				if (pcds->dwData == CMD_PRINT)
					Navigator(webview).Print();

                // right-click (sent by DirProcessor)
                if (pcds->dwData == CMD_MENU)
                    showPopupMenu(hWnd, strData);

				// search text in the browser window
				if (pcds->dwData == CMD_SEARCH)
				{
					size_t i = strData.find_first_of(L' ');
					int params = std::stoi(strData.substr(0, i));
					std::wstring pattern = strData.substr(i + 1);
					Navigator(webview).Search(pattern, params);
				}
			}
		}
		break;

	case WM_SETFOCUS:	// set the real focus on the webview
		{
			auto it = gs_Views.find(hWnd);
			if (it != gs_Views.end())
			{
				it->second->MoveFocus(COREWEBVIEW2_MOVE_FOCUS_REASON_PROGRAMMATIC);
			}
		}
		break;

	case WM_WEBVIEW_JS_KEYDOWN:
		{
			// pass hotkeys 1-8, and 'q'/'Q' to allow closing the lister
			if ((wParam >= '1' && wParam <= '8') || wParam == 'Q')
				PostMessage(GetParent(hWnd), WM_KEYDOWN, wParam, NULL);
			break;
		}
	case WM_WEBVIEW_KEYDOWN:	// resend webview keypess events to the parent
		{
			PostMessage(GetParent(hWnd), WM_KEYDOWN, wParam, NULL);
		}
		break;

	case WM_DESTROY:
		{
			// Cleanup toolbar
			gs_Toolbars.erase(hWnd);
		}
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
//------------------------------------------------------------------------
void EdgeLister::showPopupMenu(HWND hWnd, const std::wstring& filename)
{
	POINT point;
	GetCursorPos(&point);
	
	// Initialize COM
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		// Get the desktop shell folder
		wil::com_ptr<IShellFolder> pDesktopFolder;
		hr = SHGetDesktopFolder(&pDesktopFolder);
		if (SUCCEEDED(hr))
		{
			// Get PIDL for the parent directory
			std::wstring parentPath = filename.substr(0, filename.find_last_of(L'\\'));
			LPITEMIDLIST pidlParent = NULL;

			hr = pDesktopFolder->ParseDisplayName(hWnd, NULL, (LPWSTR)parentPath.c_str(), NULL, &pidlParent, NULL);
			if (SUCCEEDED(hr))
			{
				wil::com_ptr<IShellFolder> pParentFolder;
				hr = pDesktopFolder->BindToObject(pidlParent, NULL, IID_IShellFolder, (void**)&pParentFolder);
				if (SUCCEEDED(hr))
				{
					// Get relative PIDL for the file within the parent directory
					LPITEMIDLIST pidlFile = NULL;
					std::wstring fileNameOnly = filename.substr(filename.find_last_of(L'\\') + 1);
					hr = pParentFolder->ParseDisplayName(hWnd, NULL, (LPWSTR)fileNameOnly.c_str(), NULL, &pidlFile, NULL);
					if (SUCCEEDED(hr))
					{
						LPCITEMIDLIST aPidls[] = { pidlFile };
						wil::com_ptr<IContextMenu> pContextMenu;
						hr = pParentFolder->GetUIObjectOf(hWnd, 1, aPidls, IID_IContextMenu, NULL, (void**)&pContextMenu);
						if (SUCCEEDED(hr))
						{
							HMENU hMenu = CreatePopupMenu();
							if (hMenu)
							{
								hr = pContextMenu->QueryContextMenu(hMenu, 0, 1, 0x7FFF, CMF_NORMAL);
								if (SUCCEEDED(hr))
								{
									UINT uCmd = TrackPopupMenuEx(hMenu, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, point.x, point.y, hWnd, NULL);
									
									if (uCmd != 0)
									{
										CMINVOKECOMMANDINFOEX ici = { sizeof(CMINVOKECOMMANDINFOEX) };
										ici.cbSize = sizeof(CMINVOKECOMMANDINFOEX);
										ici.fMask = CMIC_MASK_PTINVOKE;
										ici.hwnd = hWnd;
										ici.ptInvoke = point;
										ici.lpVerb = MAKEINTRESOURCEA(uCmd - 1);
										ici.nShow = SW_SHOWNORMAL;

										pContextMenu->InvokeCommand((LPCMINVOKECOMMANDINFO)&ici);
									}
								}
								DestroyMenu(hMenu);
							}
						}
						CoTaskMemFree(pidlFile);
					}
				}
				CoTaskMemFree(pidlParent);
			}
		}
		CoUninitialize();
	}
}
//------------------------------------------------------------------------
