#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

std::vector<std::string> split(const std::string& str, char delimiter);
std::string find_executables_in_path(const std::string& cmd_name);

extern std::string previous_directory;  // For cd -

#endif