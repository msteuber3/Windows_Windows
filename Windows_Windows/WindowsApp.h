//***********************************************
// WindowsWindows
// Copyright 2024 Accurate Tool Company, Inc.
//===============================================
// WindowsApp.h
// ----------------------------------------------
// 07/25/2024 MS-24.01.02.06 Added PrintActiveWindows() 
// 07/25/2024 MS-24.01.02.01 Updated to be compatible with template window
// 07/23/2024 MS-24.01.01.00 created
//-----------------------------------------------
// Main window header code

#pragma once
#include <iostream>
#include <Windows.h>
#include <vector>
#include "BaseWindow.cpp"
#include "WindowControl.h"
#include <shellapi.h>

class WindowsApp : public BaseWindow<WindowsApp> {

public:
    WindowsApp();

    HRESULT Initialize();

    void RunMessageLoop();

    PCWSTR  ClassName() const;
    
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void PrintActiveWindows();

    void CreateControlOpts();

    void StackWindows();

private:
    HWND m_hControlOptions;

    HWND m_hStackButton;

    HRESULT HandleCreate();

    void HandlePaint();

    void HandleResize();

    void HandleScroll(WPARAM wParam);

};