#pragma once
#include <Windows.h>
class WindowsApp {

public:
    WindowsApp();

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    BOOL Create(HINSTANCE hInstance);

    HWND Window();

private:
    virtual PCWSTR  ClassName() const;
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    

    HWND m_hwnd;

};