#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <iostream>
#include <cstring> 
#include <chrono>

class Logger {
public:
    Logger(const std::string& filename);
    ~Logger();

    void log(const std::string& message);
    void log_error(const std::string& message);
    bool copy_LOGFILE(const std::string& destination_filename);
    void start_periodic_copying(const std::string& destination_directory, const std::string& filename, std::chrono::minutes interval);

private:
    std::ofstream log_file;
    std::string log_file_name;  // Store the log file name
};


#endif // LOGGER_H
