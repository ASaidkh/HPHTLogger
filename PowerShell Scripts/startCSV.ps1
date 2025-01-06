# Prompt the user for a specified file name
$file_name = $args[0]
Write-Output "Starting CSV Program: " + $file_name


# define wsl command to start CSV program
$execution_command = 'wsl -d AlmaLinux-8 nohup EXECUTABLE_FILE_PATH /var/log/xferlog ' + $file_name 

Invoke-Expression $execution_command
