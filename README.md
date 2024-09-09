# Windows_Windows

Expand windows customization options in windows

There are two ways to use the Windows Windows functions provided by this application: the command line and the UI.

## Command line
To use the command line commands, you first have to call the executable. In the same directory as the executable, this can be done  
with `./Windows_Windows.exe [command]`. I reccomend adding the absolute path of the executable to the system PATH for simplicity.  
If you call the executable WITH arguments, it will not start the UI.  
  
The following commands are currently compatible with Windows Windows:
`stack` - Stacks the currently open windows to fill the size of your main monitor. Command line stack supports a maximum of 8 windows in the stack. When four or less windows are open, each window will be stacked vertically and stretched to fill the length of the screen. For more than four windows, the screen will be divided in half and two stacks will be created.  
   
`cascade`  - Arranges the active windows in a cascade. This resizes each of them into a square and sets the position of the top left corner of each window to descend diagnolly spaced about 50 pixels apart starting in the top left corner of the screen.  
   
`squish`  - Following a cascade, if squish is called any window that has not been moved will be minimized.  
   
`SaveLayout` - Saves the current window layout to a JSON file. Calling this command opens a dialog box in which you can input a name for your saved layout.    
   
`ExecuteLayout [params]` - Restores the currently opened windows to the positions specified in your saved layout. This command takes the layout you would like to execute as input exactly as you entered it in the SaveLayout dialog box.    
   
`SaveDesktop` - Saves the current desktop icon layout to a JSON file. Calling this command opens a dialog box in which you can input a name for your saved layout.      
   
`ExecuteDesktop [params]` - Restores your desktop icons to the positions specified in your saved layout. This command takes the layout you would like to execute as input exactly as you entered it in the SaveDesktop dialog box.  
   
## UI  
The UI contains all of the functionality of the command line with some additional visual features
   
Besides buttons for each of the functions listed above, the UI contains a dropdown button that lists all of the user's active windows and provides minimize/maximize/close controls for each. 