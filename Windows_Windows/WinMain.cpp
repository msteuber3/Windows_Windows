//***********************************************
// WindowsWindows
// Copyright 2024 Accurate Tool Company, Inc.
//===============================================
// WinMain.cpp
// ----------------------------------------------
// 08/26/2024 MS-24.01.06.01 Added command line arguments
// 07/25/2024 MS-24.01.02.0 Updated to be compatible with template window
// 07/23/2024 MS-24.01.01.0 created
//-----------------------------------------------
// Windows entry point

#include <windows.h>
#include "WindowsApp.h"
#include <shellapi.h>

#define CASCADE 1
#define SAVE_LAYOUT 2
#define SHOW_ACTIVE_WINDOWS 3
#define HIDE_ACTIVE_WINDOWS 4
#define VIEW_SAVED_CONFIGS 5
#define EXECUTE_LAYOUT 6
#define HIDE_SAVED_CONFIGS 7
#define STACK 8
#define SAVE_DESKTOP_LAYOUT 9
#define NEXT_STACK 10
#define PREV_STACK 11
#define EXIT_STACK 12
#define SAVED_DESKTOP_LAYOUTS 13
#define HIDE_SAVED_DESKTOP_CONFIGS 14
#define EXECUTE_DESKTOP_LAYOUT 15
#define SQUISH 16

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	std::string cmdLine(pCmdLine);

	WindowsApp app;
    int id;
	if (cmdLine == "stack") {
		app.Initialize();
		app.StackWindows();
		return 0;
	}
    else if (cmdLine == "cascade") {
        app.Initialize();
        app.CascadeWindows();
        return 0;
    }
    else if (cmdLine == "squish") {
        app.Initialize();
        app.SquishCascade();
        return 0;
    }
    else if (cmdLine == "squish") {
        app.Initialize();
        app.SquishCascade();
        return 0;
    }
    else if (cmdLine == "SaveLayout") {
        app.Initialize();
        app.WinWinSaveLayout();
        return 0;
    }
    else if (cmdLine == "ExecuteLayout") {
        app.Initialize();
        app.ExecuteSaved(L"jsonFile");
        return 0;
    }
    else if (cmdLine == "SaveDesktop") {
        app.Initialize();
        app.SaveDesktopLayout();
        return 0;
    }
    else if (cmdLine == "ExecuteDesktop") {
        app.Initialize();
        app.ExecuteSavedDesktopLayout(L"desktopJsonFile");
        return 0;
    }
	if (SUCCEEDED(app.Initialize())) {
		ShowWindow(app.Window(), nCmdShow);
		app.RunMessageLoop();
	}
	return 0;
}