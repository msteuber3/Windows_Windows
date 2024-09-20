# Windows_Windows

Expand windows customization options in windows

There are two ways to use the Windows Windows functions provided by this application: the command line and the UI.

## Command line
In the same folder as Windows_Windows.exe, there is a Powershell script called setup.ps1. Run this to automatically add the executable to the system path, and you can access the Windows_Windows commands with the alias WinWin.    
After running the script, use `WinWin [command] [params]` to call the functions.  
If you call the executable WITH arguments, it will not start the UI.   
   
### The following commands are currently compatible with Windows Windows:   
`stack` - Stacks the currently open windows to fill the size of your main monitor. Command line stack supports a maximum of 8 windows in the stack. When four or less windows are open, each window will be stacked vertically and stretched to fill the length of the screen. For more than four windows, the screen will be divided in half and two stacks will be created.  
   
`cascade`  - Arranges the active windows in a cascade. This resizes each of them into a square and sets the position of the top left corner of each window to descend diagnolly spaced about 50 pixels apart starting in the top left corner of the screen.  
   
`squish`  - Following a cascade, if squish is called any window that has not been moved will be minimized.  
   
`SaveLayout` - Saves the current window layout to a JSON file. Calling this command opens a dialog box in which you can input a name for your saved layout.    
   
`ExecuteLayout [params]` - Restores the currently opened windows to the positions specified in your saved layout. This command takes the layout you would like to execute as input exactly as you entered it in the SaveLayout dialog box.    
   
`SaveDesktop` - Saves the current desktop icon layout to a JSON file. Calling this command opens a dialog box in which you can input a name for your saved layout.      
   
`ExecuteDesktop [params]` - Restores your desktop icons to the positions specified in your saved layout. This command takes the layout you would like to execute as input exactly as you entered it in the SaveDesktop dialog box.  
   
## UI  
The UI contains all of the functionality of the command line with some additional visual features 
For an explanation of the various WinWin functions that the UI buttons call upon, see above. 

### Window controls - 
The primary difference between the functionality provided by the command line and the UI is how the program stores the list of active windows. While the command line simply passes each function a new vector of window handles on each run, the UI stores a list of "Window Controls" that allow for additional control over your open windows.   

#### Control list - 
The list of window controls can be viewed by pressing the dropdown ("V" button) at the bottom of the application. Each control corresponds to an open window and contains a minimize, maximize, and close button for that particular window.  
In addition to displaying the controls, the dropdown button also refreshes the active window list every time it is activated. Unlike the command line, the control list is only updated when this button is pressed, so if you open or close a window during the runtime of this application and you'd like to include it in the commands you have to refresh the list. This allows you to exclude windows from your WinWin operations by waiting until after starting WinWin to open them.

### Multilayered stacking -  
When you view more than 8 windows in stack mode through the command line, WinWin does not stack any windows beyond the first 8 because command line commands are run as static functions that do not store any application state data. Since the UI does store a list of open windows, stacking more than 8 windows opens up multilayered stack controls underneath the stack button. 'Stack' groups the windows into sublists of 8 windows and displays the first, with the buttons allowing you to scroll through your window sublists.  
   

## Notes
- WinWin is not able to manipulate the task manager. While it will appear in your control list, the task manager does not respond to window resize messages.
- WinWin will NOT reopen closed programs when you execute a desktop layout. If you execute a desktop layout that includes closed programs, WinWin will simply skip these and leave the position they were in empty.
- Since the command line commands are static functions that do not store application state info, multilayered stacking is not a feature of the command line stack. Calling stack on the command line when more than 8 windows are open only stacks the first 8, ignoring the others.
- When executing a saved desktop layout, WinWin preforms 3 checks to match stored windows to active windows:
  - First, it checks the window handles. Any windows you kept open since saving the layout will be matched to its handle.
  - Second, it checks the window title. If you have multiple windows with identical titles (ex. two RDP windows connected to the same host) their positions may be swapped when you execute the layout. If this is an issue, the easiest solution is to execute the layout, swap your two windows manually, and resave the layout so that WinWin can match them to their current handle.
  - Third, if both of the above fail, it checks the process running the window. This is primarily applicable to applications such as web browsers whose titles depend on the open tab. For this reason, multiple isntances of a web browser may be swapped (although if you don't have the same tabs open as when you saved the layout originally, this will likely not be noticable). 
    - Because of this, you can give an application priority in the window arrangment by activating it and refreshing the Window Control list. For example, if you have two firefox windows open and would like one in the top left corner and the other in the bottom right, you can click on the window you'd like in the top left, resfresh the control, and execute the layout.