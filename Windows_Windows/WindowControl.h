#pragma once
#include <Windows.h>
#include <sstream>
#include <vector>

class WindowControl {
public:
	WindowControl(HWND parent, HWND InstanceHandle, LPCWSTR oss, int x);

	static LRESULT CALLBACK ControlPanelProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	void Create();

	int GetYPos();

	HWND m_hControlPanel;
	HWND m_Minimize;
	HWND m_Maximize;
	HWND m_Close;

	HWND m_Title;
	HWND m_Parent;
private:
	

	HWND m_InstanceHandle;

	LPCWSTR m_oss;

	int m_x;

};