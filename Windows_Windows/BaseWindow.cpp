//***********************************************
// WindowsWindows
// Copyright 2024 Accurate Tool Company, Inc.
//===============================================
// BaseWindowTemplate.cpp
// ----------------------------------------------
// 07/25/2024 MS-24.0.02.04 - Added Window enumeration function to access all active windows 
// 07/25/2024 MS-24.01.01.01 created
//-----------------------------------------------
// Template for all window classes so that you can manage the state of the application through the WindowProc callback function
// 07/25/2024 - MICROSOFT PROVIDED CODE

#pragma once
#include <Windows.h>
#include <string>
#include <sstream>
#include <vector>
#include "WindowControl.h"

template <class DERIVED_TYPE>
class BaseWindow
{
public:
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

    BaseWindow() : m_hwnd(NULL), m_hStaticControl(NULL) { }

    virtual ~BaseWindow() {}

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

        RegisterClass(&wc);

        m_hwnd = CreateWindowEx(
            dwExStyle, ClassName(), lpWindowName, dwStyle, x, y,
            nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(NULL), this
        );

        return (m_hwnd ? TRUE : FALSE);
    }

    HWND Window() const { return m_hwnd; }


    static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
        BaseWindow* app = reinterpret_cast<BaseWindow*>(lParam);
        WCHAR windowTitle[256];
        if (GetParent(hwnd) == NULL && IsWindowVisible(hwnd)) {
            if (GetWindowText(hwnd, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0])) == 0) {
                return TRUE;
            }
            oss << L"Window Handle: " << hwnd << L" Title: " << windowTitle << "\r\n";
            WindowHandle = hwnd;
            app->WindowsVector.push_back(new WindowControl(
                app->m_hwnd,
                hwnd,
                windowTitle,
                static_cast<int>(app->WindowsVector.size() * 100)
            ));
        }
        return TRUE;
    }

    HWND m_hStaticControl;
protected:

    virtual PCWSTR  ClassName() const = 0;
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

    HWND m_hwnd;
    static HWND WindowHandle;
    static std::wostringstream oss;
    std::vector<WindowControl*> WindowsVector;

   
};