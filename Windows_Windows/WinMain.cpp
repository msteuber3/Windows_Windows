//***********************************************
// WindowsWindows
// Copyright 2024 Accurate Tool Company, Inc.
//===============================================
// WinMain.cpp
// ----------------------------------------------
// 09/03/2024 MS-24.01.07.02 Updated command line args to use WinWinFunctions functions rather than WindowsApp UI functions
// 08/27/2024 MS-24.01.06.02 updated command line args to support executelayout parameters
// 08/26/2024 MS-24.01.06.01 Added command line arguments
// 07/25/2024 MS-24.01.02.0 Updated to be compatible with template window
// 07/23/2024 MS-24.01.01.0 created
//-----------------------------------------------
// Windows entry point

#include <windows.h>
#include "WindowsApp.h"
#include <shellapi.h>
#include <stdio.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
   std::string params;
   std::string cmdLine(pCmdLine);
   std::wstring layout;
   std::string command;
   if (__argc > 2) {
       command = __argv[1];
       params = __argv[2];
       int size_needed = MultiByteToWideChar(CP_UTF8, 0, params.c_str(), -1, NULL, 0);
       std::wstring wstr(size_needed, 0);
       MultiByteToWideChar(CP_UTF8, 0, params.c_str(), -1, &wstr[0], size_needed);
       layout = wstr;
   }
   WindowsApp app;
   int id;
	if (cmdLine == "stack") {
		WinWinFunctions::Stack(WinWinFunctions::GetActiveWindows());
		return 0;
	}
   else if (cmdLine == "cascade") {
        WinWinFunctions::Cascade(WinWinFunctions::GetActiveWindows());
       return 0;
   }
   else if (cmdLine == "squish") {
        WinWinFunctions::Squish(WinWinFunctions::GetActiveWindows());
       return 0;
   }
   else if (cmdLine == "SaveLayout") {
       WinWinFunctions::SaveWindowLayout(WinWinFunctions::GetActiveWindows());
       return 0;
   }
   else if (command == "ExecuteLayout") {
       WinWinFunctions::ExecuteWindowLayout(layout, WinWinFunctions::GetActiveWindows());
       return 0;
   }
   else if (cmdLine == "SaveDesktop") {
       WinWinFunctions::SaveDesktopLayout();
       return 0;
   }
   else if (command == "ExecuteDesktop") {
       WinWinFunctions::ExecuteDesktopLayout(layout);
       return 0;
   }
	if (SUCCEEDED(app.Initialize())) {
		ShowWindow(app.Window(), nCmdShow);
		app.RunMessageLoop();
	}
	return 0;
}