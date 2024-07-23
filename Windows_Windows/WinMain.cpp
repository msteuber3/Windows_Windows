//***********************************************
// WindowsWindows
// Copyright 2024 Accurate Tool Company, Inc.
//===============================================
// WinMain.cpp
// ----------------------------------------------
// 07/23/2024 MS-24.01.01.0 created
//-----------------------------------------------
// Windows entry point

#include <Windows.h>
#include "WindowsApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	
	WindowsApp app = WindowsApp(hInstance);

	if (app.m_hwnd != NULL) {
		ShowWindow(app.Window(), nCmdShow);
	}

	return 0;
}