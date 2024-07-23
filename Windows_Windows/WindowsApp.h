//***********************************************
// WindowsWindows
// Copyright 2024 Accurate Tool Company, Inc.
//===============================================
// WindowsApp.h
// ----------------------------------------------
// 07/23/2024 MS-24.01.01.0 created
//-----------------------------------------------
// Main window header code

#pragma once
#include <Windows.h>
class WindowsApp {

public:
    WindowsApp(HINSTANCE hInstance);

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    BOOL Create(HINSTANCE hInstance);

    HWND Window();

    HWND m_hwnd;

private:
    virtual PCWSTR  ClassName() const;
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};