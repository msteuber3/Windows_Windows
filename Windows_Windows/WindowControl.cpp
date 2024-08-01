//***********************************************
// WindowsWindows
// Copyright 2024 Accurate Tool Company, Inc.
//===============================================
// WindowsControl.cpp
// ----------------------------------------------
// 07/31/2024 MS-24.01.02.06 Updated child window size for sub control windows - buttons now all work properly 
// 07/23/2024 MS-24.01.01.0 created
//-----------------------------------------------
// Control Window source code

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

    RegisterClass(&wc);

    m_hControlPanel = CreateWindowExW(
        0,
        wc.lpszClassName, // Use a static control or any other class
        L"Control Panel",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        0,
        75,
        1000,
        m_x + 100, // Position and size
        m_Parent, // Parent window handle
        NULL, // Menu handle (if any)
        wc.hInstance,
        NULL
    );

    m_Title = CreateWindowEx(
        0, TEXT("STATIC"), (LPCWSTR)m_oss,
        WS_CHILD | WS_VISIBLE,
        10, m_x + 10, 200, 30,
        m_hControlPanel, NULL, GetModuleHandle(NULL), NULL);

	m_Minimize = CreateWindowEx(
        0,            
        L"BUTTON", 
        L"-", 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 
        10, m_x + 50, 100, 30,
        m_hControlPanel,
        (HMENU)MIN,
        wc.hInstance,
        NULL);

    m_Maximize = CreateWindowEx(
        0,
        L"BUTTON",
        L"+",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        110, m_x+ 50, 100, 30,
        m_hControlPanel,
        (HMENU)MAX,
        wc.hInstance,
        NULL);

    m_Close = CreateWindowExW(
        0,
        L"BUTTON",
        L"X",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        210, m_x + 50, 100, 30,
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

LRESULT CALLBACK WindowControl::ControlPanelProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
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
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

