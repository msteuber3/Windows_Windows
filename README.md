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
   
