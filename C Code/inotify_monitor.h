#ifndef INOTIFY_MONITOR_H
#define INOTIFY_MONITOR_H

#include "Logger.h"
#include "append_CSV.h"
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <ctime>
#include <thread>
#include <chrono>

// Function declarations
std::string getLastLine(const std::string& filename, Logger& logger);
std::string extractField(const std::string& logLine, int fieldNum, Logger& logger);
std::string getFilename(const std::string& path);
void handleEvent(uint32_t mask, const std::string& monitored_filename, const std::string& merged_CSV_filename, Logger& logger);

#endif // INOTIFY_MONITOR_H
