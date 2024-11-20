/*
Author: Alim Saidkhodjaev
Created: 07/27/2024
Modified: 08/13/2024
Description:  Logger class for writing event and error messages to a file
 */
#include "Logger.h"
#include <chrono>
#include <iomanip>
#include <fstream>
#include <thread>
#include <sstream>  // Include this to use std::ostringstream

// Constructor
Logger::Logger(const std::string& filename) 
    : log_file(filename, std::ios::app), log_file_name(filename) {
    if (!log_file.is_open()) {
        std::cerr << "Error opening log file: " << filename << std::endl;
    }
}

// Close logger
Logger::~Logger() {
    if (log_file.is_open()) {
        log_file.close();
    }
}

// Method to log event message
void Logger::log(const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::tm buf;
    localtime_r(&in_time_t, &buf);

    log_file << "[" << std::put_time(&buf, "%Y-%m-%d %H:%M:%S")
             << '.' << std::setfill('0') << std::setw(3) << ms.count() << "] "
             << message << std::endl;
}

// Method to log error message
void Logger::log_error(const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::tm buf;
    localtime_r(&in_time_t, &buf);

    log_file << "[" << std::put_time(&buf, "%Y-%m-%d %H:%M:%S")
             << '.' << std::setfill('0') << std::setw(3) << ms.count() << "] ERROR: "
             << message << std::endl;
}

// Method to copy log file to a destination
bool Logger::copy_LOGFILE(const std::string& destination_filename) {
    log_file.flush();  // Ensure all data is written to the file before copying

    std::ifstream src(log_file_name, std::ios::binary);  // Open the source file using the filename
    if (!src.is_open()) {
        std::cerr << "Error opening source log file for copying." << std::endl;
        return false;
    }

    std::ofstream dest(destination_filename, std::ios::binary);  // Open the destination file
    if (!dest.is_open()) {
        std::cerr << "Error opening destination file: " << destination_filename << std::endl;
        return false;
    }

    dest << src.rdbuf();  // Copy file content

    return true;
}

// Method to start periodic log file copying
void Logger::start_periodic_copying(const std::string& destination_directory, const std::string& filename, std::chrono::minutes interval) {
    std::thread([this, destination_directory, filename, interval]() {
        while (true) {
            std::this_thread::sleep_for(interval);
            auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            std::tm buf;
            localtime_r(&now, &buf);

            std::ostringstream oss;
            oss << destination_directory << filename << "_EVENTLOG.log" ;

            if (!copy_LOGFILE(oss.str())) {
                std::cerr << "Failed to copy log file to " << oss.str() << std::endl;
            }
        }
    }).detach();  // Detach the thread to run independently
}
