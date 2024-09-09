//***********************************************
// WindowsWindows
// Copyright 2024 Michael Steuber
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
//
// Win32 looks for the WinMain function to execute first. 
// Starts by checking for command line parameters - if they are present, they call their corresponding function
// If there are no command line paramters, a WindowsApp is created and the UI is initialized

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
   if (__argc > 2) {        // If there are more than 2 command line paramters, store the third in layout after converting it to a wide string
       command = __argv[1]; // The first two command line parameters are Windows_Windows.exe and whatever command the user would like to execute. 
       params = __argv[2];  // For example, if the user would like to execute cascade, they would call ./Windows_Windows.exe cascade
       int size_needed = MultiByteToWideChar(CP_UTF8, 0, params.c_str(), -1, NULL, 0);
       std::wstring wstr(size_needed, 0);
       MultiByteToWideChar(CP_UTF8, 0, params.c_str(), -1, &wstr[0], size_needed);
       layout = wstr;
   }

   WindowsApp app;
                            // Match a user function call to a WinWin function
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
        if (__argc > 2) {
            WinWinFunctions::SaveWindowLayout(WinWinFunctions::GetActiveWindows(), layout);
        }
        else {
            WinWinFunctions::SaveWindowLayout(WinWinFunctions::GetActiveWindows());
        }
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
		ShowWindow(app.Window(), nCmdShow);   // If there are no user provided parameters or if the parameters don't match any of the IDs above,                                  
		app.RunMessageLoop();                 // initialize the app and start the UI 
	}
	return 0;
}