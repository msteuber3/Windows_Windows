//***********************************************
// WindowsWindows
// Copyright 2024 Michael Steuber
//===============================================
// WindowsApp.h
// ----------------------------------------------
// 09/03/2024 MS-24.01.07.03 Updated documentation
// 07/25/2024 MS-24.01.02.06 Added PrintActiveWindows() 
// 07/25/2024 MS-24.01.02.01 Updated to be compatible with template window
// 07/23/2024 MS-24.01.01.00 created
//-----------------------------------------------
// Main window header code
//
// Master code for UI functionality. Creates the main window and all child windows that aren't members of the ControlWindow class. 
// Contains HandleMessage(), which handles all events sent to the main window. 
// m_hwnd is the top level of the windows hierarchy, it is a parent to everything you see on screen.


#pragma once
#include <iostream>
#include <Windows.h>
#include <vector>
#include "BaseWindow.cpp"
#include "WindowControl.h"
#include <shellapi.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <locale>
#include <codecvt>
#include <filesystem>
#include <commctrl.h>
#include <string>
#include <sstream>
#include <Psapi.h>
#include "resource.h"
#include "WinWinFunctions.h"

class WindowsApp : public BaseWindow<WindowsApp> {

public:
    WindowsApp();


    ///   GLOBALS   ///
    //Index for the StackWindows() multilayered stack function
    int stackIndex;
    
    // Vector of saved window layout buttons
    std::vector<HWND> layoutButtons;
    // Vector of saved desktop icon layout buttons
    std::vector<HWND> desktopLayoutButtons;

    // Main window class name
    PCWSTR ClassName() const { return L"Windows Window Extension"; }
    
    // Window procedure for the Window Control window
    WNDPROC windowWindowProc;
    // Window procedure for the active windows control panel 
    WNDPROC controlWindowProc;

    WNDPROC scrollBarProc;

    
    ///   STRUCTS   ///

    // Struct representation of a JSON saved window
    struct SavedWindow {
        SavedWindow(std::string process,
            UINT flags,
            UINT showCmd,
            POINT ptMinPosition,
            POINT ptMaxPosition,
            RECT rcNormalPosition) {
            m_process = process;
            m_flags = flags;
            m_showCmd = showCmd;
            m_ptMinPosition = ptMinPosition;
            m_ptMaxPosition = ptMaxPosition;
            m_rcNormalPosition = rcNormalPosition;
        };
        std::string m_process;
        UINT m_flags;
        UINT m_showCmd;
        POINT m_ptMinPosition;
        POINT m_ptMaxPosition;
        RECT m_rcNormalPosition;
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

    // Struct representation of a JSON saved desktop icon
    struct SavedIcon {
        SavedIcon(std::string iconName,
            POINT iconPos) {
            m_iconName = iconName;
            m_iconPos = iconPos;
        };
        std::string m_iconName;
        POINT m_iconPos;
    };
    

    ///   CALLBACK FUNCTIONS   ///
    
    /**
    * @brief Starts the loop that process and dispatches main window messages 
    *
    * This function is called after initializing the app. Anytime the window receives a message, this function
    * gets it, translates it, and dispatches it to the appropriate procedure
    */
    void RunMessageLoop();

    /**
    * @brief Processes messages sent to the main window 
    * 
    * Recieves all messages sent to the main window through the WindowProc callback function and calls the corresponding 
    * message based on a switch statement. Handles the basic window commands CREATE, PAINT, SIZE, SCROLL, CLOSE, and DESTROY
    * as well as button messages through the WM_COMMAND message. If the HIWORD of the wParam is BN_CLICKED, it gets the button id
    * and through a switch statement matches it to one of 16 preprocessor defined messages. All of these call their corresponding
    * method, but the following have additional functionality:
    * NEXT_STACK: increments the global stackIndex by one
    * PREV_STACK: decrements the global stackIndex by one
    * EXECUTE_LAYOUT: Gets the text of the pressed button through its HWND (passed through the lParam) and passes it as a wchar_t to ExecuteSaved
    * EXECUTE_DESKTOP_LAYOUT:  Gets the text of the pressed button through its HWND (passed through the lParam) and passes it as a wchar_t to ExecuteSavedDesktopLayout
    * For the preprocessor definitions of command IDs, see the top of WindowsApp.cpp
    */
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    /**
     * @brief Procedure that sends button clicked events to the main WindowProc
     * 
     * Callback method that receives all messages sent by buttons that are children of m_hwnd. If the message is WM_COMMAND, it relays that 
     * message to the main WindowProc
     */
    static LRESULT CALLBACK ButtonProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


    ///   INITIALIZE   ///

    /**
    * @brief Creates the main window
    * 
    * Creates the main window through the Create method of BaseWindow.cpp, then loads the menu and assigns it to m_hwnd.
    * If m_hwnd fails to Create, returns HRESULT S_FAIL
    */
    HRESULT Initialize();

    // Called upon window creation. Creates all child windows of m_hwnd
    /**
    * @brief Creates the active windows control panel, calls CreateControlOpts, and sets the initial WindowControl vector
    * 
    * Creates m_hActiveWindowsControlPanel which contains the UI control panel elements for each WindowControl in WindowsVector.
    * Next calls CreateControlOpts (see below), populates the WindowsVector, and sets each control panel to hidden.
    * Also sets the scroll info for the main window, but this doesn't do anything yet.
    * Returns S_OK 
    */
    HRESULT HandleCreate();
 
    /**
    * @brief Creates the child windows and buttons of m_hwnd
    * 
    * Creates the initial hierarchy of m_hwnd child windows, visualized below:
    * 
    *            m_hIconControlPanel                                             m_hWindowsControlPanel
    *             (iconWindowProc)                                                 (windowWindowProc)
    *                     |                                                                 |
    *            _________|________                            _____________________________|___________________________
    *            |        |        |                           |                 |          |        |                 |
    *            |   m_IconTitle   |                           |                 |    m_WindowTitle  |                 |
    *            |      (text)     |                           |                 |        (text)     |                 |        
    * m_hSavedDesktopConfigs  m_hSaveDesktopLayout       m_hCascadeButton  m_hStackButton    m_hSaveWinLayout   m_hSavedConfigs             
    *        (button)           (button)                     (button)         (button)            (button)          (button) 
    *                                          
    *                                     m_hActiveWindowsControlPanel
    *                                          (controlWindowProc)
    *                                                  |
    *                                            m_hShowWindows 
    *                                               (button)
    * 
    * Each top level window has a control proc that sends messages to ButtonProc
    */
    void CreateControlOpts();


    ///   UTILITIIES   ///
    /**
    * @brief Returns a vector of HWNDS from a vector of WindowsControls
    * 
    * For each WindowControl in the windowControls vector, add the HWND returned by GetInstanceHandle() of the control to the vector hwnd
    * and return hwnd.
    * 
    * @param windowControls - The WindowControls vector to convert
    * @return The corresponding vector of HWNDS 
    */
    std::vector<HWND> ExtractHwnds(std::vector<WindowControl*> windowControls);

    /**
     * @brief Converts an std::wstring to an std::string
     * 
     * Checks if wstr is empty, gets the size needed for the new string, creates a string of size size_needed, assigns the characters of the 
     * wstring to the string, and removes the null terminator from the end of the new string.
     * 
     * @param wstr The wide string to convert
     * @return The converted narrow string
     */
    std::string ConvertToNarrowString(const std::wstring& wstr);


    /**
     * @brief Manually triggers the WM_SIZE event without changing the window size
     */
    void TriggerResize();
        

    ///   WINDOW RESOURCES   ///

    /**
     * @brief Creates a dialog box to get user input
     * 
     * Creates a dialog box and assigns the user's input to the global userInput and returns userInput
     * 
     * @param hInstance hInstance of the calling process
     * @return The user's input as an std::wstring
     */
    std::wstring GetUserInput(HINSTANCE hInstance);


    ///   HANDLE WINDOWS_WINDOWS EVENTS   ///
    //    PRINT ACTIVE WINDOWS   //  

    /**
    * @brief Enumerate through the windows open on the machine and create a control panel for each of them
    * 
    * Using EnumWindows, calls the EnumWindowsProc callback from BaseWindow.cpp
    */
    void PrintActiveWindows();

    /**
     * @brief Shows a dropdown list of control panels for each open window
     * 
     * Clears the WindowsVector and repopulates it, then resizes m_hwnd (the main window) and m_hActiveWindowsControlPanel (the control window)
     * according to the number of control panels to display. m_hwnd is set to the same width as before but the height of the size of 
     * ((WindowsVector + 1) * 100) + 102 and m_hActiveWindowsControlPanel is set to the previous position, previous width, and the same height as m_hwnd
     * Then it destroys the m_hShowWindows button and creates an equally sized m_hHideWindows button. After that, it sets all control panels to visible.
     */
    void WinWinShowActive();
    
    /**
     * @brief Hides the active windows control panel dropdown
     * 
     * Sets each window control to hiden, and then sets the height of m_hActiveWindowsControlPanel to 30 and the height of m_hwnd to the client coordinate of
     * the m_hWindowsControlPanel + 100. Afterwards, it destroys the m_hHideWindows button and recreates the m_hShowWindows button 
     */
    void WinWinHideActive();


    //   STACK WINDOWS   //

        /**
    * @brief Stacks all active window
    * 
    * Called when STACK is clicked. If there are 8 or fewer windows open, it delegates the stacking to the static WinWinFunctions Stack() function. If there are more than 8, it initializes 
    * a multi layered stack by setting the stackIndex to 0, calling StackWindowsCallback, destroying the m_hStackButton button, and creating m_hExitStack, m_hNextStack,
    * and (a disabled) m_hPrevStack. 
    */
    void StackWindows();

    /**
     * @brief Handles multilayered stacking
     * 
     * Called when STACK is clicked and there are more than 8 windows open. First, determines the size of the final stack layer and uses this to determine how many layers there are.
     * If the global stackIndex is 0, m_hPrevStack is disabled. If stackIndex is equal to one less than the number of layers in the stack, m_hNextStack is disabled.
     * Next, it adds the next 8 WindowControls from WindowsVector to a SubVector (or the final number of WindowsControls if it's the last layer) and passes the subvector to 
     * StackFourOrLess or StackFiveToEight.
     */
    void StackWindowsCallback();

    /**
     * @brief Stacks four or less windows vertically on the screen
     * 
     * Based on the number of WindowControls in the SubVector, sets stackFactorY that is equal to the height of the screen - the size of the SubVector.
     * It then makes sure the window is not minimized or maximized, sets the window to TOPMOST, and sets the x coordinate to 0, the y coordinate equal to the stack position based on the stackFactor, 
     * the length to the length of the screen, and the height to the stackFactorY.
     * It then removes the TOPMOST flag and increments the stackPosition by the stackFactorY.
     * @param SubVector The vector of Windows to stack
     */
    void StackFourOrLess(std::vector<WindowControl*> SubVector);
    
    /**
     * @brief Stacks five to eight windows vertically and horizontally on the screen 
     * 
     * Based on the number of WindowControls in the SubVector, sets stackFactorY that is equal to the height of the screen minus the size of the SubVector.
     * It then makes sure the window is not minimized or maximized, sets the window to TOPMOST, sets the x coordinate to 0 or half of the length of the screen if it is the fifth or more window to be stacked,
     * the y coordinate equal to the stack position based on the stackFactor, the length to half length of the screen, and the height to the stackFactorY.
     * It then removes the TOPMOST flag and increments the stackPosition by the stackFactorY.
     * After the 4th window is stacked, the stackFactorY is reset and revaluated based on the remaining number of windows to stack and the x position of the remaining windows is set to half the length of the screen.
     * @param SubVector The vector of Windows to stack
     */
    void StackFiveToEight(std::vector<WindowControl*> SubVector);

    /**
     * @brief Removes the Prev Stack, Next Stack, and Exit stack buttons
     * 
     * Called when EXIT STACK is clicked. Destroys m_hPrevStack, m_hNextStack, and m_hExitStack, replacing m_hExitStack with a new m_hStackButton.
     */
    void ExitStack();


    //   CASCADE WINDOWS   //

    /**
    * @brief Arranges all windows in a cascaded view
    * 
    * Called when CASCADE is clicked. Calls the WinWinFunctions function Cascade and creates the squish button.
    */
    void CascadeWindows();

    /**
     * @brief Minimizes all windows not moved after a cascade
     * 
     * Called when SQUISH is clicked. Calls the WinWinFunctions function Squish.
     */
    void SquishCascade();


    //   SAVE WINDOW LAYOUTS   //

    /**
    * @brief Saves the current window layout to a JSON file
    * 
    * Called when SAVE LAYOUT button is pressed. Calls the WinWinFunctions function SaveWindowLayout.
    */
    void WinWinSaveLayout();

    /**
     * @brief Displays a dropdown of buttons that activate window layouts
     * 
     * Opens the SavedLayouts folder and creates a button for each JSON file inside. It then rearranges the control windows in the following order: 

     * 1. Get the position of the old WindowsControl rect   
     * 
     * 2. Get the position of the old m_hwnd  
     * 
     * 3. Get the position of the new WindowsControl rect  
     *      a. x - 0  
     *      b. y - 0 | SWP_NOMOVE  
     *      c. cx - old WindowControlRectRight - old WindowControlRectLeft  
     *      d. cy - yPos + 40 for last button + buffer  
     * 
     * 4. set the position of the new m_hwnd  
     *      a. x - 0  
     *      b. y - 0 | SWP_NOMOVE  
     *      c. cx - old m_hwnd right - old m_hwnd left  
     *      d. cy - old m_hwnd bottom + yPos + 40 

     * 5. set the position of the new ActiveWindowsRect  
     *      a. x - 0  
     *      b. y - Bottom of new WindowsControl rect  
     *      c. cx - 0  
     *      d. cy - 0 | SWP_NOSIZE  
     * 
     * 6. Set the position of the new WindowsControl rect  
     * 
     * After the windows are resized, it generates the buttons by iterating over the directory and places three buttons next to each other every thirty pixels 
     * before moving to the next line and repeating the process until it reaches the end of the directory.
     * It then destroys the m_hSavedConfigs and creates a m_hHideSavedConfigs in its place.
     */
    void WinWinViewSaved();

    /**
     * @brief Hides the saved window layout buttons 
     * 
     * Destroys each of the layout buttons by iterating through the layout buttons vector, destroys them_hHideSavedConfigs button, and recreates the m_hSavedConfigs button. 
     * Then, it resets the control panels to their previous states. 
     */
    void WinWinHideSaved();

    /**
     * @brief Moves Windows to the position saved in the specified JSON file
     * 
     * Called when a Saved Window Layout button is pressed. Calls the WinWinFunctions function ExecuteWindowLayout.
     * @param json The name of the JSON file to execute
     */
    void ExecuteSaved(std::wstring json);
    
    
    //   SAVE DESKTOP ICON LAYOUTS   //

   /**
    * @brief Saves the current desktop icon layout to a JSON file
    *
    * Called when SAVE ICONS button is pressed. Calls the WinWinFunctions function SaveDesktopLayout.
    */
    void SaveDesktopLayout();

    /**
     * @brief Creates a dropdown of buttons that activate different desktop icon layouts
     *
     * Rearranges the control windows in the following order:
     * m_hwnd : 
     *  length - same as before
     *  height - previous height + the difference in height of the new icon control panel
     * m_hActiveWindowsControlPanel :
     *  y coordinate - previous y coordinate + the difference in height of the new icon control panel
     * m_hWindowsControlPanel : 
     *  y coordinate - previous y coordinate + the difference in height of the new icon control panel
     * m_hIconControlPanel :
     *  length - same as before
     *  height - the y position of the last icon button + 40
     * 
     * It then iterates through the SavedDesktopLayouts directory and places three buttons horizontally spaced 30 pixels apart
     * on each row until it gets to the end of the directory
     */
    void ViewSavedDesktopLayouts();

    /**
     * @brief Hides each of the saved icon buttons 
     * 
     * Destroys every saved icon button by iterating through the desktopLayoutButtons vector, destroys m_hHideSavedDesktopConfigs, and recreates m_hSavedDesktopConfigs.
     * It then restores each control panel to its previous position.
     */
    void HideSavedDesktopLayouts();

  /**
   * @brief Moves desktop icons to the position saved in the specified JSON file
   *
   * Called when a Saved Icon Layout button is pressed. Calls the WinWinFunctions function ExecuteDesktopLayout.
   * @param json The name of the JSON file to execute
   */
    void ExecuteSavedDesktopLayout(std::wstring json);


private:

    ///   HANDLE BASIC WINDOW EVENTS   ///

    // Called when the paint event is hit
    void HandlePaint();

    // Called on window resize
    void HandleResize();

    // Called on window scroll
    void HandleScroll(WPARAM wParam, LPARAM lParam);


    ///   CHILD WINDOW HANDLES   ///

    // Main control panel window handle
    HWND m_hControlOptions;

    // Cascade button window handle
    HWND m_hCascadeButton;

    // Stack button window handle
    HWND m_hStackButton;

    // Next Stack button window handle
    HWND m_hNextStack;

    // Previous Stack button window handle
    HWND m_hPrevStack;

    // Exit multilayered stack mode button window handle
    HWND m_hExitStack;

    // Save layout button window handle
    HWND m_hSaveWinLayout;

    // Show active windows button window handle
    HWND m_hShowWindows;

    // Hide active windows button window handle
    HWND m_hHideWindows;

    // Show saved window configurations button window handle
    HWND m_hSavedConfigs;

    // Hide saved window configurations button window handle
    HWND m_hHideSavedConfigs;

    // Save desktop icons button window handle
    HWND m_hSaveDesktopLayout;

    // Show saved desktop icons button window handle
    HWND m_hSavedDesktopConfigs;

    // Hide saved desktop icons button window handle
    HWND m_hHideSavedDesktopConfigs;

    // Squish button window handle
    HWND m_hSquish;

    // Title text for the Windows control panel
    HWND m_WindowTitle;

    // Title text for the Icon control panel
    HWND m_IconTitle;

    // Icon control Window handle
    HWND m_hIconControlPanel;

    // Windows control window handle
    HWND m_hWindowsControlPanel;

    HWND m_hScrollBar;
};