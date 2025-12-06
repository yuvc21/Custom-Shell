#include <cstdlib> //std::getenv
#include <filesystem>
#include <iostream>
#include <string>
#include <unistd.h> //access(), X_OK
#include<vector>

std::vector<std::string> split(const std::string &str, char delimeter) {
  std::vector<std::string> parts;
  std::string current;
  for (char c : str) {
    if (c == delimeter) {
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

std::string find_executables_in_path(const std::string &cmd_name) {
  const char *path_cstr = std::getenv("PATH");
  if (path_cstr == nullptr) {
    return "";
  }
  std::string path_env(path_cstr);
  char path_delimeter = ':';
  auto dirs = split(path_env, path_delimeter);
  for(const auto& dir: dirs){
    std::filesystem::path p = std::filesystem::path(dir) / cmd_name;
    // skip if doesn't exist or isn't a regular file
    if(!std::filesystem::exists(p) || !std::filesystem::is_regular_file(p)){
      continue;
    }
    // checking execute permission
    if(access(p.c_str(), X_OK) == 0){
      return p.string();
    }
  }
  return "";
}

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  while (1) {
    std::cout << "$ ";
    std::string input;
    std::getline(std::cin, input);
    if (input == "exit")
      break;
    else if (input.rfind("echo", 0) == 0) {
      std::string text = input.substr(4);

      if (!text.empty() && text[0] == ' ')
        text = text.substr(1);
      std::cout << text << std::endl;
    } else if (input.rfind("type", 0) == 0) {
      std::string text = input.substr(4);
      if (!text.empty() && text[0] == ' ') {
        text = text.substr(1);
      }
      if (text == "echo" || text == "exit" || text == "type") {
        std::cout << text << " is a shell builtin" << std::endl;
      } else if(!find_executables_in_path(text).empty()){
        std::cout << text << " is " << find_executables_in_path(text) << std::endl;
      }
       else
        std::cout << text << ": not found" << std::endl;
    }
     else
      std::cout << input << ": command not found" << std::endl;
  }
}
