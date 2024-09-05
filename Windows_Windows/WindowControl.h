//***********************************************
// WindowsWindows
// Copyright 2024 Michael Steuber
//===============================================
// WindowControl.h
// ----------------------------------------------
// 
// 07/31/2024 MS-24.01.02.06 Updated child window size for sub control windows - buttons now all work properly 
// 07/23/2024 MS-24.01.01.0 created
//-----------------------------------------------
// Control Window header file

#pragma once
#include <Windows.h>
#include <sstream>
#include <vector>

class WindowControl {
public:
	WindowControl(HWND parent, HWND InstanceHandle, LPCWSTR oss, int x);

	static LRESULT CALLBACK ControlPanelProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	void Create();

	void StackWindows();

	int GetYPos();

	HWND GetInstanceHandle();

	LPCWSTR GetInstanceTitle();

	HWND m_hControlPanel;

private:
	

	HWND m_InstanceHandle;

	HWND m_Minimize;
	HWND m_Maximize;
	HWND m_Close;

	HWND m_Title;
	HWND m_Parent;

	LPCWSTR m_oss;

	int m_x;

};