# Prompt the user for a specified file name
$file_name = Read-Host "Please enter the name of the record file"

# create the credentials file
$credential_creation_command = 'C:\Users\Data_Computer\anaconda3\Scripts\conda.exe  run -n HPHT_DATA_LOGGER_ENV python ../Scripts/CreateCredentials.py'
Invoke-Expression $credential_creation_command

# set the current working directory
Set-Location "c:\Users\Data_Computer\Documents\HPHT\logger_app\PowerShell"

# define the commands to exectue the script in the proper python environmet
$arguments_1min = 'C:\Users\Data_Computer\Documents\HPHT\logger_app\PowerShell\Logger1Min.ps1 ' + $file_name
$arguments_1sec = 'C:\Users\Data_Computer\Documents\HPHT\logger_app\PowerShell\Logger1Sec.ps1 ' + $file_name
$arguments_10sec = 'C:\Users\Data_Computer\Documents\HPHT\logger_app\PowerShell\Logger10Sec.ps1 ' + $file_name
$arguments_datataker = 'C:\Users\Data_Computer\Documents\HPHT\logger_app\PowerShell\LoggerDataTaker.ps1 ' + $file_name
$arguments_growth =  'C:\Users\Data_Computer\Documents\HPHT\logger_app\PowerShell\LoggerGrowth.ps1 '+ $file_name

# define the start time for the tasks
$start_date = (Get-Date).AddMinutes(1)

# define the upload characteristics
$upload_period_1min = (New-TimeSpan -Minutes 60)
$upload_period_1sec = (New-TimeSpan -Minutes 15)
$upload_period_10sec = (New-TimeSpan -Minutes 60)
$upload_period_datataker = (New-TimeSpan -Minutes 30)
$upload_period_growth = (New-TimeSpan -Minutes 30)

# define actions for a new scheduled task
$action_1sec = New-ScheduledTaskAction -Execute 'powershell.exe' -Argument $arguments_1sec
$action_1min = New-ScheduledTaskAction -Execute 'powershell.exe' -Argument $arguments_1min
$action_10sec = New-ScheduledTaskAction -Execute 'powershell.exe' -Argument $arguments_10sec
$action_datataker = New-ScheduledTaskAction -Execute 'powershell.exe' -Argument $arguments_datataker
$action_growth = New-ScheduledTaskAction -Execute 'powershell.exe' -Argument $arguments_growth

# set up the task triggers
$trigger_1sec =  New-ScheduledTaskTrigger -Once -At $start_date -RepetitionInterval $upload_period_1sec
$trigger_10sec =  New-ScheduledTaskTrigger -Once -At $start_date -RepetitionInterval $upload_period_10sec
$trigger_1min =  New-ScheduledTaskTrigger -Once -At $start_date -RepetitionInterval $upload_period_1min
$trigger_datataker = New-ScheduledTaskTrigger -Once -At $start_date -RepetitionInterval $upload_period_datataker
$trigger_growth = New-ScheduledTaskTrigger -Once -At $start_date -RepetitionInterval $upload_period_growth

# scheudl the tasks
Register-ScheduledTask -Trigger $trigger_10sec -Action $action_10sec -TaskName "HPHT_LOGGER_10SEC" 
Register-ScheduledTask -Trigger $trigger_1sec -Action $action_1sec -TaskName "HPHT_LOGGER_1SEC"
Register-ScheduledTask -Trigger $trigger_1min -Action $action_1min -TaskName "HPHT_LOGGER_1MIN" 
Register-ScheduledTask -Trigger $trigger_datataker -Action $action_datataker -TaskName "HPHT_LOGGER_DATATAKER" 
Register-ScheduledTask -Trigger $trigger_growth -Action $action_growth -TaskName 'HPHT_LOGGER_GROWTH'

#Start FTP server
wsl -d AlmaLinux-8 sudo pkill -9 -f /sbin/vsftpd
wsl -d AlmaLinux-8 sudo /sbin/vsftpd /etc/vsftpd/vsftpd.conf




#Start merged CSV program
$arguments_CSV = 'C:\Users\Data_Computer\Documents\HPHT\logger_app\PowerShell\startCSV.ps1 ' + $file_name
$action_CSV = New-ScheduledTaskAction -Execute 'powershell.exe' -Argument $arguments_CSV
$trigger_CSV = New-ScheduledTaskTrigger -Once -At $start_date
Register-ScheduledTask -Trigger $trigger_CSV -Action $action_CSV -TaskName "START_CSV"

#Start dashboard
$arguments_dashboard = 'C:\Users\Data_Computer\Documents\HPHT\logger_app\PowerShell\startDashboard.ps1 ' + $file_name
$action_dashboard = New-ScheduledTaskAction -Execute 'powershell.exe' -Argument $arguments_dashboard
$trigger_dashboard = New-ScheduledTaskTrigger -Once -At $start_date
Register-ScheduledTask -Trigger $trigger_dashboard -Action $action_dashboard -TaskName "START_DASHBOARD"


