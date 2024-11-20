/*
Author: Alim Saidkhodjaev
Created: 07/24/2024
Modified: 08/13/2024
Description:  DataTaker logging program to monitor xferlog file for FTP events and merge newly uploaded CSV files to a larger CSV File

Usage: execute executable with arguments: {path to xferlog file} {filename of merged CSV file to generate}
 */

#include "Logger.h"
#include "append_CSV.h"
#include "inotify_monitor.h"
#include <iostream>
#include <sys/inotify.h>
#include <unistd.h>
#include <vector>
#include <cstring>
#include <limits.h>

#define CSV_FILE_PATH "/home/NeoHPHTLogger/Data/"
#define LOG_PATH "/home/NeoHPHTLogger/Logs/"
#define GD_BACKUP_FILE_PATH "/home/NeoHPHTLogger/Data/GD/"
#define LOG_BACKUP_MIN 5 
#define BACKUP_INTERVAL 240


#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <file to monitor> <base filename>" << std::endl;
        return 1;
    }

    //File paths
    std::string monitored_file = argv[1];
    std::string base_filename = argv[2];
    std::string merged_CSV_file = CSV_FILE_PATH + base_filename + ".csv";
    std::string backup_CSV_file = GD_BACKUP_FILE_PATH + base_filename + ".csv";
    std::string log_file = LOG_PATH + base_filename + "_EVENTLOG.log";

    //Counter for copying CSV to GD back up directory
    int backup_count = 0;

    //Instance logger
    Logger logger(log_file);
    logger.start_periodic_copying(GD_BACKUP_FILE_PATH, base_filename, std::chrono::minutes(LOG_BACKUP_MIN)); 

    // Initialize inotify
    int inotifyFd = inotify_init();
    if (inotifyFd == -1) {
        logger.log_error("Error initializing inotify: " + std::string(strerror(errno)));
        return 1;
    }

    // Add watch for a specific file
    int wd = inotify_add_watch(inotifyFd, monitored_file.c_str(), IN_OPEN | IN_MODIFY | IN_CLOSE_WRITE | IN_CREATE | IN_DELETE_SELF);
    if (wd == -1) {
        logger.log_error("Error adding inotify watch: " + std::string(strerror(errno)));
        close(inotifyFd);
        return 1;
    } else {
        logger.log("Monitoring file: " + monitored_file);
    }

    std::vector<char> buf(BUF_LEN);

    // Keep inotify monitoring xferlog file and perform CSV merge when new FTP transfers detected
    while (true) {
        ssize_t numRead = read(inotifyFd, buf.data(), BUF_LEN);
        if (numRead == 0) {
            logger.log_error("read() from inotify fd returned 0!");
            break;
        }

        if (numRead == -1) {
            logger.log_error("Error reading inotify events: " + std::string(strerror(errno)));
            break;
        }

        for (char* ptr = buf.data(); ptr < buf.data() + numRead; ) {
            struct inotify_event* event = (struct inotify_event*) ptr;
            handleEvent(event->mask, monitored_file, merged_CSV_file, logger);
            backup_count++;
            //Backup merged_CSV_file to GD after certain amount of events sync folder
            if (backup_count >= BACKUP_INTERVAL) {
                copy_CSV(merged_CSV_file, backup_CSV_file, logger);
                backup_count = 0;
            }
            ptr += sizeof(struct inotify_event) + event->len;
        }
    }

    // Clean up
    close(wd);
    close(inotifyFd);
    return 0;
}
