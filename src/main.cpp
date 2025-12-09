#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include <sys/wait.h>
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

std::vector<std::string> tokenize(const std::string &input) {
  std::vector<std::string> tokens;
  std::string current;
  for (char c : input) {
    if (c == ' ' || c == '\t') {
      if (!current.empty()) {
        tokens.push_back(current);
        current.clear();
      }
    }else{
      current += c;
    }
  }
  if(!current.empty()){
    tokens.push_back(current);
  }
  return tokens;
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
extern char **environ;
void executeExternal(const std::vector<std::string> &args) {
  if(args.empty()) return;
  std::string command_path = find_executables_in_path(args[0]);
  if (command_path.empty()) {
    std::cout << args[0] << ": command not found" << std::endl;
    return;
  }
  // fork() creates a child process
  // returns 0  in child , child's PID in parent, -1 on error
  pid_t pid = fork();
  if (pid == -1) {
    // there was an error in creating a child process
    std::cerr << "Failed to fork\n";
    return;
  }
  if (pid == 0) {
    //  === CHILD PROCESS CODE ===
    std::vector<char *> argv;
    for (const auto &arg : args) {
      argv.push_back(const_cast<char *>(arg.c_str()));
    }
    // this is a null-terminated array
    // execve() replaces  current program with the new program
    argv.push_back(nullptr);
    execve(command_path.c_str(), argv.data(), environ);

    std::cerr << "Failed to execute " << args[0] << std::endl;
    exit(1);
  } else {
    // === PARENT PROCESS CODE ===
    int status;
    waitpid(pid, &status, 0);
  }
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
    }else {
        auto args = tokenize(input);
        executeExternal(args);
      }
  }
}