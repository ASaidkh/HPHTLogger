
Unregister-ScheduledTask -TaskName 'START_DASHBOARD' -Confirm:$false
Unregister-ScheduledTask -TaskName 'START_CSV' -Confirm:$false

#Unregister-ScheduledTask -TaskName 'FTP_CSV_Program' -Confirm:$false


# delete the upload credentials
Remove-Item -Path '..//Creds//mycreds.txt'

#Stop merged CSV program
wsl -d AlmaLinux-8 pkill -9 -f /home/NeoHPHTLogger/Code/./DataTaker_FTP_CSV

#Stop dashboard
wsl -d AlmaLinux-8 pkill -9 -f /home/NeoHPHTLogger/Code/dashboard.py
