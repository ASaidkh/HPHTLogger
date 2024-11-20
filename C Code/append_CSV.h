#ifndef APPEND_CSV_H
#define APPEND_CSV_H

#include <string>
#include <vector>
#include "Logger.h" // Include your Logger class header

// Function declarations
bool file_exists(const std::string& filename);
bool try_open_file(std::ifstream& file, const std::string& filename, Logger& logger);
bool try_open_file(std::ofstream& file, const std::string& filename, std::ios_base::openmode mode, Logger& logger);
std::vector<std::string> read_lines(const std::string& filename);
void append_csv(const std::string& source_file, const std::string& destination_file, Logger& logger);
void copy_CSV(const std::string& source_file, const std::string& destination_file, Logger& logger);

#endif // APPEND_CSV_H
