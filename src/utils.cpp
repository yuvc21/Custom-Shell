#include "utils.h"
#include <filesystem>
#include <cstdlib>
#include <unistd.h>

std::string previous_directory; 

std::vector<std::string> split(const std::string& str, char delimiter) {
  std::vector<std::string> parts;
  std::string current;
  for (char c : str) {
    if (c == delimiter) {
      if (!current.empty()) {
        parts.push_back(current);
        current.clear();
      }
    } else {
      current += c;
    }
  }
  if (!current.empty()) {
    parts.push_back(current);
  }
  return parts;
}

std::string find_executables_in_path(const std::string& cmd_name) {
  const char* path_cstr = std::getenv("PATH");
  if (path_cstr == nullptr) {
    return "";
  }
  std::string path_env(path_cstr);
  auto dirs = split(path_env, ':');
  
  for (const auto& dir : dirs) {
    std::filesystem::path p = std::filesystem::path(dir) / cmd_name;
    if (!std::filesystem::exists(p) || !std::filesystem::is_regular_file(p)) {
      continue;
    }
    if (access(p.c_str(), X_OK) == 0) {
      return p.string();
    }
  }
  return "";
}