#pragma once
#include <Windows.h>
#include <sstream>

class WindowControl {
public:
	WindowControl(HWND parent, HWND InstanceHandle, LPCWSTR oss, int x);

	static LRESULT CALLBACK ControlPanelProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	void Create();

private:
	HWND m_Minimize;
	HWND m_Maximize;
	HWND m_Close;

	HWND m_Title;
	HWND m_Parent;

	HWND m_InstanceHandle;

	LPCWSTR m_oss;

	int m_x;
};