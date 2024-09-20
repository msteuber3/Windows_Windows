//***********************************************
// WindowsWindows
// Copyright 2024 Michael Steuber
//===============================================
// WinWinFunctions.h
// ----------------------------------------------
// 09/03/2024 MS-24.01.07.02 Moved all WinWin base functionality here for use with the command line and UI
// 09/03/2024 MS-24.01.07.01 created
//-----------------------------------------------
// Header file for WinWin base functions
// 
// Contains static class WinWinFunctions which performs all window operations independent of the UI.
// These functions can be called from anywhere and provides a method GetActiveWindows that returns 
// a vector of HWNDS for every active window. This can be passed to any of the WinWin functions allowing
// any window operations to occur from this class.

#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <Windows.h>
#include "resource.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <locale>
#include <codecvt>
#include <filesystem>
#include <commctrl.h>
#include <sstream>
#include <Psapi.h>
#include <shellapi.h>
#include <algorithm>

static class WinWinFunctions {
public:

	/**
	 * @brief Callback function to add every active window to a vector
	 * 
	 * Get a pointer to the WindowHwndVector from the lParam, then check the title to make sure it isn't 
	 * Program Manager, Windows Input Experience, Windows Shell Experience Host, the Windows Windows UI, or the Visual Studio window running Windows Windows
	 * The first 3 are always active even when their windows are closed and they disrupt core WinWin functionality by messing up the count,
	 * WinWin is excluded to prevent it from being impacted by itself, and Visual Studio is only excluded if it is running WinWin for my own sanity during development/
	 * After that, it pushes back the enumerated window handle to the vector pointer
	 * 
	 * @param hwnd The handle to the current window in the enumeration, sort of like i in a for loop
	 * @param lParam Contains user defined parameters
	 * @return Success code as a BOOL
	 */
	static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

	/**
	 * @brief Get every active window
	 * 
	 * Calls EnumWindowsProc callback function to add every active HWND to a vector, then sorts the vector alphabetically by window title
	 * 
	 * @return Every active window as a vector of HWNDs
	 */
	static std::vector<HWND> GetActiveWindows();

	/**
	 * @brief Utility function to compare window titles
	 * @param a First window to compare
	 * @param b Second window to compare
	 * @return True if a is first alphabetically, False if b is first
	 */
	//bool compareHwnd(HWND a, HWND b);

	/**
	 * @brief Stack windows vertically to fill the screen 
	 * 
	 * Triggered by pressing the stack button or calling the "stack" command in the command line. 
	 * Based on the number of windows in the input vector, passes the vector to the appropriate stack function
	 * 
	 * @param WindowVect Vector of windows to stack
	 */
	static void Stack(std::vector<HWND> WindowVect);
	void StackScroll();

	/**
	 * @brief Stack four or less windows to fill the screen. Windows are stretched to screen width.
	 * 
	 * For each vector in the WindowVect, restore the window, set the y position to stackPosY, set the width to the screen width, and set the height to the stackFactorY.
	 * stackFactorY is constant through the runtime of the function and is equal to the screen height divided by the number of windows. stackPosY is incremented by the stackFactorY for each window.
	 * 
	 * @param WindowVector Vector of windows to stack
	 */
	static void StackFourOrLess(std::vector<HWND> WindowVector);

	/**
	 * @brief Stack between five and eight windows to fill the screen. The screen is split in half and windows are split as evenly as possible between both halves.
	 * 
	 * Variables - 
	 * stackFactorYLeft - the height of the screen divided by half of the size of the window vector rounded up
	 * stackFactorYRight - the height of the screen divided by half of the size of the window vector rounded down
	 * Due to these two, the left side of the screen has stacking priority for an uneven vector size. For example, if there are 7 windows, 4 go to the left and 3 go to the right.
	 * stackPosy - Y position of the next vector to be stacked
	 * i - window index
	 * For each window in the vector, the window is restored and then the following is done to split the windows:
	 * 1. If the window index i is not more than half of the size of the vector (rounded up), the x position is set to 0, the y is set to stackPosy, the length to half the screen size, and the height to stackFactorYLeft.
	 * 2. Increment stackPosY
	 * 3. Increment i
	 * 4. Repeat steps 1 through 3 until i is equal to half the size of the window vector, then reset stackPosy to 0
	 * 5. If i is more than half the size of the vector (rounded up), set the x position to half the length of the screen, the y to stackPosy, the width to half the width of the screen, and the height to stackFactorYRight. 
	 * 6. Increment stackPosy
	 * 7. Repeat steps 5 and 6 until the end of the vector is reached
	 * 
	 * @param WindowVector Vector of windows to stack
	 */
	static void StackFiveToEight(std::vector<HWND> WindowVector);
	
	/**
	 * @brief For vectors larger than 8, create subvectors to scroll through for multilayered stacking
	 * 
	 * Create a subvector with a max length of 8 starting with the index determined by winFunStackIndex, then pass the subvector to the appropriate stack function. Clears the vector on completion.
	 * @param WindowVect Vector of windows to stack
	 */
	static void StackWindowsCallback(std::vector<HWND> WindowVect);

	/**
	 * @brief Arranges all windows in a cascaded view 
	 * 
	 * Triggered by pressing cascade button or calling "cascade" in the command line. Starting ten pixels from the top left of the screen, resize each window to a 750 pixel square 
	 * and seperate their top left coordinate by 50 pixels down and 50 pixels to the left
	 * 
	 * @param WindowVect Vector of windows to cascade
	 */
	static void Cascade(std::vector<HWND> WindowVect);

	/**
	 * @brief Minimizes all windows not moved after a cascade
	 * 
	 * Run through the cascade again and if any window is in the same position as specified by the cascade formula, minimize it. 
	 * 
	 * @param WindowVect Vector of windows to squish
	 */
	static void Squish(std::vector<HWND> WindowVect);
	
	/**
	 * @brief Opens a dialog box that allows user to enter text
	 * 
	 * Using the custom DialogBox resource in resource.h, set the entered user text to the static global userInput. Return userInput. 
	 * 
	 * @param hInstance hInstace of calling process
	 * @return The user entered text as a wide string. Empty wstring as default. 
	 */
	static std::wstring GetUserInput(HINSTANCE hInstance);
	
	/**
	 * @brief Save a window layout to a JSON file
	 * 
	 * 1. Open a dialog box to let the  user input a name for the layout. If the dialog box is left blank, the default name is "NewLayout"
	 * 2. Create the file at SavedLayouts/[UserEnteredName].json. If the file already exists, delete the contents. Open the file as LayFile. 
	 * 3. Create a WINDOWPLACEMENT object that will store details about each window's placement. Then create the nlohmann::json objects to write to the JSON file.
	 * 4. For each window in the WindowVect, set the WINDOWPLACEMENT object with details about that particular window. If the window is minimized, reset the right, left, top, and bottom values of the WINDOWPLACEMENT to the values retrieved by GetWindowRect.
	 *    This is done to prevent the window size from being set to 0, 0, 0, 0. GetWindowPlacement does this for minimized windows, while GetWindowRect returns the size of the restored window. 
	 * 5. Get the name of the process running the window
	 * 6. Store the window placement data in a temporary nlohmann::json object. In addition to the values stored in the WINDOWPLACEMENT object, the json holds the name of the process running the window, a minimized flag, the window handle, and the title of the window.
	 * 7. Add the temporary json object to the main json object and clear the temporary object
	 * 8. When the end of the vector is reached, dump the main JSON object into the user defined JSON file.
	 * 
	 * @param WindowVect Vector of windows to save
	 */
	static void SaveWindowLayout(std::vector<HWND> WindowVect, std::wstring presetLayoutName = L"Default");

	/**
	 * @brief Executes the layout of the specified user window layout
	 * 
	 * NOTE: This does not reopen windows. If the window is not open, it is skipped in the JSON. Only open windows are effected.  
	 * Triggered by pressing a window layout button in the UI or calling "ExecuteDesktop" on the command line.
	 *  1. Open the JSON file passed in the json parameter as LayFile after checking if it exists. This is retrieved from the text of button if triggered through the UI or the first parameter of the command line command. 
	 *  2. Create a vector of pointers to SavedWindows (see below for SavedWindow struct) and iterate through each object in the json file creating a new SavedWindow for each object.
	 *  3. For each window in the WindowVect, retrieve the name of the process running the window. Compare to the processes of the SavedWindows in the SavedWindows vector, and when a match is found, create a WINDOWPLACEMENT object with all of the relevant data in the SavedWindow struct.
	 *	   Set the placement of the current window to this WINDOWPLACEMENT object.
	 * 
	 * @param json wide string of the name of the json file WITHOUT .json at the end or the directory at the beginning. This is the same text that the user entered when creating the layout and is retrieved from the text of the button if triggered through the UI or the first parameter of the "ExecuteLayout" command on the command line. 
	 * @param WindowVect Vector of currently open windows to execute JSON on.
	 */
	static void ExecuteWindowLayout(std::wstring json, std::vector<HWND> WindowVect);
	
	/**
	 * @brief Saves a desktop icon layout to a JSON file
	 * 
	 *  1. Open a dialog box to let the  user input a name for the layout. If the dialog box is left blank, the default name is "NewLayout"
	 *  2. Create the file at SavedLayouts/[UserEnteredName].json. If the file already exists, delete the contents. Open the file as LayFile. 
	 *  3. Accessing the desktop -
	 *		This step is pretty tricky. If you are determined to understand it, be my guest, but I would reccomend skipping it lest you rip your hair out over it. 
	 *		You need drill down into the SysListView32 control and retrieve the window handle. This is the hierarchy for it:
	 *		Progman
	 *		   --->SHELLDLL_DefView
	 *					  -------->SysListView32
	 *		Use the handle of SysListView32 to get the number of desktop icons and the process id of SysListView32, open the process to read and write. 
	 *		Then, allocate virtual memory in the SysListView32 process for 3 objects: the LPPOINT (special windows type for pointer to a point object) of the icon, 
	 *      a pointer to the LVITEM (list view item, recieves info about the icon), and the text of the icon as a LPWSTR (pointer to a WCHAR)
	 *		You'll also need a WCHAR for the itemName, a POINT for the icon position, and a SIZE_T object to specify the number of bytes to read from the SysListView32 process
	 *		Got it? Good. Now we can move on; we'll be back to these very soon. 
	 *  4. Iterate over the number of desktop icons you retrieved in step 3: 
	 *		- Get the position of the ith icon and store it in the virtually allocated LPPOINT. This is done by sending i as a message to SysListView32 with the LPPOINT as the wParam 
	 *		- Read the SysListView32 process memory of the LPPOINT to the the POINT iconPos so that you can access it
	 *		- Create an LVITEM object with the pszText attribute set to the virtually allocated LPWSTR
	 *		- Write this new LVITEM to the virtually allocated LVITEM
	 *		- Send a message to the SysListView32 process telling it to store the item text of the ith icon in the virtually allocated LVITEM. Now, the virtually allocated LPWSTR (itemText) contains the icon's name.
	 *		- Read itemText (from SysListView32) into our WCHAR itemName
	 *		After all of that, the icon position is stored in a POINT iconPos and the icon name is stored in a WCHAR itemName
	 *		That wasn't so bad now, was it? Now back to the normal stuff for human beings.
	 *	5. Put the icon name and position in a temporary nlohmann::json object, then add that object to the main json object and clear the temp one
	 *  6. When we get to the end of the icon list, free all of that virtual memory we allocated earlier
	 *	7. Dump the json object into the LayFile and close it.
	 * 
	 * This took a VERY long time to figure out so I hope someone out there reads it someday
	 */
	static void SaveDesktopLayout();

	/**
	 * @brief Executes the layout of the specified user icon layout
	 * 
	 * 1. Check if the JSON file passed through json exists, open it if it does
	 * 2. Create a vector of pointers to SavedIcon structs, see SavedIcon struct below
	 * 3. Much like SaveDesktopLayout(), we start the funky windows stuff by drilling down into SysListView32 and obtaining the window handle
	 *	  Progman
	 *		   --->SHELLDLL_DefView
	 *						  -------->SysListView32
	 *	  Use this handle to get the number of desktop icons, the process id, and the handle of the process
	 *    Virtually allocate memory for an LPPOINT, LVITEM, and LPWSTR in the SysListView32 process
	 *	  In addition, create a WCHAR to store the icon name, a POINT to store the icon position, a SIZE_T object to store the number of bytes to read from the SysListView32 process, and anotehr POINT for the saved icon position.
	 * 
	 *  4. For each item in the JSON file, add a SavedIcon struct to the SavedIcons vector, assigning the json object attributes to their corresponding SavedIcon struct attributes
	 *	5. Also similar to SaveDesktopLayout(), iterate over the number of desktop icons we retrieved in step 3
	 *		- Get the position of the ith icon and store it in the virtually allocated LPPOINT. This is done by sending i as a message to SysListView32 with the LPPOINT as the wParam 
	 *		- Read the SysListView32 process memory of the LPPOINT to the the POINT iconPos so that you can access it
	 *		- Create an LVITEM object with the pszText attribute set to the virtually allocated LPWSTR
	 *		- Write this new LVITEM to the virtually allocated LVITEM
	 *		- Send a message to the SysListView32 process telling it to store the item text of the ith icon in the virtually allocated LVITEM. Now, the virtually allocated LPWSTR (itemText) contains the icon's name.
	 *		- Read itemText (from SysListView32) into our WCHAR itemName
	 *		I'm so glad windows made this simple and easy to understand. yay.  
	 * 6. Iterate over the vector of SavedIcons and compare the saved names to the itemName. 
	 *	  When a match is found, send a message to SysListView32 telling it to set the item at index i to the (x,y) position in the SavedIcon struct
	 * 7. After the loop reaches the last icon, free the virtually allocated memory and close the HANDLE to the SysListView32 process 
	      (NOT the process itself, this will cause bad things to happen. Windows will lay a curse upon your family.)

	 * @param json wide string of the name of the json file WITHOUT .json at the end or the directory at the beginning. 
	 *		  This is the same text that the user entered when creating the layout and is retrieved from the text of the button if triggered through the UI or the first parameter of the "ExecuteDesktop" command on the command line. 
	 */
	static void ExecuteDesktopLayout(std::wstring json);

	/**
	 * @brief Struct to store data retrieved from a JSON file to be converted to WINDOWPLACEMENT object 
	 */
	static struct SavedWindow {
		SavedWindow(std::string process,
			std::string title,
			HWND handle,
			UINT flags,
			UINT showCmd,
			POINT ptMinPosition,
			POINT ptMaxPosition,
			RECT rcNormalPosition) {
			m_process = process;
			m_title = title;
			m_handle = handle;
			m_flags = flags;
			m_showCmd = showCmd;
			m_ptMinPosition = ptMinPosition;
			m_ptMaxPosition = ptMaxPosition;
			m_rcNormalPosition = rcNormalPosition;
			moved = FALSE;
		};
		std::string m_process;
		std::string m_title;
		HWND m_handle;
		UINT m_flags;
		UINT m_showCmd;
		POINT m_ptMinPosition;
		POINT m_ptMaxPosition;
		RECT m_rcNormalPosition;
		bool moved;
		WINDOWPLACEMENT* getWinPlacement() {
			WINDOWPLACEMENT placement;
			placement.showCmd = m_showCmd;
			placement.flags = m_flags;
			placement.length = sizeof(WINDOWPLACEMENT);
			placement.ptMaxPosition = m_ptMinPosition;
			placement.ptMaxPosition = m_ptMaxPosition;
			placement.rcNormalPosition = m_rcNormalPosition;
			return &placement;
		}
	};

	/**
	 * @brief Struct to store name and position data from a JSON file to be used to match and set icon positions
	 */
	struct SavedIcon {
		SavedIcon(std::string iconName,
			POINT iconPos) {
			m_iconName = iconName;
			m_iconPos = iconPos;
		};
		std::string m_iconName;
		POINT m_iconPos;
	};

	/**
	 * @brief Utility to convert wide strings to narrow strings
	 * 
	 * Since Win32 primarily uses wide strings and nlohmann json uses almost exclusively narrow strings, this utiltity function is used as a translation between the two libraries.
	 * Get the size needed for the new string using WideCharToMultibyte, create a string of that size, and use WideCharToMultibyte again to write the wstring to the new string.
	 * Then remove the null terminator from the end of the new string (WideCharToMultibyte adds one)
	 * 
	 * @param wstr Wide string to convert
	 * @return Converted narrow string
	 */
	static std::string ConvertToNarrowString(const std::wstring& wstr) {
		if (wstr.empty()) {
			return std::string(); // If no wstring is provided, return an empty string
		}

		int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL); // Get the size needed for the new string
		if (size_needed == 0) {
			throw std::runtime_error("WideCharToMultiByte failed");
		}

		std::string str(size_needed - 1, 0); // Create new string
		int result = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size_needed, NULL, NULL); // And write the contents of the wstring to it
		if (result == 0) {
			throw std::runtime_error("WideCharToMultiByte failed");
		}

	//	str.resize(size_needed - 1); // Remove the null terminator added by WideCharToMultiByte

		return str;
	}
};