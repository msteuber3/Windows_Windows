//***********************************************
// WindowsWindows
// Copyright 2024 Accurate Tool Company, Inc.
//===============================================
// WindowsApp.cpp
// ----------------------------------------------
// 08/01/2024 MS-24.01.02.09 Added dropdown menu to hide active window list
// 08/01/2024 MS-24.01.02.08 Added save layout button (saves current layout in a json file)
// 08/01/2024 MS-24.01.02.07 Added stack windows button (broke scrolling again)
// 07/25/2024 MS-24.01.02.06 Fixed scrolling, cleaned up window destruction for better memory management, updated child window size for sub control windows
// 07/25/2024 MS-24.01.02.05 Added global oss to store active windows ostreamstring and added function to get all active windows to the create event
// 07/25/2024 MS-24.01.02.2 Updated to be compatible with template window
// 07/23/2024 MS-24.01.01.0 created
//-----------------------------------------------
// Main window source code

#include "WindowsApp.h"
#include <string>
#include <sstream>
#include <iostream>
#include <Psapi.h>

#define STACK 1
#define SAVE_LAYOUT 2
#define SHOW_ACTIVE_WINDOWS 3
#define HIDE_ACTIVE_WINDOWS 4

WindowsApp::WindowsApp() {}


std::wostringstream WindowsApp::oss; //ostream string that contains window control panel titles
HWND WindowsApp::WindowHandle; // Window handle that holds info for creation of the windows vector

void WindowsApp::RunMessageLoop() {
    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

PCWSTR WindowsApp::ClassName() const { return L"Windows Window Extension"; }

HRESULT WindowsApp::Initialize()
{
    HRESULT hr;
    // See BaseWindow.cpp
    hr = this->Create(L"Windows Window Extension Window",
        WS_OVERLAPPEDWINDOW | WS_VSCROLL,
        0,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        500,
        175
    );
    return hr;
}

LRESULT WindowsApp::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = 0;

    switch (uMsg) {
    case WM_CREATE:
        HandleCreate();
        break;
    case WM_PAINT:
        HandlePaint();
        break;
    case WM_SIZE:
        HandleResize();
        break;
    case WM_VSCROLL:
        HandleScroll(wParam);
        break;
    case WM_COMMAND:
        if (HIWORD(wParam) == BN_CLICKED) {
            int id = LOWORD(wParam);   //Extract button id from the wParam and dispatch accordingly
            int ControlY = ((WindowsVector.size() + 1) * 100) + 75;
            switch (id) {
            case STACK:
                StackWindows();
                break;
            case SAVE_LAYOUT:
                WinWinSaveLayout();
                break;
            case SHOW_ACTIVE_WINDOWS:
                  // Enumerate through active windows and create controls
                PrintActiveWindows();
                ControlY = ((WindowsVector.size() + 1) * 100) + 75;
                SetWindowPos(m_hControlWindow, NULL, 0, 500, 500, ControlY, SW_SHOWNORMAL); // resize control window
                SetWindowPos(m_hwnd, NULL, 0, 0, 500, ControlY, SWP_NOMOVE);
                DestroyWindow(m_hShowWindows);
                m_hHideWindows = CreateWindowEx(
                    0,
                    L"BUTTON",
                    L"^",
                    WS_TABSTOP | WS_VISIBLE | WS_CHILD,
                    0, 100, 400, 30,
                    m_hwnd,
                    (HMENU)HIDE_ACTIVE_WINDOWS,
                    (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
                    NULL);
                for (WindowControl* ctrl : WindowsVector) {
                    SetWindowPos(ctrl->m_hControlPanel, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
                }
                break;
            case HIDE_ACTIVE_WINDOWS:
                for (WindowControl* ctrl : WindowsVector) {
                    SetWindowPos(ctrl->m_hControlPanel, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW );
                }
                SetWindowPos(m_hControlWindow, NULL, 0, 0, 500, 175, SW_SHOWNORMAL | SW_INVALID); // resize control window
                SetWindowPos(m_hwnd, NULL, 0, 0, 500, 175, SWP_NOMOVE);
                m_hShowWindows = CreateWindowEx(
                    0,
                    L"BUTTON",
                    L"V",
                    WS_TABSTOP | WS_VISIBLE | WS_CHILD,
                    0, 100, 400, 30,
                    m_hwnd,
                    (HMENU)SHOW_ACTIVE_WINDOWS,
                    (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
                    NULL);
                DestroyWindow(m_hHideWindows);

                break;
            }
        }
        break;
    case WM_CLOSE:
        if (m_hwnd != NULL) {
            DestroyWindow(m_hwnd);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

void WindowsApp::PrintActiveWindows() {
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(this)); // Enumerate through the windows with this callback function. 
                                                                  //EnumWindowsProc is a custom method, see BaseWindow.cpp
}

void WindowsApp::StackWindows()
{   
    int stackPos = 10;
    for (WindowControl* ctrl : WindowsVector) { //Iterate through all windows in WindowsVector (all open windows)
        ShowWindow(ctrl->GetInstanceHandle(), SW_SHOWNORMAL);  // Set each window to normal mode (unmax/unmin)
        SetWindowPos(ctrl->GetInstanceHandle(), HWND_TOPMOST, stackPos, stackPos, 750, 750, NULL); // Bring current window to front
        SetWindowPos(ctrl->GetInstanceHandle(), HWND_NOTOPMOST, stackPos, stackPos, 750, 750, NULL); // Remove "TOPMOST" flag
        stackPos += 50; 
    }
}

void WindowsApp::WinWinSaveLayout()
{
    const char* WinWinLayoutsFile = "WinWinSavedLayouts.json";   // Name of json file (on same level as this code file)
    if (!std::filesystem::exists(WinWinLayoutsFile)) { // Check if it exists, create it if not
        std::ofstream{ WinWinLayoutsFile };
    }
    std::fstream LayFile;
    LayFile.open(WinWinLayoutsFile);
    WINDOWPLACEMENT pInstancePlacement;
    pInstancePlacement.length = sizeof(WINDOWPLACEMENT);  // Instantiate WINDOWPLACEMENT object
    nlohmann::json placeInfo; // Final json that is written to file
    nlohmann::basic_json placeInfoTemp; // Temp json, cleared after each run of the loop
    DWORD processId;
    WCHAR path[MAX_PATH] = L"";
    HANDLE hProcess;
    for (WindowControl* ctrl : WindowsVector) {
        GetWindowPlacement(ctrl->GetInstanceHandle(), &pInstancePlacement); // Get the hwnd of the current handle and extract placement details. 
                                                                            // Put into WINDOWPLACEMENT object 
        GetWindowThreadProcessId(ctrl->GetInstanceHandle(), &processId); 
        hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId); // Get executible associated with window handle
        GetModuleFileNameEx(hProcess, NULL, path, MAX_PATH);
        CloseHandle(hProcess);

        placeInfoTemp = { {"process", std::wstring(path).c_str()}, // Translate WINDOWPLACEMENT to json, stick the process in the front
            {"flags", pInstancePlacement.flags},
        {"showCmd", pInstancePlacement.showCmd },
        {"ptMinPosition",
            { {"x", pInstancePlacement.ptMinPosition.x },
             { "y",  pInstancePlacement.ptMinPosition.y } } },
        {"ptMaxPosition", 
             { { "x", pInstancePlacement.ptMaxPosition.x},
               { "y", pInstancePlacement.ptMaxPosition.y} } },
         {"rcNormalPosition",
             { { "left", pInstancePlacement.rcNormalPosition.left },
               { "right", pInstancePlacement.rcNormalPosition.right },
               { "top", pInstancePlacement.rcNormalPosition.top },
               { "bottom", pInstancePlacement.rcNormalPosition.bottom } } }
        };
        placeInfo.push_back(placeInfoTemp); //append to main json
        placeInfoTemp.clear();// clear temp
    }
    LayFile << placeInfo; // add to json file

    LayFile.close();
}

HRESULT WindowsApp::HandleCreate() {
    HRESULT hr = S_OK;
    if (m_hwnd && m_hwnd != 0) {
        ShowWindow(m_hwnd, SW_SHOWNORMAL);
        m_hControlWindow = CreateWindowEx( // Create window that holds all WindowControl control panels
            0, TEXT("STATIC"), NULL,
            WS_CHILD | WS_VISIBLE | SS_LEFT ,
            0, 0, 500, 9000, m_hwnd, NULL, (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE), NULL);

        UpdateWindow(m_hwnd);
       
    }
    CreateControlOpts(); // Create control buttons that are children of m_hwnd
    // Enumerate through active windows and create controls
    int ControlY = 400;
    SetWindowPos(m_hControlWindow, NULL, 0, 0, 500, ControlY, SW_SHOWNORMAL); // resize control window
    SCROLLINFO si; // Set scroll information
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_RANGE | SIF_PAGE;
    si.nMin = 0;
    si.nMax = ControlY;
    si.nPage = 1000;
    if (m_hwnd != 0) {
        SetScrollInfo(m_hwnd, SB_VERT, &si, TRUE);
    }

    return hr;
}

void WindowsApp::CreateControlOpts() {
    m_hStackButton = CreateWindowEx(
        0,
        L"BUTTON",
        L"STACK",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, 20, 110, 30,
        m_hwnd,
        (HMENU)STACK,
        (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
        NULL);

    m_hSaveWinLayout = CreateWindowEx(
        0,
        L"BUTTON",
        L"SAVE LAYOUT",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        120, 20, 110, 30,
        m_hwnd,
        (HMENU)SAVE_LAYOUT,
        (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
        NULL);

    m_hShowWindows = CreateWindowEx(
        0,
        L"BUTTON",
        L"V",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD,
        0, 100, 400, 30,
        m_hwnd,
        (HMENU)SHOW_ACTIVE_WINDOWS,
        (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
        NULL);

}

void WindowsApp::HandlePaint() {
    PAINTSTRUCT ps;
    HDC hdc;
    RECT rect;
    GetClientRect(m_hwnd, &rect);
    hdc = BeginPaint(m_hwnd, &ps);
    SetTextColor(hdc, RGB(0, 0, 0));
    SetBkMode(hdc, TRANSPARENT);
    EndPaint(m_hwnd, &ps);

}

void WindowsApp::HandleResize(){
    RECT resizeRect;
    if (m_hwnd != NULL) {
        GetWindowRect(m_hwnd, &resizeRect);
        SetWindowPos(m_hControlWindow, NULL, 0, 0, resizeRect.right, resizeRect.bottom, NULL);
        UpdateWindow(m_hwnd);
    }
}

void WindowsApp::HandleScroll(WPARAM wParam) {
    int scrollCode = LOWORD(wParam);
    int nPos;
    int nOldPos;
    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS;
    GetScrollInfo(m_hwnd, SB_VERT, &si);

    nOldPos = si.nPos;

    switch (scrollCode) {
        case SB_TOP:            
            nPos = si.nMin; 
            break;
        case SB_BOTTOM:         
            nPos = si.nMax; 
            break;
        case SB_LINEUP:
            nPos = si.nPos - 1;
            break;
        case SB_LINEDOWN:
            nPos = si.nPos + 1;
            break;
        case SB_THUMBPOSITION:
            nPos = si.nTrackPos;
            break;
        default:
        case SB_THUMBTRACK:
            nPos = si.nPos;
            break;
    }
    SetScrollPos(m_hwnd, SB_VERT, nPos, TRUE);

    nPos = GetScrollPos(m_hwnd, SB_VERT);

    ScrollWindowEx(m_hwnd, 0, (int)(nOldPos - nPos) * 1,
        NULL, NULL, NULL, NULL, SW_INVALIDATE | SW_SCROLLCHILDREN);



    UpdateWindow(m_hwnd);

}
