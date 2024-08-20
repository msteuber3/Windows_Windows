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
#include <nlohmann/json.hpp>
#include <fstream>
#include <locale>
#include <codecvt>
#include <filesystem>
#include <commctrl.h>
#include <string>
#include <sstream>
#include <Psapi.h>
#include "resource.h"

class WindowsApp : public BaseWindow<WindowsApp> {

public:
    WindowsApp();

    // Create main window (calls Create() function from BaseWindow.cpp
    HRESULT Initialize();

    // Starts the loop that process and dispatches main window messages 
    void RunMessageLoop();

    // Main window class name
    PCWSTR  ClassName() const;
    
    // Called by WindowProc callback function, processes commands 
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    // Enumerate through the windows open on the machine and create a control panel for each of them [[TEMPORARY]]
    void PrintActiveWindows();

    // Creates the main control panel to manage window layouts, stack windows, etc.
    void CreateControlOpts();

    // Called when STACK button is pressed
    void StackWindows();

    void StackWindowsCallback();

    void StackFourOrLess(std::vector<WindowControl*> SubVector);
    
    void StackFiveToEight(std::vector<WindowControl*> SubVector);

    void CascadeWindows();

    // Called when SAVE LAYOUT button is pressed. Saves the current layout to a json file
    void WinWinSaveLayout();

    void WinWinViewSaved();

    void ViewSavedDesktopLayouts();

    void ExecuteSaved(std::wstring json);

    void ExecuteSavedDesktopLayout(std::wstring json);

    struct SavedWindow {
        SavedWindow(std::string process,
            UINT flags,
            UINT showCmd,
            POINT ptMinPosition,
            POINT ptMaxPosition,
            RECT rcNormalPosition) {
            m_process = process;
            m_flags = flags;
            m_showCmd = showCmd;
            m_ptMinPosition = ptMinPosition;
            m_ptMaxPosition = ptMaxPosition;
            m_rcNormalPosition = rcNormalPosition;
        };
        std::string m_process;
        UINT m_flags;
        UINT m_showCmd;
        POINT m_ptMinPosition;
        POINT m_ptMaxPosition;
        RECT m_rcNormalPosition;
        WINDOWPLACEMENT* getWinPlacement() {
            WINDOWPLACEMENT placement;
            placement.showCmd = m_showCmd;
            placement.flags = m_flags;
            placement.length = sizeof(WINDOWPLACEMENT);
            placement.ptMaxPosition = m_ptMinPosition;
            placement.ptMaxPosition = m_ptMaxPosition;
            placement.rcNormalPosition = m_rcNormalPosition;
            return &placement;
        }
    };
    struct SavedIcon {
        SavedIcon(std::string iconName,
            POINT iconPos) {
            m_iconName = iconName;
            m_iconPos = iconPos;
        };
        std::string m_iconName;
        POINT m_iconPos;
    };

private:
    // Main control panel window handle
    HWND m_hControlOptions;

    HWND m_hCascadeButton;

    // Stack button window handle
    HWND m_hStackButton;

    HWND m_hNextStack;

    HWND m_hPrevStack;

    HWND m_hExitStack;

    // Save layout button window handle
    HWND m_hSaveWinLayout;

    HWND m_hShowWindows;

    HWND m_hHideWindows;

    HWND m_hSavedConfigs;

    HWND m_hHideSavedConfigs;

    HWND m_hSaveDesktopLayout;

    HWND m_hSavedDesktopConfigs;

    HWND m_hHideSavedDesktopConfigs;

    // Called upon window creation. Creates all child windows of m_hwnd
    HRESULT HandleCreate();

    // Called when the paint event is hit
    void HandlePaint();

    // Called on window resize
    void HandleResize();

    // Called on window scroll
    void HandleScroll(WPARAM wParam);

    void SaveDesktopLayout();

};