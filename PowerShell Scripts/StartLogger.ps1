# Prompt the user for a specified file name
$file_name = Read-Host "Please enter the name of the record file"

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


