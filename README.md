# HPHTLogger
DataTaker Logging system:

Linux server set up on a WSL that a DataTaker sends its logged data to every 5 seconds as a CSV. C program that uses inotify to monitor the xferlog file which contains FTP requests which includes IP addresses, user info, and files transferred. Each time a file is transferred, if it is a CSV from DataTaker, the program will merge the data to the current CSV file used to log combined data. Also includes a python dashboard that allows visualization of the data.

The logger is started by running the StartLogger PowerShell script. User will be prompted for a file name to write to.

The logger can be stopped by running the StopLogger Powershell script. 



**Note: Could be repurposed for logging of other systems, but primarily designed for the server and DataTaker at Euclid Techlabs. Requires changing of file paths and some settings. 

