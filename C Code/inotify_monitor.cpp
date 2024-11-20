/*
Author: Alim Saidkhodjaev
Date: 07/24/2024
Modified: 08/13/2024
Description:  Inotify event handling
 */

#include "inotify_monitor.h"
#include "Logger.h"
#include "append_CSV.h"
#include "FileParser.h"  
#include <iostream>
#include <sys/inotify.h>
#include <unistd.h>
#include <cstring>
#include <errno.h>
#include <vector>
#include <sstream>
#include <thread>
#include <chrono>

#define CSV_FILEPATH_FIELDNUM 9
#define IP_ADDRESS_FIELDNUM 7
#define USER_FIELDNUM 14
#define DATATAKER_IP "10.1.10.192"


// Function to handle inotify events
void handleEvent(uint32_t mask, const std::string& monitored_filename, const std::string& merged_CSV_filename, Logger& logger) {
    if (mask & IN_OPEN) {
        logger.log("File opened: " + monitored_filename);
    }
    //Perform CSV merging after xferlog modification detected
    if (mask & IN_MODIFY) {
        logger.log("File modified: " + monitored_filename);
        //Extract last line of xferlog file
        std::string lastLine = getLastLine(monitored_filename, logger);
        logger.log("Last line of the file: " + lastLine);
        //Parse to get last uploaded csv file
        if (extractField(lastLine, USER_FIELDNUM, logger) == "DataTaker" && extractField(lastLine, IP_ADDRESS_FIELDNUM, logger) == DATATAKER_IP) {
            std::string extractedFilePath = extractField(lastLine, CSV_FILEPATH_FIELDNUM, logger);
            logger.log("Extracted file path: " + extractedFilePath);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            append_csv(extractedFilePath, merged_CSV_filename, logger);
        }
        else {
            logger.log("Error, FTP transfer from USER: " + extractField(lastLine, USER_FIELDNUM, logger) + ", IP: " + extractField(lastLine, IP_ADDRESS_FIELDNUM, logger));
        }
    }
    if (mask & IN_CLOSE_WRITE) {
        logger.log("File closed after writing: " + monitored_filename);
    }
    if (mask & IN_CREATE) {
        logger.log("File created: " + monitored_filename);
    }
    if (mask & IN_DELETE_SELF) {
        logger.log("File deleted: " + monitored_filename);
    }
}
