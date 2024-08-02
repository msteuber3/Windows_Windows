# Windows_Windows

Expand windows customization options in windows



# TODO:

- [ x ] Save and Restore WinWinInfo properties to JSON files by WinWinId# number\
       Properties - 
    - [ x ] Open Windows application information (like executable or shortcut name)
    - [ x ]  Open Windows layout positions
    - [ ]  Desktop Icons layout positions


- [ ] Shortcuts to Execute the WinWinSave feature - 
    - [ x ] WinWinSave with unique WinWinId# as an argument saves all currently open Windows information required by the Restore feature\
        - ex: winwinsave 3 saves the current Windows layout to WinWinInfo3.json


- [ ] Shortcuts to Execute the WinWinRestore feature
    - [ ] Restore with unique WinWinId# as an argument
        - [ ] restores the Windows layout previously saved
        - [ ] ignores Windows that are currently open that are not referenced in the saved layout (i.e. does not close existing Windows)
        - ex: winwinrestore 3 restores the current Windows layout from WinWinInfo3.json
