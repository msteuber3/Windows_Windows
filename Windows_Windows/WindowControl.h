//***********************************************
// WindowsWindows
// Copyright 2024 Michael Steuber
//===============================================
// WindowControl.h
// ----------------------------------------------
// 07/31/2024 MS-24.01.02.06 Updated child window size for sub control windows - buttons now all work properly 
// 07/23/2024 MS-24.01.01.0 created
//-----------------------------------------------
// Control Window header file
//
// Contains WindowControl class. WindowControls are the WinWin UI representation of the active windows, so each window has an associated window control 
// that WinWin operates on. The WindowsApp master file contains a vector of WindowControls which functions as a list of all of the user's active windows,
// and this list can be refreshed by displaying the window list. 
// Physically, window controls are each of the rectangles you see containing minimize, maximize, and close buttons when you display the window list.

#pragma once
#include <Windows.h>
#include <sstream>
#include <vector>

class WindowControl {
public:

	/**
	 * @brief WindowControl constructor
	 * 
	 * Assigns parameters to member fields and calls Create()
	 * 
	 * @param parent Parent window for the control
	 * @param InstanceHandle Handle for the window the control represents
	 * @param oss wostringstream containing the title of the window
	 * @param x Position of control in the parent window, used to determine control screen coordinates
	 */
	WindowControl(HWND parent, HWND InstanceHandle, LPCWSTR oss, int x);


	/**
	 * @brief Receives WM_COMMAND events from the control buttons
	 * 
	 * Callback function that receives and handles events from m_hControlPanel windows. When it receives a WM_COMMAND message, it retrieves the m_InstanceHandle 
	 * from the window that sent the command and checks for one of three IDs:
	 * 1. MIN: Sends a SW_MINIMIZE event to the calling window's m_InstanceHandle, minimizing the associated window
	 * 2. MAX: Sends a SW_MAXIMIZE event to the calling window's m_InstanceHandle, maximizing the associated window
	 * 3. CLOSE: Sends a WM_CLOSE message to the calling window's m_InstanceHandle, closing the associated window
	 * 
	 * @param hwnd HWND of calling window
	 * @param message Message to translate
	 * @param wParam Stores the event ID in its HIWORD and command ID in its LOWORD 
	 * @param lParam Stores the HWND of the child window that triggered the calling window
	 * @return Success/failure code
	 */
	static LRESULT CALLBACK ControlPanelProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	/**
	 * @brief Creates the windows associated with each WindowControl
	 * 
	 * Creates and registers the ControlPanelClass window class and then creates m_hControl and below in the following hierarchy - 
	 * 
	 *					m_hwnd
	 *					   |
	 *        m_hActiveWindowsControlPanel
	 *                     |
	 *                     V
	 *              WindowControl(...)
	 *                     |
	 *                     V
	 *					Create()
	 *                     |
	 *                     V
	 *             m_hControlPanel 
	 *            (ControlPanelProc)
	 *	    _______________|_______________
	 *		|        |	          |       |
	 *		|        |            |       |       
	 * m_Minimize  m_Maximize  m_Title  m_Close  
	 *  (button)     (button)   (text)  (button)
	 * 
	 * m_hControlPanel and all of its child windows send messages to ControlPanelProc 
	 */
	void Create();

	/**
	 * @brief Get the distance of the ControlPanel from the top of the screen
	 * @return Y postition of the control panel relative to the screen as an int
	 */
	int GetYPos();

	/**
	 * @brief Get the associated window handle
	 * @return The handle of the assigned window as an HWND
	 */
	HWND GetInstanceHandle();

	/**
	 * @brief Get the title of the associated window
	 * @return The title of the assigned window as an LPCWSTR
	 */
	LPCWSTR GetInstanceTitle();

	// Control Panel - parent of the button windows
	HWND m_hControlPanel;

private:
	// HWND of the associated window
	HWND m_InstanceHandle;

	// Minimize associated window button
	HWND m_Minimize;
	// Maximize associated window button
	HWND m_Maximize;
	// Close associated window button
	HWND m_Close;

	// Title text of the control panel
	HWND m_Title;

	// Parent HWND of the control panel
	HWND m_Parent;

	// Title of the associated window
	LPCWSTR m_oss;

	// Position of control panel relative to other control panels
	int m_x;
};