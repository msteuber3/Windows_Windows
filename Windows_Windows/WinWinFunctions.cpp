//***********************************************
// WindowsWindows
// Copyright 2024 Michael Steuber
//===============================================
// WinWinFunctions.cpp
// ----------------------------------------------
// 09/03/2024 MS-24.01.07.02 Moved all WinWin base functionality here for use with the command line and UI
// 09/03/2024 MS-24.01.07.01 created
//-----------------------------------------------
// Source code for WinWin base functions
// 
// Defines functions for static class WinWinFunctions which performs all window operations independent of the UI.
// These functions can be called from anywhere and provides a method GetActiveWindows that returns 
// a vector of HWNDS for every active window. This can be passed to any of the WinWin functions allowing
// any window operations to occur from this class.


#include "WinWinFunctions.h"

static std::wstring userInput;

INT_PTR CALLBACK DialogProcWinFun(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK: {
            WCHAR text[256];
            GetDlgItemText(hDlg, IDC_EDIT_TEXT, text, 256);
            userInput = text; // Store the input text in the global variable
            EndDialog(hDlg, IDOK);
            return TRUE;
        }
        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

BOOL CALLBACK WinWinFunctions::EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    std::vector<HWND>* WindowHwndVector = reinterpret_cast<std::vector<HWND>*>(lParam);
    WCHAR windowTitle[256];
    if (GetParent(hwnd) == NULL && IsWindowVisible(hwnd)) {
        LRESULT result = SendMessageTimeoutW(hwnd, WM_NULL, 0, 0, SMTO_ABORTIFHUNG, 1000, NULL);
        if (result == 0 || !IsWindowEnabled(hwnd)) { // Check if the window is able to receive/respond to messages
            return TRUE;
        }
        if (GetWindowText(hwnd, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0])) == 0) {
            return TRUE;
        }
        if ((std::wstring(windowTitle) == L"Program Manager" || std::wstring(windowTitle) == L"Windows Input Experience" || std::wstring(windowTitle) == L"Windows Shell Experience Host")) {
            return TRUE;
        }
        if (std::wstring(windowTitle) == L"Windows Window Extension Window" || std::wstring(windowTitle) == L"Windows_Windows (Running) - Microsoft Visual Studio") {
            return TRUE;
        }
        // ^ Exclude Program Manager, Windows Input Experience, Windows Shell Experience Host, the Windows Windows UI, and the Visual Studio window running Windows Windows from the vector
        WindowHwndVector->push_back(hwnd);
    }
    return TRUE;
}

bool compareHwnd(HWND a, HWND b) { // Sort windows by title alphabetically
    char titleA[256], titleB[256];
    GetWindowTextA(a, titleA, sizeof(titleA));
    GetWindowTextA(b, titleB, sizeof(titleB));
    return std::string(titleA) < std::string(titleB); // Reutrn True if a is first alphabetically, False if b is first 
}

std::vector<HWND> WinWinFunctions::GetActiveWindows()
{
    std::vector<HWND> WindowHwndVector;
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&WindowHwndVector)); // Enumerate through the windows with this callback function. 
    if (!WindowHwndVector.empty()) {
        std::sort(WindowHwndVector.begin(), WindowHwndVector.end(), compareHwnd);
    }
    return WindowHwndVector;
}

void WinWinFunctions::Stack(std::vector<HWND> WindowVect)
{
    int winFunStackIndex;

    if (WindowVect.size() <= 4) {
        StackFourOrLess(WindowVect);
    }
    else if (WindowVect.size() <= 8) {
        StackFiveToEight(WindowVect);
    }
    else if (WindowVect.size() > 8) {

        winFunStackIndex = 0;
        StackWindowsCallback(WindowVect);
    }
}

void WinWinFunctions::StackFourOrLess(std::vector<HWND> WindowVector) {
    int stackFactorY;
    int stackPosy = 0;
    stackFactorY = GetSystemMetrics(SM_CYSCREEN) / WindowVector.size();
    for (HWND ctrl : WindowVector) {
        ShowWindow(ctrl, SW_RESTORE);
        SendMessage(ctrl, WM_SYSCOMMAND, SC_RESTORE, 0);
        SetWindowPos(ctrl, HWND_TOPMOST, 0, stackPosy, GetSystemMetrics(SM_CXSCREEN), stackFactorY, NULL);
        SetWindowPos(ctrl, HWND_NOTOPMOST, 0, stackPosy, GetSystemMetrics(SM_CXSCREEN), stackFactorY, NULL);
        stackPosy += stackFactorY;
    }
}

void WinWinFunctions::StackFiveToEight(std::vector<HWND> WindowVector) {
    int stackFactorYLeft;
    int stackFactorYRight;
    int stackPosy = 0;
    int i = 0;
    stackFactorYLeft = GetSystemMetrics(SM_CYSCREEN) / ceil(float(WindowVector.size()) / float(2));
    stackFactorYRight = GetSystemMetrics(SM_CYSCREEN) / floor(float(WindowVector.size()) / float(2));
    for (HWND ctrl : WindowVector) {
        ShowWindow(ctrl, SW_RESTORE);
        SendMessage(ctrl, WM_SYSCOMMAND, SC_RESTORE, 0);
        if (i >= ceil(float(WindowVector.size()) / float(2))) {

            SetWindowPos(ctrl, HWND_TOPMOST, GetSystemMetrics(SM_CXSCREEN) / 2, stackPosy, GetSystemMetrics(SM_CXSCREEN) / 2, stackFactorYRight, NULL);
            SetWindowPos(ctrl, HWND_NOTOPMOST, GetSystemMetrics(SM_CXSCREEN) / 2, stackPosy, GetSystemMetrics(SM_CXSCREEN) / 2, stackFactorYRight, NULL);

            stackPosy += stackFactorYRight;
        }

        else {
            SetWindowPos(ctrl, HWND_TOPMOST, 0, stackPosy, GetSystemMetrics(SM_CXSCREEN) / 2, stackFactorYLeft, NULL);
            SetWindowPos(ctrl, HWND_NOTOPMOST, 0, stackPosy, GetSystemMetrics(SM_CXSCREEN) / 2, stackFactorYLeft, NULL);

            stackPosy += stackFactorYLeft;
        }
        i++;
        if (i == ceil(float(WindowVector.size()) / float(2))) {
            stackPosy = 0;
        }
    }
}

void WinWinFunctions::StackWindowsCallback(std::vector<HWND> WindowVect)
{
    int winFunStackIndex = 0;

    int lastVector = WindowVect.size() % 8;
    int subVectorCount = (floor(float(WindowVect.size()) / float(8))) + (lastVector == 0 ? 0 : 1);

    std::vector<HWND> SubVector;
    for (int i = 0; i < 8; i++) {
        if (WindowVect.size() <= (winFunStackIndex * 8) + i) {
            break;
        }
        SubVector.push_back(WindowVect[(winFunStackIndex * 8) + i]);
    }
    if (SubVector.size() <= 4) {
        StackFourOrLess(SubVector);
    }
    else if (SubVector.size() <= 8) {
        StackFiveToEight(SubVector);
    }
    SubVector.clear();
}

void WinWinFunctions::Cascade(std::vector<HWND> WindowVect) {
    int stackPosX = 10;
    int stackPosY = 10;
    int stackFactorY = 50;
    if (WindowVect.size() * stackFactorY + 760 > GetSystemMetrics(SM_CYSCREEN)) {
        stackFactorY = (GetSystemMetrics(SM_CYSCREEN) - 760) / WindowVect.size();
        if (stackFactorY < 5) { stackFactorY = 5; }
    }
    for (HWND ctrl : WindowVect) { //Iterate through all windows in WindowsVector (all open windows)
        SendMessage(ctrl, WM_SYSCOMMAND, SC_RESTORE, 0);
        ShowWindow(ctrl, SW_SHOWNORMAL);  // Set each window to normal mode (unmax/unmin)
        SetWindowPos(ctrl, HWND_TOPMOST, stackPosX, stackPosY, 750, 750, NULL); // Bring current window to front
        SetWindowPos(ctrl, HWND_NOTOPMOST, stackPosX, stackPosY, 750, 750, NULL); // Remove "TOPMOST" flag
        stackPosX += 100;
        stackPosY += stackFactorY;
    }
}

void WinWinFunctions::Squish(std::vector<HWND> WindowVect) {
    int stackPosX = 10;
    int stackPosY = 10;
    int stackFactorY = 50;
    if (WindowVect.size() * stackFactorY + 760 > GetSystemMetrics(SM_CYSCREEN)) {
        stackFactorY = (GetSystemMetrics(SM_CYSCREEN) - 760) / WindowVect.size();
        if (stackFactorY < 5) { stackFactorY = 5; }
    }
    RECT windowRect;
    for (HWND ctrl : WindowVect) { //Iterate through all windows in WindowsVector (all open windows)
        GetWindowRect(ctrl, &windowRect);
        if (windowRect.top == stackPosY && windowRect.left == stackPosX) { //&& windowRect.right - windowRect.left == 750 && windowRect.bottom - windowRect.top == 750
            ShowWindow(ctrl, SW_MINIMIZE);
        }
        stackPosX += 75;
        stackPosY += stackFactorY;
    }
}

std::wstring WinWinFunctions::GetUserInput(HINSTANCE hInstance) {
    userInput.clear(); // Clear previous input
    if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_SIMPLE_INPUT_DIALOG), NULL, DialogProcWinFun) == IDOK) {
        return userInput;
    }
    return L"";
}

void WinWinFunctions::SaveWindowLayout(std::vector<HWND> WindowVect, std::wstring presetLayoutName)
{
    std::wstring layoutName;
    if (presetLayoutName != L"Default") {
        layoutName = presetLayoutName;
    }
    else {
        layoutName = GetUserInput(GetModuleHandle(NULL));
    }
    if (layoutName == L"") { layoutName = L"NewLayout"; }
    if (!layoutName.empty() && layoutName.back() == L'\0') layoutName.pop_back();


    // Retrieve the path of the executable to ensure that the correct SavedLayouts directory is referenced
    // Without this, it will attempt to find SavedLayouts in whatever dir the command was called in
    // This is for command line support

    wchar_t exeWcharPath[MAX_PATH];
    GetModuleFileName(NULL, exeWcharPath, MAX_PATH);

    std::wstring exePath(exeWcharPath);
    std::wstring::size_type pos = exePath.find_last_of(L"\\/");
    std::wstring exeDir = exePath.substr(0, pos);

    SetCurrentDirectory(exeDir.c_str()); // Set the working directory to the executable's directory

    std::wstring WinWinLayoutsFile = exeDir + L"/SavedLayouts/" + layoutName + L".json";   // Name of json file (in SavedLayouts folder)
    
    if (!std::filesystem::exists(WinWinLayoutsFile)) { // Check if it exists, create it if not
        std::ofstream{ WinWinLayoutsFile };
    }

    if (std::filesystem::exists(WinWinLayoutsFile)) { // Check if it exists, delete contents if it does
        std::ofstream file(WinWinLayoutsFile, std::ios::trunc);
        file.close();
    }

    std::fstream LayFile;
    LayFile.open(WinWinLayoutsFile);


    WINDOWPLACEMENT pInstancePlacement;
    pInstancePlacement.length = sizeof(WINDOWPLACEMENT);  // Instantiate WINDOWPLACEMENT object

    nlohmann::json placeInfo; // Final json that is written to file
    nlohmann::basic_json placeInfoTemp; // Temp json, cleared after each run of the loop

    DWORD processId;
    WCHAR path[MAX_PATH] = L"";
    HANDLE hProcess;
    RECT rect;
    std::wstring processPath;
    WCHAR instanceTitle[256];
    for (HWND ctrl : WindowVect) {
        GetWindowRect(ctrl, &rect);
        GetWindowPlacement(ctrl, &pInstancePlacement); // Get the hwnd of the current handle and extract placement details. 
                                                       // Put into WINDOWPLACEMENT object 
        if (!IsIconic(ctrl)) { // If the window isn't minimized, set the window size to the dimensions from GetWindowRect. Without this, windows set with the 
                               // Windows auto window placement magic won't be set correctly
            pInstancePlacement.rcNormalPosition.left = rect.left;
            pInstancePlacement.rcNormalPosition.right = rect.right;
            pInstancePlacement.rcNormalPosition.top = rect.top;
            pInstancePlacement.rcNormalPosition.bottom = rect.bottom;

        }
        GetWindowThreadProcessId(ctrl, &processId);
        hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId); // Get executable associated with window handle
        GetModuleFileNameEx(hProcess, NULL, path, MAX_PATH);
        CloseHandle(hProcess);

        processPath = std::wstring(path);
        GetWindowTextW(ctrl, instanceTitle, sizeof(instanceTitle));

        placeInfoTemp = { {"process", ConvertToNarrowString(processPath)},// Translate WINDOWPLACEMENT to json, stick the process in the front
            {"minimized", IsIconic(ctrl) },
            {"handle", int(ctrl)},
            {"title", ConvertToNarrowString(std::wstring(instanceTitle))},
            {"length", pInstancePlacement.length},
            {"flags", pInstancePlacement.flags},
        {"showCmd", pInstancePlacement.showCmd },
        {"ptMinPosition",
            { {"x", pInstancePlacement.ptMinPosition.x },
             { "y",  pInstancePlacement.ptMinPosition.y } } },
        {"ptMaxPosition",
             { { "x", pInstancePlacement.ptMaxPosition.x},
               { "y", pInstancePlacement.ptMaxPosition.y} } },
         {"rcNormalPosition",
             { { "left", pInstancePlacement.rcNormalPosition.left },
               { "right", pInstancePlacement.rcNormalPosition.right },
               { "top", pInstancePlacement.rcNormalPosition.top },
               { "bottom", pInstancePlacement.rcNormalPosition.bottom } } }
        };

        placeInfo.push_back(placeInfoTemp); //append to main json
        placeInfoTemp.clear();// clear temp
    }

    LayFile << placeInfo; // add to json file

    LayFile.close();
}

void WinWinFunctions::ExecuteWindowLayout(std::wstring json, std::vector<HWND> WindowVect) {

    if (!json.empty() && json.back() == L'\0') json.pop_back(); // Remove null terminator from end of file name

    // Retrieve the path of the executable to ensure that the correct SavedLayouts directory is referenced
    // Without this, it will attempt to find SavedLayouts in whatever dir the command was called in
    // This is for command line support

    wchar_t exeWcharPath[MAX_PATH];
    GetModuleFileName(NULL, exeWcharPath, MAX_PATH);

    std::wstring exePath(exeWcharPath);
    std::wstring::size_type pos = exePath.find_last_of(L"\\/");
    std::wstring exeDir = exePath.substr(0, pos);

    SetCurrentDirectory(exeDir.c_str()); // Set the working directory to the executable's directory

    std::wstring jsonFile = exeDir + L"/SavedLayouts/" + json;
    jsonFile.append(L".json");
    if (!std::filesystem::exists(jsonFile)) { // Check if it exists
        return;
    }

    // So here I want to get every layout from the json file, check which processess are open, put the matching ones in the right place, and minimize the others
    // So from the json I need a vector of structs 
    // And I have my vector of open windows
    // Iterate over open windows and then iterate over vector of structs
    std::vector<SavedWindow*> SavedWindows;

    DWORD processId;
    WCHAR path[MAX_PATH] = L"";
    HANDLE hProcess;
    WCHAR windowTitle[256];
    bool foundWindow = FALSE;

    std::fstream LayFile;
    LayFile.open(jsonFile, std::ios::in);
    nlohmann::json Doc{ nlohmann::json::parse(LayFile) };
    int i = 0;
    for (auto& window : Doc.items()) {
        std::string process = Doc[i].at("process");
        std::string title = Doc[i].at("title");
        int handle = Doc[i].at("handle");
        UINT flags = Doc[i].at("flags");
        UINT length = Doc[i].at("length");
        UINT showCmd = Doc[i].at("showCmd");
        POINT minPos;
        UINT minPosX = Doc[i].at("ptMinPosition").at("x");
        UINT minPosY = Doc[i].at("ptMinPosition").at("y");
        minPos.x = minPosX;
        minPos.y = minPosY;
        POINT maxPos;
        UINT maxPosX = Doc[i].at("ptMaxPosition").at("x");
        UINT maxPosY = Doc[i].at("ptMaxPosition").at("y");
        maxPos.x = maxPosX;
        maxPos.y = maxPosY;
        RECT rcNormalPos;
        rcNormalPos.left = Doc[i].at("rcNormalPosition").at("left");
        rcNormalPos.right = Doc[i].at("rcNormalPosition").at("right");
        rcNormalPos.top = Doc[i].at("rcNormalPosition").at("top");
        rcNormalPos.bottom = Doc[i].at("rcNormalPosition").at("bottom");

        SavedWindows.push_back(new SavedWindow(
            process,
            title,
            HWND(handle),
            flags,
            showCmd,
            minPos,
            maxPos,
            rcNormalPos));
        i++;
    }

    for (SavedWindow* window : SavedWindows) {
        for (HWND ctrl : WindowVect) {
            if (ctrl == window->m_handle) {
                WINDOWPLACEMENT placement;
                placement.length = sizeof(WINDOWPLACEMENT);
                placement.showCmd = window->m_showCmd;
                placement.flags = window->m_flags;
                placement.ptMaxPosition = window->m_ptMinPosition;
                placement.ptMaxPosition = window->m_ptMaxPosition;
                placement.rcNormalPosition = window->m_rcNormalPosition;

                SetWindowPlacement(ctrl, &placement);
                window->moved = TRUE;
                foundWindow = TRUE;
            }
        }
        if (!foundWindow) {
            for (HWND ctrl : WindowVect) {
                GetWindowThreadProcessId(ctrl, &processId);
                hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId); // Get executable associated with window handle
                GetModuleFileNameEx(hProcess, NULL, path, MAX_PATH);
                CloseHandle(hProcess);
                GetWindowText(ctrl, windowTitle, sizeof(windowTitle));

                std::string convertedWindowTitle = ConvertToNarrowString(windowTitle);
                if (convertedWindowTitle == window->m_title && !window->moved) {
                    WINDOWPLACEMENT placement;
                    placement.length = sizeof(WINDOWPLACEMENT);
                    placement.showCmd = window->m_showCmd;
                    placement.flags = window->m_flags;
                    placement.ptMaxPosition = window->m_ptMinPosition;
                    placement.ptMaxPosition = window->m_ptMaxPosition;
                    placement.rcNormalPosition = window->m_rcNormalPosition;

                    SetWindowPlacement(ctrl, &placement);
                    window->moved = TRUE;
                    foundWindow = TRUE;
                }
            }
        }
        if (!foundWindow) {
            for (HWND ctrl : WindowVect) {
                int wideStrSize = MultiByteToWideChar(CP_UTF8, 0, window->m_process.c_str(), -1, nullptr, 0);
                std::wstring convertedWideStr(wideStrSize, L'/0');
                MultiByteToWideChar(CP_UTF8, 0, window->m_process.c_str(), -1, &convertedWideStr[0], wideStrSize);
                convertedWideStr.erase(std::remove(convertedWideStr.begin(), convertedWideStr.end(), L'\0'), convertedWideStr.end());
                std::wstring pathWstring(path);
                if (pathWstring == convertedWideStr) {

                    WINDOWPLACEMENT placement;
                    placement.length = sizeof(WINDOWPLACEMENT);
                    placement.showCmd = window->m_showCmd;
                    placement.flags = window->m_flags;
                    placement.ptMaxPosition = window->m_ptMinPosition;
                    placement.ptMaxPosition = window->m_ptMaxPosition;
                    placement.rcNormalPosition = window->m_rcNormalPosition;

                    SetWindowPlacement(ctrl, &placement);
                    window->moved = TRUE;
                    foundWindow = TRUE;
                }
            }
        }
    foundWindow = FALSE;
    }
    for (SavedWindow* windStruct : SavedWindows) {
        windStruct->moved = FALSE;
    }
}



 
void WinWinFunctions::SaveDesktopLayout()
{
    std::wstring layoutName = GetUserInput(GetModuleHandle(NULL));

    std::wstring WinWinLayoutsFile = L"SavedDesktopLayouts/" + layoutName + L".json";   // Name of json file (in SavedLayouts folder)

    if (!std::filesystem::exists(WinWinLayoutsFile)) { // Check if it exists, create it if not
        std::ofstream{ WinWinLayoutsFile };
    }

    HWND hProgMan = FindWindow(L"Progman", NULL);
    HWND hShellView = FindWindowExW(hProgMan, NULL, L"SHELLDLL_DefView", NULL);
    FindWindowEx(hShellView, NULL, L"SysListView32", NULL);

    HWND hDesktopListView = FindWindowEx(hShellView, NULL, L"SysListView32", NULL); // Drill down into SysListView32
    int itemCount = ListView_GetItemCount(hDesktopListView);
    DWORD pid = 0;
    GetWindowThreadProcessId(hDesktopListView, &pid);
    HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, pid);

    // Allocate memory in SysListView32
    LPPOINT pt = (LPPOINT)VirtualAllocEx(hProcess, NULL, sizeof(POINT), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    LVITEMW* lvItem = (LVITEM*)VirtualAllocEx(hProcess, NULL, sizeof(LVITEM), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    LPWSTR itemText = (LPWSTR)VirtualAllocEx(hProcess, NULL, 256 * sizeof(WCHAR), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    WCHAR itemName[256] = L"";

    POINT iconPos;
    SIZE_T numRead;

    std::fstream LayFile;
    LayFile.open(WinWinLayoutsFile);

    nlohmann::json placeInfo; // Final json that is written to file
    nlohmann::basic_json placeInfoTemp; // Temp json, cleared after each run of the loop

    for (int i = 0; i < itemCount; ++i) {
        if (!ListView_GetItemPosition(hDesktopListView, i, pt)) {
            wprintf(L"Failed to get position for item %d\n", i);
            continue;
        }
        ReadProcessMemory(hProcess, pt, &iconPos, sizeof(POINT), &numRead); // Transfer the LPPOINT from SysListView32 memory to iconPos

        LVITEM lvi = { 0 };
        lvi.iSubItem = 0;
        lvi.cchTextMax = 256;
        lvi.pszText = itemText; // Set pszText to virtually allocated LPWSTR 

        WriteProcessMemory(hProcess, lvItem, &lvi, sizeof(LVITEM), NULL); // Write LVITEM to the target process

        SendMessage(hDesktopListView, LVM_GETITEMTEXT, (WPARAM)i, (LPARAM)lvItem); // Retrieve the item text

        ReadProcessMemory(hProcess, itemText, itemName, 256 * sizeof(WCHAR), &numRead); // Read the item text from the target process
        
        placeInfoTemp = { {"icon", std::wstring(itemName).c_str()},// Translate WINDOWPLACEMENT to json, stick the process in the front
            {"position", {{"x", std::to_string(iconPos.x)}, {"y",  std::to_string(iconPos.y)}}} };

        placeInfo.push_back(placeInfoTemp); //append to main json
        placeInfoTemp.clear();// clear temp
    }
    VirtualFreeEx(hProcess, pt, 0, MEM_RELEASE);
    VirtualFreeEx(hProcess, lvItem, 0, MEM_RELEASE);
    VirtualFreeEx(hProcess, itemText, 0, MEM_RELEASE);
    CloseHandle(hProcess);
    LayFile << placeInfo; // add to json file

    LayFile.close();
}

void WinWinFunctions::ExecuteDesktopLayout(std::wstring json)
{
    std::wstring jsonFile = L"SavedDesktopLayouts/" + json + L".json";
    if (!std::filesystem::exists(jsonFile)) { // Check if it exists
        return;
    }
    std::vector<SavedIcon*> SavedIcons;

    HWND hProgMan = FindWindow(L"Progman", NULL);
    HWND hShellView = FindWindowEx(hProgMan, NULL, L"SHELLDLL_DefView", NULL);
    FindWindowEx(hShellView, NULL, L"SysListView32", NULL);

    HWND hDesktopListView = FindWindowEx(hShellView, NULL, L"SysListView32", NULL);
    int itemCount = ListView_GetItemCount(hDesktopListView);
    DWORD pid = 0;
    GetWindowThreadProcessId(hDesktopListView, &pid);
    HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, pid);

    // Allocate memory in SysListView32
    LPPOINT pt = (LPPOINT)VirtualAllocEx(hProcess, NULL, sizeof(POINT), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    LVITEM* lvItem = (LVITEM*)VirtualAllocEx(hProcess, NULL, sizeof(LVITEM), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    LPWSTR itemText = (LPWSTR)VirtualAllocEx(hProcess, NULL, 256 * sizeof(WCHAR), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    WCHAR itemName[256] = L"";

    POINT iconPos;
    SIZE_T numRead;
    POINT savedIconPos;

    std::fstream LayFile;
    LayFile.open(jsonFile, std::ios::in);
    nlohmann::json Doc{ nlohmann::json::parse(LayFile) };
    int i = 0;
    for (auto& window : Doc.items()) {
        std::string iconName = Doc[i].at("icon");
        std::string iconPosXStr = Doc[i].at("position").at("x");
        UINT iconPosX = std::stoi(iconPosXStr);
        std::string iconPosYStr = Doc[i].at("position").at("y");
        UINT iconPosY = std::stoi(iconPosYStr);
        savedIconPos.x = iconPosX;
        savedIconPos.y = iconPosY;

        SavedIcons.push_back(new SavedIcon(iconName, savedIconPos));
        i++;
    }
    for (int i = 0; i < itemCount; ++i) {
        if (!ListView_GetItemPosition(hDesktopListView, i, pt)) {
            wprintf(L"Failed to get position for item %d\n", i);
        }
        ReadProcessMemory(hProcess, pt, &iconPos, sizeof(POINT), &numRead);

        LVITEM lvi = { 0 };
        lvi.iSubItem = 0;
        lvi.cchTextMax = 256;
        lvi.pszText = itemText;

        WriteProcessMemory(hProcess, lvItem, &lvi, sizeof(LVITEM), NULL); // Write LVITEM to the target process
        
        SendMessageW(hDesktopListView, LVM_GETITEMTEXT, (WPARAM)i, (LPARAM)lvItem);// Retrieve the item text

        ReadProcessMemory(hProcess, itemText, itemName, 256 * sizeof(WCHAR), &numRead);// Read the item text from the target process

        for (SavedIcon* icon : SavedIcons) {
            int wideStrSize = MultiByteToWideChar(CP_UTF8, 0, icon->m_iconName.c_str(), -1, nullptr, 0);
            std::wstring convertedWideStr(wideStrSize, L'/0');
            MultiByteToWideChar(CP_UTF8, 0, icon->m_iconName.c_str(), -1, &convertedWideStr[0], wideStrSize);
            convertedWideStr.erase(std::remove(convertedWideStr.begin(), convertedWideStr.end(), L'\0'), convertedWideStr.end());
            std::wstring pathWstring(itemName);
            if (pathWstring == convertedWideStr) {

                int itemIndex = i;
                SendMessage(hDesktopListView, LVM_SETITEMPOSITION, (WPARAM)itemIndex, MAKELPARAM(icon->m_iconPos.x, icon->m_iconPos.y));
                break;
                itemName[0] = L'\0';
            }
        }
    }
    VirtualFreeEx(hProcess, pt, 0, MEM_RELEASE);
    VirtualFreeEx(hProcess, lvItem, 0, MEM_RELEASE);
    VirtualFreeEx(hProcess, itemText, 0, MEM_RELEASE);
    CloseHandle(hProcess);
}
