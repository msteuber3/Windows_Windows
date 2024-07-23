//***********************************************
// WindowsWindows
// Copyright 2024 Accurate Tool Company, Inc.
//===============================================
// DirectXPDFEngine.MainRenderer.cpp
// ----------------------------------------------
// 07/23/2024 MS-24.01.01.0 created
//-----------------------------------------------
// Windows entry point

#include <Windows.h>
#include "WindowsApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	return 0;
}