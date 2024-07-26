//***********************************************
// WindowsWindows
// Copyright 2024 Accurate Tool Company, Inc.
//===============================================
// WindowsApp.cpp
// ----------------------------------------------
// 07/25/2024 MS-24.01.02.05 Added global oss to store active windows ostreamstring and added function to get all active windows to the create event
// 07/25/2024 MS-24.01.02.2 Updated to be compatible with template window
// 07/23/2024 MS-24.01.01.0 created
//-----------------------------------------------
// Main window source code

#include "WindowsApp.h"
#include <string>
#include <sstream>
#include <iostream>


WindowsApp::WindowsApp() {}


std::wostringstream WindowsApp::oss;
HWND WindowsApp::WindowHandle;

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
    hr = this->Create(L"Windows Window Extension Window",
        WS_OVERLAPPEDWINDOW,
        0,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT
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
    case WM_CLOSE:
        DestroyWindow(m_hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

void WindowsApp::PrintActiveWindows() {
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(this));
    
    //WindowsVector.push_back(new WindowControl(m_hStaticControl, WindowHandle, oss.str().c_str(), ((int)WindowsVector.size() + 100)));
}

HRESULT WindowsApp::HandleCreate() {
    HRESULT hr = S_OK;
    if (m_hwnd && m_hwnd != 0) {
        ShowWindow(m_hwnd, SW_SHOWNORMAL);
        UpdateWindow(m_hwnd);
        m_hStaticControl = CreateWindowEx(
            0, TEXT("STATIC"), NULL,
            WS_CHILD | WS_VISIBLE | SS_LEFT | WS_VSCROLL,
            0, 0, 500, 9000, m_hwnd, NULL, GetModuleHandle(NULL), NULL);
    }
    PrintActiveWindows();


    return hr;
}

void WindowsApp::HandlePaint() {
    UpdateWindow(m_hwnd);
}

void WindowsApp::HandleResize(){
    UpdateWindow(m_hwnd);
}
