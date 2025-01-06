X$file_name = $args[0]
Write-Output "Starting dashboard: " + $file_name


# define dashboard wsl command to start
$execution_command = "wsl -d AlmaLinux-8 nohup python3 EXECUTABLE_FILE_PATH DATA_FILE_PATH" + $file_name + ".csv" 

Invoke-Expression $execution_command
