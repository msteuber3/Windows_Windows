//***********************************************
// WindowsWindows
// Copyright 2024 Michael Steuber
//===============================================
// WindowsControl.cpp
// ----------------------------------------------
// 07/31/2024 MS-24.01.02.06 Updated child window size for sub control windows
// 07/23/2024 MS-24.01.01.0 created
//-----------------------------------------------
// Control Window source code
// 
// Contains WindowControl class. WindowControls are the WinWin UI representation of the active windows, so each window has an associated window control 
// that WinWin operates on. The WindowsApp master file contains a vector of WindowControls which functions as a list of all of the user's active windows,
// and this list can be refreshed by displaying the window list. 
// Physically, window controls are each of the rectangles you see containing minimize, maximize, and close buttons when you display the window list.



#include "WindowControl.h"

#define MIN 1
#define MAX 2
#define CLOSE 3


WindowControl::WindowControl(HWND parent, HWND InstanceHandle, LPCWSTR oss, int x) : m_Minimize(NULL), m_Maximize(NULL), m_Close(NULL), m_hControlPanel(NULL){
    m_Parent = parent;
    m_oss = oss;
    m_InstanceHandle = InstanceHandle;
    m_x = x;
    Create();
}

void WindowControl::Create()
{
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = ControlPanelProc; // Set the window procedure function
    wc.hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongPtr(m_Parent, GWLP_HINSTANCE));
    wc.lpszClassName = TEXT("ControlPanelClass");
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);

    RegisterClass(&wc);

    m_hControlPanel = CreateWindowExW(
       0,
       wc.lpszClassName, 
       L"Control Panel",
       WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
       0,
       m_x + 30,
       280,
       100,
       m_Parent, 
       NULL, 
       wc.hInstance,
       NULL
   );

    m_Title = CreateWindowEx(
        0, TEXT("STATIC"), (LPCWSTR)m_oss,
        WS_CHILD | WS_VISIBLE,
        10, 10, 200, 30,
        m_hControlPanel, NULL, GetModuleHandle(NULL), NULL);

	m_Minimize = CreateWindowEx(
        0,            
        L"BUTTON", 
        L"-", 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 
        10, 50, 70, 30,
        m_hControlPanel,
        (HMENU)MIN,
        wc.hInstance,
        NULL);

    m_Maximize = CreateWindowEx(
        0,
        L"BUTTON",
        L"+",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        85, 50, 70, 30,
        m_hControlPanel,
        (HMENU)MAX,
        wc.hInstance,
        NULL);

    m_Close = CreateWindowExW(
        0,
        L"BUTTON",
        L"X",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        160, 50, 70, 30,
        m_hControlPanel,
        (HMENU)CLOSE,
        wc.hInstance,
        NULL);

    SetWindowLongPtrW(m_hControlPanel, GWLP_USERDATA, (LONG_PTR)(m_InstanceHandle));
    SetWindowLongPtrW(m_Minimize, GWLP_USERDATA, (LONG_PTR)(m_InstanceHandle));
    SetWindowLongPtrW(m_Maximize, GWLP_USERDATA, (LONG_PTR)(m_InstanceHandle));
    SetWindowLongPtrW(m_Close, GWLP_USERDATA, (LONG_PTR)(m_InstanceHandle));

}

int WindowControl::GetYPos(){
    RECT controlRect;
    GetWindowRect(m_hControlPanel, &controlRect);
    return controlRect.top;
}

HWND WindowControl::GetInstanceHandle()
{
    return this->m_InstanceHandle;
}

LPCWSTR WindowControl::GetInstanceTitle()
{
    return this->m_oss;
}

LRESULT CALLBACK WindowControl::ControlPanelProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_PAINT:
        PAINTSTRUCT ps;
        HDC hdc;
        RECT rect;
        GetClientRect((HWND)lParam, &rect);
        hdc = BeginPaint((HWND)lParam, &ps);
        SetTextColor(hdc, RGB(0, 0, 0));
        SetBkMode(hdc, TRANSPARENT);
        EndPaint((HWND)lParam, &ps);
        break;
    case WM_ERASEBKGND:
    {
        HDC hdc = (HDC)wParam;
        RECT rect;
        GetClientRect((HWND)lParam, &rect);
        HBRUSH hBrush = CreateSolidBrush(COLOR_WINDOW + 1);
        FillRect(hdc, &rect, hBrush);
        DeleteObject(hBrush);
        break;
    }
    case WM_COMMAND:
        if (HIWORD(wParam) == BN_CLICKED) {
            int id = LOWORD(wParam);
            HWND control = (HWND)lParam;
            HWND InstanceHandle = (HWND)GetWindowLongPtrW(control, GWLP_USERDATA);

            switch (id) {
            case MIN:
                ShowWindow(InstanceHandle, SW_MINIMIZE);
                UpdateWindow(InstanceHandle);
                break;
            case MAX:
                ShowWindow(InstanceHandle, SW_MAXIMIZE);
                UpdateWindow(InstanceHandle);
                break;
            case CLOSE:
                SendMessage(InstanceHandle, WM_CLOSE, 0, 0);
                break;
            }
        }
        break;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

