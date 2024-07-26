//***********************************************
// WindowsWindows
// Copyright 2024 Accurate Tool Company, Inc.
//===============================================
// WindowsApp.h
// ----------------------------------------------
// 07/25/2024 MS-24.01.02.1 Updated to be compatible with template window
// 07/23/2024 MS-24.01.01.0 created
//-----------------------------------------------
// Main window header code

#pragma once
#include <iostream>
#include <Windows.h>
#include "BaseWindow.cpp"

class WindowsApp : public BaseWindow<WindowsApp> {

public:
    WindowsApp();

    HRESULT Initialize();

    void RunMessageLoop();

    PCWSTR  ClassName() const;
    
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void PrintActiveWindows();


private:
    HRESULT HandleCreate();

    void HandlePaint();

    void HandleResize();
};