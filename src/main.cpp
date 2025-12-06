#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

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
  for (const auto &dir : dirs) {
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

void handle_echo(const std::string &input) {
  std::string text = input.substr(4);
  if (!text.empty() && text[0] == ' ')
    text = text.substr(1);
  std::cout << text << std::endl;
}

void handle_type(const std::string &input) {
  std::string text = input.substr(4);
  if (!text.empty() && text[0] == ' ') {
    text = text.substr(1);
  }
  
  if (text == "echo" || text == "exit" || text == "type") {
    std::cout << text << " is a shell builtin" << std::endl;
  } else {
    std::string exec_path = find_executables_in_path(text);
    if (!exec_path.empty()) {
      std::cout << text << " is " << exec_path << std::endl;
    } else {
      std::cout << text << ": not found" << std::endl;
    }
  }
}

void handle_unknown_command(const std::string &input) {
  std::cout << input << ": command not found" << std::endl;
}

int main() {
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  while (1) {
    std::cout << "$ ";
    std::string input;
    std::getline(std::cin, input);
    
    if (input == "exit") {
      break;
    } else if (input.rfind("echo", 0) == 0) {
      handle_echo(input);
    } else if (input.rfind("type", 0) == 0) {
      handle_type(input);
    } else {
      handle_unknown_command(input);
    }
  }
}