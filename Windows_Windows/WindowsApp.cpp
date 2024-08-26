//***********************************************
// WindowsWindows
// Copyright 2024 Accurate Tool Company, Inc.
//===============================================
// WindowsApp.cpp
// ----------------------------------------------
// 08/26/2024 MS-24.01.05.01 Reworked UI
// 08/20/2024 MS-24.01.04.02 Added preprocessor definition for closed main window height
// 08/19/2024 MS-24.01.04.01 Added ability to save layout configurations, fixed bugs in the stack function
// 08/16/2024 MS-24.01.03.06 Refactored
// 08/15/2024 MS-24.01.03.05 Fixed windows stack
// 08/13/2024 MS-24.01.03.04 Added (currently useless) menu, added (currently useless) save Desktop Icons button, fixed execute layout button
// 08/13/2024 MS-24.01.03.03 Fixed UI bugs
// 08/12/2024 MS-24.01.03.03 Added Windows sstack feature, renamed old stack to cascade
// 08/12/2024 MS-24.01.03.02 Fixed bugs surrounding printing active windows
// 08/12/2024 MS-24.01.03.01 Added functionality to executing saved layouts
// 08/01/2024 MS-24.01.02.10 Added controls for viewing saved layouts, updated json folder to include multiple files for different layouts
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


#define CASCADE 1
#define SAVE_LAYOUT 2
#define SHOW_ACTIVE_WINDOWS 3
#define HIDE_ACTIVE_WINDOWS 4
#define VIEW_SAVED_CONFIGS 5
#define EXECUTE_LAYOUT 6
#define HIDE_SAVED_CONFIGS 7
#define STACK 8
#define SAVE_DESKTOP_LAYOUT 9
#define NEXT_STACK 10
#define PREV_STACK 11
#define EXIT_STACK 12
#define SAVED_DESKTOP_LAYOUTS 13
#define HIDE_SAVED_DESKTOP_CONFIGS 14
#define EXECUTE_DESKTOP_LAYOUT 15
#define SQUISH 16

#define SHOW_ACTIVE_WINDOWS_BUTTON_Y 250
#define M_HWND_CLOSED_Y 355
#define WINDOWS_CONTROL_CLOSED_Y 150
#define ICON_WINDOW_CLOSED_Y 100


INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

WindowsApp::WindowsApp() {}

///   GLOBALS   ///
std::wostringstream WindowsApp::oss; //ostream string that contains window control panel titles
HWND WindowsApp::WindowHandle; // Window handle that holds info for creation of the windows vector
static std::wstring userInput;
std::vector<HWND> layoutButtons;
std::vector<HWND> desktopLayoutButtons;
PCWSTR WindowsApp::ClassName() const { return L"Windows Window Extension"; }
int stackIndex;
WNDPROC iconWindowProc;
WNDPROC windowWindowProc;
WNDPROC controlWindowProc;

///   CALLBACK FUNCTIONS   ///

void WindowsApp::RunMessageLoop() {
    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
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
            case CASCADE:
                CascadeWindows();
                break;
            case STACK:
                StackWindows();
                break;
            case NEXT_STACK:
                stackIndex++;
                StackWindowsCallback();
                break;
            case PREV_STACK:
                stackIndex--;
                StackWindowsCallback();
                break;
            case EXIT_STACK:
                ExitStack();
                break;
            case SAVE_LAYOUT:
                WinWinSaveLayout();
                break;
            case VIEW_SAVED_CONFIGS:
                WinWinViewSaved();
                break;
            case HIDE_SAVED_CONFIGS:
                WinWinHideSaved();
                break;
            case SHOW_ACTIVE_WINDOWS:
                WinWinShowActive();
                break;
            case HIDE_ACTIVE_WINDOWS:
                WinWinHideActive();
                break;
            case EXECUTE_LAYOUT:
                wchar_t jsonFile[256];
                GetWindowText((HWND)lParam, jsonFile, 256);
                ExecuteSaved(jsonFile);
                break;
            case SAVE_DESKTOP_LAYOUT:
                SaveDesktopLayout();
                break;
            case SAVED_DESKTOP_LAYOUTS:
                ViewSavedDesktopLayouts();
                break;
            case HIDE_SAVED_DESKTOP_CONFIGS:
                HideSavedDesktopLayouts();
                break;
            case EXECUTE_DESKTOP_LAYOUT:
                wchar_t desktopJsonFile[256];
                GetWindowText((HWND)lParam, desktopJsonFile, 256);
                ExecuteSavedDesktopLayout(desktopJsonFile);
                break;
            case SQUISH:
                SquishCascade();
                break;
}
        RECT mainWindowRect;
        GetWindowRect(m_hwnd, &mainWindowRect);
        SetWindowPos(m_hwnd, HWND_TOPMOST, mainWindowRect.left, mainWindowRect.top, mainWindowRect.right - mainWindowRect.left, mainWindowRect.bottom - mainWindowRect.top, SWP_NOMOVE);
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

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK: {
            WCHAR text[256];
            GetDlgItemText(hDlg, IDC_EDIT_TEXT, text, 256);
            userInput = text; // Store the input text in the global variable
            EndDialog(hDlg, IDOK);
            return TRUE;
        }
        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

LRESULT CALLBACK WindowsApp::ButtonProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_COMMAND) {
        SendMessage(GetParent(hwnd), uMsg, wParam, lParam);
        return 0; 
    }
    return CallWindowProc(iconWindowProc, hwnd, uMsg, wParam, lParam);

}


///   INITIALIZE   ///

HRESULT WindowsApp::Initialize()
{
    HRESULT hr;
    // See BaseWindow.cpp
    hr = this->Create(L"Windows Window Extension Window",
        WS_OVERLAPPEDWINDOW | WS_VSCROLL,
        0,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        300,
        M_HWND_CLOSED_Y
    );
    HMENU hMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU1));
    if (hMenu == NULL) {
        MessageBoxW(NULL, L"Failed to load menu.", L"Error", MB_OK | MB_ICONERROR);
        return 0;
    }
    SetMenu(m_hwnd, hMenu);
    return hr;
}

HRESULT WindowsApp::HandleCreate() {
    HRESULT hr = S_OK;
    if (m_hwnd && m_hwnd != 0) {
        ShowWindow(m_hwnd, SW_SHOWNORMAL);
        m_hActiveWindowsControlPanel = CreateWindowExW(
            0,
            L"STATIC",
            L"",
            WS_CHILD | WS_VISIBLE,
            0, SHOW_ACTIVE_WINDOWS_BUTTON_Y, 300, 50,
            m_hwnd,
            NULL,
            (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
            NULL);

        controlWindowProc = (WNDPROC)SetWindowLongPtr(m_hActiveWindowsControlPanel, GWLP_WNDPROC, (LONG_PTR)ButtonProc);

        UpdateWindow(m_hwnd);
       
    }
    CreateControlOpts(); // Create control buttons that are children of m_hwnd
    // Enumerate through active windows and create controls
    PrintActiveWindows();
    for (WindowControl* ctrl : WindowsVector) {
        SetWindowPos(ctrl->m_hControlPanel, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
    }
    int ControlY = 400;
    //SetWindowPos(m_hControlWindow, NULL, 0, 0, 500, ControlY, SW_SHOWNORMAL); // resize control window
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
    
    m_hIconControlPanel = CreateWindowEx(
        0,
        L"STATIC",
        L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        0,
        0,
        300,
        100,
        m_hwnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
        NULL
    );
    iconWindowProc = (WNDPROC)SetWindowLongPtr(m_hIconControlPanel, GWLP_WNDPROC, (LONG_PTR)ButtonProc);


     m_IconTitle = CreateWindowEx(
        0, TEXT("STATIC"), L"Desktop Icon Manager",
        WS_CHILD | WS_VISIBLE,
        60, 10, 200, 30,
         m_hIconControlPanel, NULL, GetModuleHandle(NULL), NULL);

    m_hSavedDesktopConfigs = CreateWindowEx(
        0,
        L"BUTTON",
        L"ICON LAYOUTS",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        150, 50, 110, 30,
        m_hIconControlPanel,
        (HMENU)SAVED_DESKTOP_LAYOUTS,
        (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
        NULL);
    m_hSaveDesktopLayout = CreateWindowEx(
        0,
        L"BUTTON",
        L"SAVE ICONS",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, 50, 110, 30,
        m_hIconControlPanel,
        (HMENU)SAVE_DESKTOP_LAYOUT,
        (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
        NULL
    );
    
    m_hWindowsControlPanel = CreateWindowExW(
        0,
        L"STATIC",
        L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        0,
        100,
        300,
        150,
        m_hwnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
        NULL
    );

    windowWindowProc = (WNDPROC)SetWindowLongPtr(m_hWindowsControlPanel, GWLP_WNDPROC, (LONG_PTR)ButtonProc);

    m_WindowTitle = CreateWindowEx(
            0, TEXT("STATIC"), L"Window Manager",
            WS_CHILD | WS_VISIBLE,
            75, 10, 200, 30,
        m_hWindowsControlPanel, NULL, GetModuleHandle(NULL), NULL);

    m_hCascadeButton = CreateWindowEx(
        0,
        L"BUTTON",
        L"CASCADE",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, 50, 110, 30,
        m_hWindowsControlPanel,
        (HMENU)CASCADE,
        (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
        NULL);

    m_hStackButton = CreateWindowEx(
        0,
        L"BUTTON",
        L"STACK",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        150, 50, 110, 30,
        m_hWindowsControlPanel,
        (HMENU)STACK,
        (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
        NULL);

    m_hSaveWinLayout = CreateWindowEx(
        0,
        L"BUTTON",
        L"SAVE LAYOUT",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        5, 100, 120, 30,
        m_hWindowsControlPanel,
        (HMENU)SAVE_LAYOUT,
        (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
        NULL);

    m_hSavedConfigs = CreateWindowEx(
        0,
        L"BUTTON",
        L"WINDOW LAYOUTS",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        135, 100, 140, 30,
        m_hWindowsControlPanel,
        (HMENU)VIEW_SAVED_CONFIGS,
        (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
        NULL);

  

    m_hShowWindows = CreateWindowEx(
        0,
        L"BUTTON",
        L"V",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD,
        10, 5, 270, 30,
        m_hActiveWindowsControlPanel,
        (HMENU)SHOW_ACTIVE_WINDOWS,
        (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
        NULL);
}

///   HANDLE BASIC WINDOW EVENTS   ///

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
  //      SetWindowPos(m_hControlWindow, NULL, 0, 0, resizeRect.right, resizeRect.bottom, NULL);
        UpdateWindow(m_hwnd);
    }
}

///   WINDOW RESOURCES   ///

std::wstring GetUserInput(HINSTANCE hInstance) {
    userInput.clear(); // Clear previous input
    if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_SIMPLE_INPUT_DIALOG), NULL, DialogProc) == IDOK) {
        return userInput;
    }
    return L"";
}

///   HANDLE WINDOWS_WINDOWS EVENTS   ///
 //   PRINT ACTIVE WINDOWS   //  

void WindowsApp::PrintActiveWindows() {
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(this)); // Enumerate through the windows with this callback function. 
                                                                  //EnumWindowsProc is a custom method, see BaseWindow.cpp
}

void WindowsApp::WinWinShowActive() {
    // Enumerate through active windows and create controls
    WindowsVector.clear();
    PrintActiveWindows();
    int ControlY = ((WindowsVector.size() + 1) * 100) + 102;
    RECT prevRect;
    GetWindowRect(m_hwnd, &prevRect);
    SetWindowPos(m_hwnd, NULL, 0, 0, prevRect.right - prevRect.left, ControlY, SWP_NOMOVE);
    RECT ActiveWinControl;
    GetWindowRect(m_hActiveWindowsControlPanel, &ActiveWinControl);
    POINT ActiveWinControlBottomLeft;
    ActiveWinControlBottomLeft.x = ActiveWinControl.left;
    ActiveWinControlBottomLeft.y = ActiveWinControl.bottom;
    ScreenToClient(m_hwnd, &ActiveWinControlBottomLeft);
    SetWindowPos(m_hActiveWindowsControlPanel, NULL, ActiveWinControlBottomLeft.x, ActiveWinControlBottomLeft.y - 30, ActiveWinControl.right - ActiveWinControl.left, ControlY + 30, SW_INVALIDATE); // resize control window
    
    DestroyWindow(m_hShowWindows);

    m_hHideWindows = CreateWindowExW(
        0,
        L"BUTTON",
        L"^",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD,
        10, 0, 270, 30,
        m_hActiveWindowsControlPanel,
        (HMENU)HIDE_ACTIVE_WINDOWS,
        (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
        NULL);

    for (WindowControl* ctrl : WindowsVector) {
        SetWindowPos(ctrl->m_hControlPanel, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    }
}

void WindowsApp::WinWinHideActive() {
    for (WindowControl* ctrl : WindowsVector) {
        SetWindowPos(ctrl->m_hControlPanel, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
    }
    UpdateWindow(m_hActiveWindowsControlPanel);

    RECT prevActiveWinControl;
    GetWindowRect(m_hActiveWindowsControlPanel, &prevActiveWinControl);

    RECT winControlRect;
    GetWindowRect(m_hWindowsControlPanel, &winControlRect);
    POINT winBottomLeft = { winControlRect.left, winControlRect.bottom };
    ScreenToClient(m_hwnd, &winBottomLeft);

    RECT m_hwndPrevRect;
    GetWindowRect(m_hwnd, &m_hwndPrevRect);

    SetWindowPos(m_hActiveWindowsControlPanel, NULL, 0, 0, prevActiveWinControl.right - prevActiveWinControl.left, 30, SW_INVALIDATE | SWP_NOMOVE);

    SetWindowPos(m_hwnd, NULL, 0, 0, m_hwndPrevRect.right - m_hwndPrevRect.left, winBottomLeft.y + 100, SWP_NOMOVE);

    DestroyWindow(m_hHideWindows);
    m_hShowWindows = CreateWindowEx(
        0,
        L"BUTTON",
        L"V",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD,
        10, 0, 270, 30,
        m_hActiveWindowsControlPanel,
        (HMENU)SHOW_ACTIVE_WINDOWS,
        (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
        NULL);
}

//   STACK WINDOWS   //

void WindowsApp::StackWindows()
{
    if (WindowsVector.size() <= 4) {
        StackFourOrLess(WindowsVector);
    }
    else if (WindowsVector.size() <= 8) {
        StackFiveToEight(WindowsVector);
    }
    else if (WindowsVector.size() > 8) {
       
        stackIndex = 0;
        StackWindowsCallback();
        DestroyWindow(m_hStackButton);
        m_hExitStack = CreateWindowEx(
            0,
            L"BUTTON",
            L"EXIT STACK",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            150, 50, 110, 30,
            m_hWindowsControlPanel,
            (HMENU)EXIT_STACK,
            (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
            NULL);
        m_hNextStack = CreateWindowEx(
            0,
            L"BUTTON",
            L"NEXT >",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD,
            210, 82, 75, 25,
            m_hWindowsControlPanel,
            (HMENU)NEXT_STACK,
            (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
            NULL);
        m_hPrevStack = CreateWindowEx(
            0,
            L"BUTTON",
            L"< PREV",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD,
            125, 82, 75, 25,
            m_hWindowsControlPanel,
            (HMENU)PREV_STACK,
            (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
            NULL);
        EnableWindow(m_hPrevStack, FALSE);
    }
}

void WindowsApp::StackWindowsCallback()
{
    int lastVector = WindowsVector.size() % 8;
    int subVectorCount = (floor(float(WindowsVector.size()) / float(8))) + (lastVector == 0 ? 0 : 1);
    (stackIndex == 0 ? EnableWindow(m_hPrevStack, FALSE) : EnableWindow(m_hPrevStack, TRUE));
    (stackIndex == subVectorCount - 1 ? EnableWindow(m_hNextStack, FALSE) : EnableWindow(m_hNextStack, TRUE));

    std::vector<WindowControl*> SubVector;
    for (int i = 0; i < 8; i++) {
        if (WindowsVector.size() <= (stackIndex * 8) + i) {
            break;
        }
        SubVector.push_back(WindowsVector[(stackIndex * 8) + i]);
    }
    if (SubVector.size() <= 4) {
        StackFourOrLess(SubVector);
    }
    else if (SubVector.size() <= 8) {
        StackFiveToEight(SubVector);
    }
    SubVector.clear();
}

void WindowsApp::StackFourOrLess(std::vector<WindowControl*> SubVector) {
    int stackFactorY;
    int stackPosy = 0;
    int i = 0;
    stackFactorY = GetSystemMetrics(SM_CYSCREEN) / SubVector.size();
    for (WindowControl* ctrl : SubVector) {
        ShowWindow(ctrl->GetInstanceHandle(), SW_RESTORE);
        SendMessage(ctrl->GetInstanceHandle(), WM_SYSCOMMAND, SC_RESTORE, 0);
        SetWindowPos(ctrl->GetInstanceHandle(), HWND_TOPMOST, 0, stackPosy, GetSystemMetrics(SM_CXSCREEN), stackFactorY, NULL);
        SetWindowPos(ctrl->GetInstanceHandle(), HWND_NOTOPMOST, 0, stackPosy, GetSystemMetrics(SM_CXSCREEN), stackFactorY, NULL);
        stackPosy += stackFactorY;
        i++;
    }
}

void WindowsApp::StackFiveToEight(std::vector<WindowControl*>SubVector) {
    int stackFactorYLeft;
    int stackFactorYRight;
    int stackPosy = 0;
    int i = 0;
    stackFactorYLeft = GetSystemMetrics(SM_CYSCREEN) / ceil(float(SubVector.size()) / float(2));
    stackFactorYRight = GetSystemMetrics(SM_CYSCREEN) / floor(float(SubVector.size()) / float(2));
    for (WindowControl* ctrl : SubVector) {
        ShowWindow(ctrl->GetInstanceHandle(), SW_RESTORE);
        SendMessage(ctrl->GetInstanceHandle(), WM_SYSCOMMAND, SC_RESTORE, 0);
        if (i >= ceil(float(SubVector.size()) / float(2))) {

            SetWindowPos(ctrl->GetInstanceHandle(), HWND_TOPMOST, GetSystemMetrics(SM_CXSCREEN) / 2, stackPosy, GetSystemMetrics(SM_CXSCREEN) / 2, stackFactorYRight, NULL);
            SetWindowPos(ctrl->GetInstanceHandle(), HWND_NOTOPMOST, GetSystemMetrics(SM_CXSCREEN) / 2, stackPosy, GetSystemMetrics(SM_CXSCREEN) / 2, stackFactorYRight, NULL);

            stackPosy += stackFactorYRight;
        }

        else {
            SetWindowPos(ctrl->GetInstanceHandle(), HWND_TOPMOST, 0, stackPosy, GetSystemMetrics(SM_CXSCREEN) / 2, stackFactorYLeft, NULL);
            SetWindowPos(ctrl->GetInstanceHandle(), HWND_NOTOPMOST, 0, stackPosy, GetSystemMetrics(SM_CXSCREEN) / 2, stackFactorYLeft, NULL);

            stackPosy += stackFactorYLeft;
        }
        i++;
        if (i == ceil(float(SubVector.size()) / float(2))) {
            stackPosy = 0;
        }
    }
}

void WindowsApp::ExitStack() {
    stackIndex = 0;
    DestroyWindow(m_hPrevStack);
    DestroyWindow(m_hNextStack);
    DestroyWindow(m_hExitStack);
    m_hStackButton = CreateWindowEx(
        0,
        L"BUTTON",
        L"STACK",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, 50, 110, 30,
        m_hwnd,
        (HMENU)STACK,
        (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
        NULL);
}

 //   CASCADE WINDOWS   //

void WindowsApp::CascadeWindows() {
    int stackPos = 10;
    for (WindowControl* ctrl : WindowsVector) { //Iterate through all windows in WindowsVector (all open windows)
        SendMessage(ctrl->GetInstanceHandle(), WM_SYSCOMMAND, SC_RESTORE, 0);
        ShowWindow(ctrl->GetInstanceHandle(), SW_SHOWNORMAL);  // Set each window to normal mode (unmax/unmin)
        SetWindowPos(ctrl->GetInstanceHandle(), HWND_TOPMOST, stackPos, stackPos, 750, 750, NULL); // Bring current window to front
        SetWindowPos(ctrl->GetInstanceHandle(), HWND_NOTOPMOST, stackPos, stackPos, 750, 750, NULL); // Remove "TOPMOST" flag
        stackPos += 50;
    }
    m_hSquish = CreateWindowEx(0,
        L"BUTTON",
        L"SQUISH",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        15, 185, 100, 20,
        m_hwnd,
        (HMENU)SQUISH,
        (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
        NULL);
}

void WindowsApp::SquishCascade() {
    int stackPos = 10;
    RECT windowRect;
    for (WindowControl* ctrl : WindowsVector) { //Iterate through all windows in WindowsVector (all open windows)
        GetWindowRect(ctrl->GetInstanceHandle(), &windowRect);
        if (windowRect.top == stackPos && windowRect.left == stackPos && windowRect.right - windowRect.left == 750 && windowRect.bottom - windowRect.top == 750) {
            ShowWindow(ctrl->GetInstanceHandle(), SW_MINIMIZE);
        }
        stackPos += 50;
    }
}

  //   SAVE WINDOW LAYOUTS   //

void WindowsApp::WinWinSaveLayout()
{

    std::wstring layoutName = GetUserInput((HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE));

    std::wstring WinWinLayoutsFile = L"SavedLayouts/" + layoutName + L".json";   // Name of json file (in SavedLayouts folder)

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
    RECT rect;
    for (WindowControl* ctrl : WindowsVector) {

        GetWindowPlacement(ctrl->GetInstanceHandle(), &pInstancePlacement); // Get the hwnd of the current handle and extract placement details. 
        if (!IsIconic(ctrl->GetInstanceHandle())) {                                                           // Put into WINDOWPLACEMENT object 
            GetWindowRect(ctrl->GetInstanceHandle(), &rect);
            pInstancePlacement.rcNormalPosition.right = rect.right;
            pInstancePlacement.rcNormalPosition.left = rect.left;
            pInstancePlacement.rcNormalPosition.top = rect.top;
            pInstancePlacement.rcNormalPosition.bottom = rect.bottom;
        }
        GetWindowThreadProcessId(ctrl->GetInstanceHandle(), &processId); 
        hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId); // Get executible associated with window handle
        GetModuleFileNameEx(hProcess, NULL, path, MAX_PATH);
        CloseHandle(hProcess);

        placeInfoTemp = { {"process", std::wstring(path).c_str()},// Translate WINDOWPLACEMENT to json, stick the process in the front
            {"minimized", IsIconic(ctrl->GetInstanceHandle()) },
            {"handle", int(ctrl->GetInstanceHandle())},
            {"title", std::wstring(ctrl->GetInstanceTitle()).c_str()},
            {"length", pInstancePlacement.length},
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

void WindowsApp::WinWinViewSaved() {
    std::wstring WinWinLayoutsFolder = L"SavedLayouts/";
    std::wstring layoutName;
    int xPos = 15;
    int yPos = 140;
    int i = 0;
    
    for (const auto& WinWinLayoutsFile : std::filesystem::directory_iterator(WinWinLayoutsFolder)) {
        i++;
        if (i >= 2) {
            xPos = 15;
            yPos += 30;
            i = 0;
        }
        else {
            xPos += 130;
        }
    }
    // 1. Get the position of the old WindowsControl rect
    // 2. Get the position of the old m_hwnd
    // 3. Get the position of the new WindowsControl rect
    //      a. x - 0
    //      b. y - 0 | SWP_NOMOVE
    //      c. cx - old WindowControlRectRight - old WindowControlRectLeft
    //      d. cy - yPos + 40 for last button + buffer
    // 4. set the position of the new m_hwnd
    //      a. x - 0
    //      b. y - 0 | SWP_NOMOVE
    //      c. cx - old m_hwnd right - old m_hwnd left
    //      d. cy - old m_hwnd bottom + yPos + 40
    // 5. set the position of the new ActiveWindowsRect
    //      a. x - 0
    //      b. y - Bottom of new WindowsControl rect
    //      c. cx - 0
    //      d. cy - 0 | SWP_NOSIZE
    // 6. Set the position of the new WindowsControl rect
    

    RECT m_WindowControlPrevRect;
    GetWindowRect(m_hWindowsControlPanel, &m_WindowControlPrevRect);

    RECT m_hwndPrevRect;
    GetWindowRect(m_hwnd, &m_hwndPrevRect);

    POINT WindowControlTopLeft = { m_WindowControlPrevRect.left, m_WindowControlPrevRect.top };
    ScreenToClient(m_hwnd, &WindowControlTopLeft);

    int newWindowControlCX = m_WindowControlPrevRect.right - m_WindowControlPrevRect.left;
    int newWindowControlCY = yPos + 40;

    int windowControlDiff = newWindowControlCY - (m_WindowControlPrevRect.bottom - m_WindowControlPrevRect.top);

    SetWindowPos(m_hwnd, NULL, 0, 0, m_hwndPrevRect.right - m_hwndPrevRect.left, (m_hwndPrevRect.bottom - m_hwndPrevRect.top) + windowControlDiff, SWP_NOMOVE);
        
    SetWindowPos(m_hActiveWindowsControlPanel, NULL, 0, WindowControlTopLeft.y + newWindowControlCY, 0, 0, SWP_NOSIZE);

    SetWindowPos(m_hWindowsControlPanel, NULL, 0, 0, newWindowControlCX, newWindowControlCY, SWP_NOMOVE);

    xPos = 15;
    yPos = 140;
    i = 0;
    for (const auto& WinWinLayoutsFile : std::filesystem::directory_iterator(WinWinLayoutsFolder)) {
        layoutName = WinWinLayoutsFile.path().filename();
        layoutButtons.push_back(CreateWindowEx(
            0,
            L"BUTTON",
            layoutName.erase(layoutName.length() - 5).c_str(),
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            xPos, yPos, 130, 30,
            m_hWindowsControlPanel,
            (HMENU)EXECUTE_LAYOUT,
            (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
            NULL));
        i++;
        if (i >= 2) {
            xPos = 15;
            yPos += 30;
            i = 0;
        }
        else {
            xPos += 130;
        }

    }
    
    DestroyWindow(m_hSavedConfigs);
    m_hHideSavedConfigs = CreateWindowEx(
        0,
        L"BUTTON",
        L"^",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        135, 100, 140, 30,
        m_hWindowsControlPanel,
        (HMENU)HIDE_SAVED_CONFIGS,
        (HINSTANCE)GetWindowLongPtr(m_hWindowsControlPanel, GWLP_HINSTANCE),
        NULL);
}

void WindowsApp::WinWinHideSaved() {
    RECT prevRect3;
    GetWindowRect(m_hwnd, &prevRect3);
    for (HWND layoutButton : layoutButtons) {
        DestroyWindow(layoutButton);
    }
    DestroyWindow(m_hHideSavedConfigs);
    m_hSavedConfigs = CreateWindowExW(
        0,
        L"BUTTON",
        L"WINDOW LAYOUTS",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        135, 100, 140, 30,
        m_hWindowsControlPanel,
        (HMENU)VIEW_SAVED_CONFIGS,
        (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
        NULL);


    RECT m_WindowControlPrevRect;
    GetWindowRect(m_hWindowsControlPanel, &m_WindowControlPrevRect);

    RECT m_hwndPrevRect;
    GetWindowRect(m_hwnd, &m_hwndPrevRect);

    POINT WindowControlTopLeft = { m_WindowControlPrevRect.left, m_WindowControlPrevRect.top };
    ScreenToClient(m_hwnd, &WindowControlTopLeft);

    int newWindowControlCX = m_WindowControlPrevRect.right - m_WindowControlPrevRect.left;
    int newWindowControlCY = WINDOWS_CONTROL_CLOSED_Y;

    int windowControlDiff = newWindowControlCY - (m_WindowControlPrevRect.bottom - m_WindowControlPrevRect.top);

    SetWindowPos(m_hwnd, NULL, 0, 0, m_hwndPrevRect.right - m_hwndPrevRect.left, (m_hwndPrevRect.bottom - m_hwndPrevRect.top) + windowControlDiff, SWP_NOMOVE);

    SetWindowPos(m_hActiveWindowsControlPanel, NULL, 0, WindowControlTopLeft.y + newWindowControlCY, 0, 0, SWP_NOSIZE);

    SetWindowPos(m_hWindowsControlPanel, NULL, 0, 0, newWindowControlCX, newWindowControlCY, SWP_NOMOVE);
}

void WindowsApp::ExecuteSaved(std::wstring json) {
    std::wstring jsonFile = L"SavedLayouts/" + json + L".json";
    if (!std::filesystem::exists(jsonFile)) { // Check if it exists
        return;
    }

    // So here I want to get every layout from the json file, check which processess are open, put the matching ones in the right place, and minimize the others
    // So from the json I need a vector of structs 
    // And I have my vector of open windows
    // Iterate over open windows and then iterate over vector of structs
    std::vector<SavedWindow*> SavedWindows;
  
    DWORD processId;
    WCHAR path[MAX_PATH] = L"";
    HANDLE hProcess;

    std::fstream LayFile;
    LayFile.open(jsonFile, std::ios::in);
    nlohmann::json Doc{ nlohmann::json::parse(LayFile) };
    int i = 0;
    for (auto& window : Doc.items()) {
        std::string process = Doc[i].at("process");
        UINT flags = Doc[i].at("flags");
        UINT length = Doc[i].at("length");
        UINT showCmd = Doc[i].at("showCmd");
        POINT minPos;
        UINT minPosX = Doc[i].at("ptMinPosition").at("x");
        UINT minPosY = Doc[i].at("ptMinPosition").at("y");
        minPos.x = minPosX;
        minPos.y = minPosY;
        POINT maxPos;
        UINT maxPosX = Doc[i].at("ptMaxPosition").at("x");
        UINT maxPosY = Doc[i].at("ptMaxPosition").at("y");
        maxPos.x = maxPosX;
        maxPos.y = maxPosY;
        RECT rcNormalPos;
        rcNormalPos.left = Doc[i].at("rcNormalPosition").at("left");
        rcNormalPos.right = Doc[i].at("rcNormalPosition").at("right");
        rcNormalPos.top = Doc[i].at("rcNormalPosition").at("top");
        rcNormalPos.bottom = Doc[i].at("rcNormalPosition").at("bottom");

        SavedWindows.push_back(new SavedWindow(
            process,
            flags,
            showCmd,
            minPos,
            maxPos,
            rcNormalPos));
        i++;
    }
    for (WindowControl* ctrl : WindowsVector) {
        GetWindowThreadProcessId(ctrl->GetInstanceHandle(), &processId);
        hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId); // Get executible associated with window handle
        GetModuleFileNameEx(hProcess, NULL, path, MAX_PATH);
        CloseHandle(hProcess);
        for (SavedWindow* window : SavedWindows) {
            int wideStrSize = MultiByteToWideChar(CP_UTF8, 0, window->m_process.c_str(), -1, nullptr, 0);
            std::wstring convertedWideStr(wideStrSize, L'/0');
            MultiByteToWideChar(CP_UTF8, 0, window->m_process.c_str(), -1, &convertedWideStr[0], wideStrSize);
            convertedWideStr.erase(std::remove(convertedWideStr.begin(), convertedWideStr.end(), L'\0'), convertedWideStr.end());
            std::wstring pathWstring(path);
            if (pathWstring == convertedWideStr) {
                WINDOWPLACEMENT placement;
                placement.length = sizeof(WINDOWPLACEMENT);
                placement.showCmd = window->m_showCmd;
                placement.flags = window->m_flags;
                placement.ptMaxPosition = window->m_ptMinPosition;
                placement.ptMaxPosition = window->m_ptMaxPosition;
                placement.rcNormalPosition = window->m_rcNormalPosition;

                SetWindowPlacement(ctrl->GetInstanceHandle(), &placement);
            }
        }
    }
    
}

 //   SAVE DESKTOP ICON LAYOUTS   //

void WindowsApp::SaveDesktopLayout()
{
    std::wstring layoutName = GetUserInput((HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE));

    std::wstring WinWinLayoutsFile = L"SavedDesktopLayouts/" + layoutName + L".json";   // Name of json file (in SavedLayouts folder)

    if (!std::filesystem::exists(WinWinLayoutsFile)) { // Check if it exists, create it if not
        std::ofstream{ WinWinLayoutsFile };
    }

    HWND hProgMan = FindWindow(L"Progman", NULL);
    HWND hShellView = FindWindowEx(hProgMan, NULL, L"SHELLDLL_DefView", NULL);
    FindWindowEx(hShellView, NULL, L"SysListView32", NULL);

    HWND hDesktopListView = FindWindowEx(hShellView, NULL, L"SysListView32", NULL);
     int itemCount = ListView_GetItemCount(hDesktopListView);
    DWORD pid = 0;
    GetWindowThreadProcessId(hDesktopListView, &pid);
    HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, pid);

    // Allocate memory in the target process
    LPPOINT pt = (LPPOINT)VirtualAllocEx(hProcess, NULL, sizeof(POINT), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    LVITEM* lvItem = (LVITEM*)VirtualAllocEx(hProcess, NULL, sizeof(LVITEM), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    LPWSTR itemText = (LPWSTR)VirtualAllocEx(hProcess, NULL, 256 * sizeof(WCHAR), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    WCHAR itemName[256] = L"";

    POINT iconPos;
    SIZE_T numRead;

    std::fstream LayFile;
    LayFile.open(WinWinLayoutsFile);

    nlohmann::json placeInfo; // Final json that is written to file
    nlohmann::basic_json placeInfoTemp; // Temp json, cleared after each run of the loop

    for (int i = 0; i < itemCount; ++i) {
        if (!ListView_GetItemPosition(hDesktopListView, i, pt)) {
            wprintf(L"Failed to get position for item %d\n", i);
            continue;
        }
        ReadProcessMemory(hProcess, pt, &iconPos, sizeof(POINT), &numRead);

        LVITEM lvi = { 0 };
        lvi.iSubItem = 0;
        lvi.cchTextMax = 256;
        lvi.pszText = itemText;

        // Write LVITEM to the target process
        WriteProcessMemory(hProcess, lvItem, &lvi, sizeof(LVITEM), NULL);

        // Retrieve the item text
        SendMessage(hDesktopListView, LVM_GETITEMTEXT, (WPARAM)i, (LPARAM)lvItem);

        // Read the item text from the target process
        ReadProcessMemory(hProcess, itemText, itemName, 256 * sizeof(WCHAR), &numRead);
        placeInfoTemp = { {"icon", std::wstring(itemName).c_str()},// Translate WINDOWPLACEMENT to json, stick the process in the front
            {"position", {{"x", std::to_string(iconPos.x)}, {"y",  std::to_string(iconPos.y)}}} };

        placeInfo.push_back(placeInfoTemp); //append to main json
        placeInfoTemp.clear();// clear temp
    }
    VirtualFreeEx(hProcess, pt, 0, MEM_RELEASE);
    VirtualFreeEx(hProcess, lvItem, 0, MEM_RELEASE);
    VirtualFreeEx(hProcess, itemText, 0, MEM_RELEASE);
    CloseHandle(hProcess);
    LayFile << placeInfo; // add to json file

    LayFile.close();
}

void WindowsApp::ViewSavedDesktopLayouts() {
    std::wstring WinWinLayoutsFolder = L"SavedDesktopLayouts/";
    std::wstring layoutName;
    int xPos = 15;
    int yPos = 100;
    int i = 0;

    for (const auto& WinWinLayoutsFile : std::filesystem::directory_iterator(WinWinLayoutsFolder)) {
        i++;
        if (i >= 2) {
            xPos = 15;
            yPos += 30;
            i = 0;
        }
        else {
            xPos += 130;
        }
    }

    RECT m_IconControlPrevRect;
    GetWindowRect(m_hIconControlPanel, &m_IconControlPrevRect);

    int newWindowControlCX = m_IconControlPrevRect.right - m_IconControlPrevRect.left;
    int newWindowControlCY = yPos + 40;

    int windowControlDiff = newWindowControlCY - (m_IconControlPrevRect.bottom - m_IconControlPrevRect.top);

    POINT IconControlTopLeft = { m_IconControlPrevRect.left, m_IconControlPrevRect.top };
    ScreenToClient(m_hwnd, &IconControlTopLeft);

    RECT ActiveWindowsPrevRect;
    GetWindowRect(m_hActiveWindowsControlPanel, &ActiveWindowsPrevRect);
    POINT ActiveWindowsTopLeft = { ActiveWindowsPrevRect.left, ActiveWindowsPrevRect.top };
    ScreenToClient(m_hwnd, &ActiveWindowsTopLeft);

    RECT m_WindowControlPrevRect;
    GetWindowRect(m_hWindowsControlPanel, &m_WindowControlPrevRect);

    RECT m_hwndPrevRect;
    GetWindowRect(m_hwnd, &m_hwndPrevRect);

    SetWindowPos(m_hwnd, NULL, 0, 0, m_hwndPrevRect.right - m_hwndPrevRect.left, (m_hwndPrevRect.bottom - m_hwndPrevRect.top) + windowControlDiff, SWP_NOMOVE);

    SetWindowPos(m_hActiveWindowsControlPanel, NULL, 0, ActiveWindowsTopLeft.y + windowControlDiff, 0, 0, SWP_NOSIZE);
    
    SetWindowPos(m_hWindowsControlPanel, NULL, 0, IconControlTopLeft.y + newWindowControlCY, 0, 0, SWP_NOSIZE);

    SetWindowPos(m_hIconControlPanel, NULL, 0, 0, newWindowControlCX, newWindowControlCY, SWP_NOMOVE);
   

    xPos = 15;
    yPos = 100;
    i = 0;
    for (const auto& WinWinLayoutsFile : std::filesystem::directory_iterator(WinWinLayoutsFolder)) {
        layoutName = WinWinLayoutsFile.path().filename();
        desktopLayoutButtons.push_back(CreateWindowEx(
            0,
            L"BUTTON",
            layoutName.erase(layoutName.length() - 5).c_str(),
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            xPos, yPos, 130, 30,
            m_hIconControlPanel,
            (HMENU)EXECUTE_DESKTOP_LAYOUT,
            (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
            NULL));
        i++;
        if (i >= 2) {
            xPos = 15;
            yPos += 30;
            i = 0;
        }
        else {
            xPos += 130;
        }
    }
    DestroyWindow(m_hSavedDesktopConfigs);
    m_hHideSavedDesktopConfigs = CreateWindowEx(
        0,
        L"BUTTON",
        L"^",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        150, 50, 110, 30,
        m_hIconControlPanel,
        (HMENU)HIDE_SAVED_DESKTOP_CONFIGS,
        (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
        NULL);

  

    }

void WindowsApp::HideSavedDesktopLayouts() {
    RECT prevRect6;
    GetWindowRect(m_hwnd, &prevRect6);
    for (HWND layoutButton : desktopLayoutButtons) {
        DestroyWindow(layoutButton);
    }
    DestroyWindow(m_hHideSavedDesktopConfigs);
    m_hSavedDesktopConfigs = CreateWindowExW(
        0,
        L"BUTTON",
        L"LAYOUTS",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        150, 50, 110, 30,
        m_hwnd,
        (HMENU)SAVED_DESKTOP_LAYOUTS,
        (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
        NULL);

    RECT m_IconControlPrevRect;
    GetWindowRect(m_hIconControlPanel, &m_IconControlPrevRect);

    int newWindowControlCX = m_IconControlPrevRect.right - m_IconControlPrevRect.left;
    int newWindowControlCY = ICON_WINDOW_CLOSED_Y;

    int windowControlDiff = newWindowControlCY - (m_IconControlPrevRect.bottom - m_IconControlPrevRect.top);

    POINT IconControlTopLeft = { m_IconControlPrevRect.left, m_IconControlPrevRect.top };
    ScreenToClient(m_hwnd, &IconControlTopLeft);

    RECT ActiveWindowsPrevRect;
    GetWindowRect(m_hActiveWindowsControlPanel, &ActiveWindowsPrevRect);
    POINT ActiveWindowsTopLeft = { ActiveWindowsPrevRect.left, ActiveWindowsPrevRect.top };
    ScreenToClient(m_hwnd, &ActiveWindowsTopLeft);

    RECT m_WindowControlPrevRect;
    GetWindowRect(m_hWindowsControlPanel, &m_WindowControlPrevRect);

    RECT m_hwndPrevRect;
    GetWindowRect(m_hwnd, &m_hwndPrevRect);

    SetWindowPos(m_hwnd, NULL, 0, 0, m_hwndPrevRect.right - m_hwndPrevRect.left, (m_hwndPrevRect.bottom - m_hwndPrevRect.top) + windowControlDiff, SWP_NOMOVE);

    SetWindowPos(m_hIconControlPanel, NULL, 0, 0, newWindowControlCX, newWindowControlCY, SWP_NOMOVE);

    SetWindowPos(m_hWindowsControlPanel, NULL, 0, IconControlTopLeft.y + newWindowControlCY, 0, 0, SWP_NOSIZE);

    SetWindowPos(m_hActiveWindowsControlPanel, NULL, 0, ActiveWindowsTopLeft.y + windowControlDiff, 0, 0, SWP_NOSIZE);
}

void WindowsApp::ExecuteSavedDesktopLayout(std::wstring json) {
    std::wstring jsonFile = L"SavedDesktopLayouts/" + json + L".json";
    if (!std::filesystem::exists(jsonFile)) { // Check if it exists
        return;
    }
    std::vector<SavedIcon*> SavedIcons;

    HWND hProgMan = FindWindow(L"Progman", NULL);
    HWND hShellView = FindWindowEx(hProgMan, NULL, L"SHELLDLL_DefView", NULL);
    FindWindowEx(hShellView, NULL, L"SysListView32", NULL);

    HWND hDesktopListView = FindWindowEx(hShellView, NULL, L"SysListView32", NULL);
    int itemCount = ListView_GetItemCount(hDesktopListView);
    DWORD pid = 0;
    GetWindowThreadProcessId(hDesktopListView, &pid);
    HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, pid);

    // Allocate memory in the target process
    LPPOINT pt = (LPPOINT)VirtualAllocEx(hProcess, NULL, sizeof(POINT), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    LVITEM* lvItem = (LVITEM*)VirtualAllocEx(hProcess, NULL, sizeof(LVITEM), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    LPWSTR itemText = (LPWSTR)VirtualAllocEx(hProcess, NULL, 256 * sizeof(WCHAR), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    WCHAR itemName[256] = L"";

    POINT iconPos;
    SIZE_T numRead;
    POINT savedIconPos;

    std::fstream LayFile;
    LayFile.open(jsonFile, std::ios::in);
    nlohmann::json Doc{ nlohmann::json::parse(LayFile) };
    int i = 0;
    for (auto& window : Doc.items()) {
        std::string iconName = Doc[i].at("icon");
        std::string iconPosXStr = Doc[i].at("position").at("x");
        UINT iconPosX = std::stoi(iconPosXStr);
        std::string iconPosYStr = Doc[i].at("position").at("y");
        UINT iconPosY = std::stoi(iconPosYStr);
        savedIconPos.x = iconPosX;
        savedIconPos.y = iconPosY;
        
        SavedIcons.push_back(new SavedIcon(iconName, savedIconPos));
        i++;
    }
    for (int i = 0; i < itemCount; ++i) {
        if (!ListView_GetItemPosition(hDesktopListView, i, pt)) {
            wprintf(L"Failed to get position for item %d\n", i);
        }
        ReadProcessMemory(hProcess, pt, &iconPos, sizeof(POINT), &numRead);


        LVITEM lvi = { 0 };
        lvi.iSubItem = 0;
        lvi.cchTextMax = 256;
        lvi.pszText = itemText;

        // Write LVITEM to the target process
        WriteProcessMemory(hProcess, lvItem, &lvi, sizeof(LVITEM), NULL);

        // Retrieve the item text
        SendMessageW(hDesktopListView, LVM_GETITEMTEXT, (WPARAM)i, (LPARAM)lvItem);

        // Read the item text from the target process
        ReadProcessMemory(hProcess, itemText, itemName, 256 * sizeof(WCHAR), &numRead);

        for (SavedIcon* icon : SavedIcons) {
            int wideStrSize = MultiByteToWideChar(CP_UTF8, 0, icon->m_iconName.c_str(), -1, nullptr, 0);
            std::wstring convertedWideStr(wideStrSize, L'/0');
            MultiByteToWideChar(CP_UTF8, 0, icon->m_iconName.c_str(), -1, &convertedWideStr[0], wideStrSize);
            convertedWideStr.erase(std::remove(convertedWideStr.begin(), convertedWideStr.end(), L'\0'), convertedWideStr.end());
            std::wstring pathWstring(itemName);
            if (pathWstring == convertedWideStr) {

                int itemIndex = i;
                SendMessage(hDesktopListView, LVM_SETITEMPOSITION, (WPARAM)itemIndex, MAKELPARAM(icon->m_iconPos.x, icon->m_iconPos.y));
                break;
                itemName[0] = L'\0';
            }
        }
    }
    VirtualFreeEx(hProcess, pt, 0, MEM_RELEASE);
    VirtualFreeEx(hProcess, lvItem, 0, MEM_RELEASE);
    VirtualFreeEx(hProcess, itemText, 0, MEM_RELEASE);
    CloseHandle(hProcess);
}
