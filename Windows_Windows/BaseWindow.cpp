//***********************************************
// WindowsWindows
// Copyright 2024 Michael Steuber
//===============================================
// BaseWindowTemplate.cpp
// ----------------------------------------------
// 08/12/2024 MS-24.01.03.04 - Removed invisible OS windows from windows list
// 07/25/2024 MS-24.0.02.04 - Added Window enumeration function to access all active windows 
// 07/25/2024 MS-24.01.01.01 created
//-----------------------------------------------
// Template for main window class
// 
// This template class is derived from Microsoft provided code is inherited by the WindowsApp class to provide key functionality. 
// Methods provided directly from Microsoft are denoted as such.
// 
// WindowProc Callback (MICROSOFT) - Serves as the jumping-off point for m_hwnd messages. 
//                                   Allows for tracking of the application state and passes messages to HandleMessage in WindowsApp
// Create (MICORSOFT) - Creates m_hwnd
// EnumWindowsProc - Window enumeration callback method, is called to create the vector of WindowControls
// WindowsVector - The vector of WindowControls
// m_hwnd - Main window handle

#pragma once
#include <Windows.h>
#include <string>
#include <sstream>
#include <vector>
#include "WindowControl.h"
#include "resource.h"

#define IDM_MENURESOURCE 1

template <class DERIVED_TYPE>
class BaseWindow
{
public:
    // BaseWindow WindowProc, allows for state management of m_hwnd. Provided by Microsoft.
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        DERIVED_TYPE* pThis = NULL;

        if (uMsg == WM_NCCREATE)
        {
            CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
            pThis = (DERIVED_TYPE*)pCreate->lpCreateParams;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

            pThis->m_hwnd = hwnd;
        }
        else
        {
            pThis = (DERIVED_TYPE*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        }
        if (pThis)
        {
            return pThis->HandleMessage(uMsg, wParam, lParam);
        }
        else
        {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }

    BaseWindow() : m_hwnd(NULL), m_hControlWindow(NULL) { }

    virtual ~BaseWindow() {}

    // Window creation function, called in WindowsApp
    BOOL Create(
        PCWSTR lpWindowName,
        DWORD dwStyle,
        DWORD dwExStyle = 0,
        int x = CW_USEDEFAULT,
        int y = CW_USEDEFAULT,
        int nWidth = CW_USEDEFAULT,
        int nHeight = CW_USEDEFAULT,
        HWND hWndParent = 0,
        HMENU hMenu = 0
    )
    {

        WNDCLASS wc = { 0 };

        wc.lpfnWndProc = DERIVED_TYPE::WindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = ClassName();
        wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);

        RegisterClass(&wc);

        m_hwnd = CreateWindowEx(
            dwExStyle, ClassName(), lpWindowName, dwStyle, x, y,
            nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(NULL), this
        );
     
        return (m_hwnd ? TRUE : FALSE);
    }

    HWND Window() const { return m_hwnd; } // Return m_hwnd

    HWND m_hControlWindow;

    HWND m_hActiveWindowsControlPanel; // The ActiveWindowControlPanel, declared here and created in WindowsApp

   /**
    * @brief Callback method to handle window enumeration and create the vector of WindowControls
    * 
    * Creates a pointer to a BaseWindow which is the instance of BaseWindow (or more likely a child of BaseWindow) that is calling the enumeration method.
    * It then checks the title and immediately returns if Program Manager, Windows Input Experience, Windows Shell Experience Host, the Windows Windows UI, 
    * or the Visual Studio window running Windows Windowsa are the current window, exluding them from the vector. 
	* The first 3 are always active even when their windows are closed and they disrupt core WinWin functionality by messing up the count,
	* WinWin is excluded to prevent it from being impacted by itself, and Visual Studio is only excluded if it is running WinWin for my own sanity during development.
    * It then creates a new instance of WindowControl and adds it to the WindowsVector.    
    * 
    * @param hwnd Current window in enumeration
    * @param lParam Stores user defined variables, in this case a pointer the instance of WindowsApp calling the enumeration function
    * @return Success code as BOOL
    */
   static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
        BaseWindow* app = reinterpret_cast<BaseWindow*>(lParam);
        WCHAR windowTitle[256];
        if (GetParent(hwnd) == NULL && IsWindowVisible(hwnd)) {
            LRESULT result = SendMessageTimeoutW(hwnd, WM_NULL, 0, 0, SMTO_ABORTIFHUNG, 1000, NULL);
            if (result == 0 || !IsWindowEnabled(hwnd)) { // Check if the window is able to receive/respond to messages
                return TRUE;
            }
            if (GetWindowText(hwnd, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0])) == 0) {
                return TRUE;
            }
            if ((std::wstring(windowTitle) == L"Program Manager" || std::wstring(windowTitle) == L"Windows Input Experience" || std::wstring(windowTitle) == L"Windows Shell Experience Host")) {
                return TRUE;
            }
            if (std::wstring(windowTitle) == L"Windows Window Extension Window" || std::wstring(windowTitle) == L"Windows_Windows (Running) - Microsoft Visual Studio") {
                return TRUE;
            }
            // ^ Exclude Program Manager, Windows Input Experience, Windows Shell Experience Host, the Windows Windows UI, and the Visual Studio window running Windows Windows from the vector
            oss << L"Window Handle: " << hwnd << L" Title: " << windowTitle << "\r\n"; // Format the title to put in the oss
            WindowHandle = hwnd;
            app->WindowsVector.push_back(new WindowControl( // Add a new WindowControl to the WindowVector
                app->m_hActiveWindowsControlPanel, 
                hwnd,
                windowTitle,
                (app->WindowsVector.size() == 1 ? 100 : (static_cast<int>(app->WindowsVector.size() * 100))))); // Set the position of the control to 100 times the size of the vector at the tiem of control creation
        }
        return TRUE;
    }

protected:
    // Declaration of main window class name
    virtual PCWSTR  ClassName() const = 0;

    // Declaration of HandleMessage method, defined in WindowsApp
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

    // Declaration of main window handle
    HWND m_hwnd;
    
    // Declaration of window handle, stores the currently enumerated window handle in the EnumWindowsProc fucntion
    static HWND WindowHandle;

    // Declaration of title ostringstream to be passed to WindowControls
    static std::wostringstream oss;

    // Declaration of WindowsVector
    std::vector<WindowControl*> WindowsVector;   
};