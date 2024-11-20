# Disable the logging tasks
Disable-ScheduledTask -TaskPath '\' -TaskName 'HPHT_LOGGER_10SEC' 
Disable-ScheduledTask -TaskPath '\' -TaskName 'HPHT_LOGGER_1SEC' 
Disable-ScheduledTask -TaskPath '\' -TaskName 'HPHT_LOGGER_1MIN'
Disable-ScheduledTask -TaskPath '\' -TaskName 'HPHT_LOGGER_DATATAKER'
Disable-ScheduledTask -TaskPath '\' -TaskName 'HPHT_LOGGER_GROWTH'

# delete the logging tasks
Unregister-ScheduledTask -TaskName 'HPHT_LOGGER_10SEC' -Confirm:$false
Unregister-ScheduledTask -TaskName 'HPHT_LOGGER_1SEC' -Confirm:$false
Unregister-ScheduledTask -TaskName 'HPHT_LOGGER_1min' -Confirm:$false
Unregister-ScheduledTask -TaskName 'HPHT_LOGGER_DATATAKER' -Confirm:$false
Unregister-ScheduledTask -TaskName 'HPHT_LOGGER_GROWTH' -Confirm:$false
Unregister-ScheduledTask -TaskName 'START_DASHBOARD' -Confirm:$false
Unregister-ScheduledTask -TaskName 'START_CSV' -Confirm:$false

#Unregister-ScheduledTask -TaskName 'FTP_CSV_Program' -Confirm:$false


# delete the upload credentials
Remove-Item -Path '..//Creds//mycreds.txt'

#Stop merged CSV program
wsl -d AlmaLinux-8 pkill -9 -f /home/NeoHPHTLogger/Code/./DataTaker_FTP_CSV

#Stop dashboard
wsl -d AlmaLinux-8 pkill -9 -f /home/NeoHPHTLogger/Code/dashboard.py