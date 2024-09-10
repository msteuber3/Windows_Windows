# Get the directory of the script
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition

# Get the current system PATH
$path = [System.Environment]::GetEnvironmentVariable("Path", [System.EnvironmentVariableTarget]::Machine)

# Check if the script directory is already in the PATH
if ($path -notlike "*$scriptDir*") {
    # Add the script directory to the PATH
    $newPath = "$path;$scriptDir"
    [System.Environment]::SetEnvironmentVariable("Path", $newPath, [System.EnvironmentVariableTarget]::Machine)
    Write-Output "Path updated successfully."
} else {
    Write-Output "Path already contains the script directory."
}

# Define the profile path for the current user
$profilePath = $PROFILE.CurrentUserAllHosts

# Create the profile file if it doesn't exist
if (-not (Test-Path -Path $profilePath)) {
    New-Item -ItemType File -Path $profilePath -Force
    Write-Output "Profile file created at $profilePath."
}

# Create an alias for the executable
$aliasCommand = "Set-Alias -Name WinWin -Value `"$scriptDir\Windows_Windows.exe`""

# Add the alias to the profile script if it doesn't already exist
if (-not (Get-Content $profilePath | Select-String -Pattern "Set-Alias -Name WinWin")) {
    Add-Content -Path $profilePath -Value $aliasCommand
    Write-Output "Alias 'WinWin' created successfully."
} else {
    Write-Output "Alias 'WinWin' already exists."
}
