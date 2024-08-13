//***********************************************
// WindowsWindows
// Copyright 2024 Accurate Tool Company, Inc.
//===============================================
// WinMain.cpp
// ----------------------------------------------
// 07/25/2024 MS-24.01.02.0 Updated to be compatible with template window
// 07/23/2024 MS-24.01.01.0 created
//-----------------------------------------------
// Windows entry point

#include <windows.h>
#include "WindowsApp.h"
#include <shellapi.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	WindowsApp app;

	if (SUCCEEDED(app.Initialize())) {
		ShowWindow(app.Window(), nCmdShow);
		app.RunMessageLoop();
	}
	return 0;
}