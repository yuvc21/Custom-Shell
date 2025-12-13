#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

struct Command {
  std::vector<std::string> args;    // command and it'a arguments
  std::string output_file;          // file for stdout redirection
  bool has_output_redirect = false; // flag for > ot 1>
};

Command parseCommand(const std::vector<std::string> &tokens) {
  Command cmd;
  for (size_t i = 0; i < tokens.size(); ++i) {
    if (tokens[i] == ">" || tokens[i] == "1>") {
      if (i + 1 < tokens.size()) {
        cmd.output_file = tokens[i + 1];
        cmd.has_output_redirect = true;
        ++i; // for skipping filename token..
      }
    } else {
      cmd.args.push_back(tokens[i]);
    }
  }
  return cmd;
}
std::string home_directory = std::getenv("HOME") ? std::getenv("HOME") : "";
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
  std::string current_token;
  bool in_single_quotes = false;
  bool in_double_quotes = false;
  for (size_t i = 0; i < input.size(); ++i) {
    char c = input[i];
    if (c == '\'' && !in_double_quotes) {
      in_single_quotes = !in_single_quotes;
      continue;
    } else if (c == '"' && !in_single_quotes) {
      in_double_quotes = !in_double_quotes;
      continue;
    } else if (c == ' ' && !in_single_quotes && !in_double_quotes) {
      if (!current_token.empty()) {
        tokens.push_back(current_token);
        current_token.clear();
      }
      // handling the backslash with ans without quotes
    } else if (c == '\\' && i + 1 < input.length()) {
      char next = input[i + 1];
      if (in_single_quotes) {
        current_token += c;
      } else if (in_double_quotes) {
        if (next == '"' || next == '\\' || next == '$' || next == '`') {
          current_token += next;
          ++i;
        } else {
          current_token += c;
        }
      } else {
        current_token += next;
        ++i;
      }
    } else if (c == '>') {
      if (!current_token.empty() && current_token.back() == '1') {
        current_token.pop_back();
        if (!current_token.empty()) {
          tokens.push_back(current_token);
          current_token.clear();
        }
        tokens.push_back("1>");
      } else if (!current_token.empty() && current_token.back() == '2') {
        current_token.pop_back();
        if (!current_token.empty()) {
          tokens.push_back(current_token);
          current_token.clear();
        }
        tokens.push_back("2>");
      } else {
        if (!current_token.empty()) {
          tokens.push_back(current_token);
          current_token.clear();
        }
        tokens.push_back(">");
      }
    } else {
      current_token += c;
    }
  }
  if (!current_token.empty()) {
    tokens.push_back(current_token);
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
    // check if executing permssions are there
    if (access(p.c_str(), X_OK) == 0) {
      return p.string();
    }
  }
  return "";
}

void handle_echo(const std::string &input) {
  auto tokens = tokenize(input);
  for (size_t i = 1; i < tokens.size(); ++i) {
    if (i > 1)
      std::cout << ' ';
    std::cout << tokens[i];
  }
  std::cout << std::endl;
}

void handle_type(const std::string &input) {
  auto tokens = tokenize(input);
  if (tokens.size() < 2)
    return;
  std::string text = tokens[1];

  if (text == "echo" || text == "exit" || text == "type" || text == "pwd" ||
      text == "cd") {
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
void executeExternal(const Command &cmd) {
  if (cmd.args.empty())
    return;
  std::string command_path = find_executables_in_path(cmd.args[0]);
  if (command_path.empty()) {
    std::cout << cmd.args[0] << ": command not found" << std::endl;
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
    if (cmd.has_output_redirect) {
      int fd =
          open(cmd.output_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if (fd != -1) {
        dup2(fd, STDOUT_FILENO);
        close(fd);
      }
    }
    std::vector<char *> argv;
    for (const auto &arg : cmd.args) {
      argv.push_back(const_cast<char *>(arg.c_str()));
    }
    // this is a null-terminated array
    // execve() replaces  current program with the new program
    argv.push_back(nullptr);
    execve(command_path.c_str(), argv.data(), environ);

    std::cerr << "Failed to execute " << cmd.args[0] << std::endl;
    exit(1);
  } else {
    // === PARENT PROCESS CODE ===
    int status;
    waitpid(pid, &status, 0);
  }
}
void handle_exit(const std::string &input) {
  auto tokens = tokenize(input);
  int code = 0;
  if (tokens.size() > 1) {
    try {
      code = std::stoi(tokens[1]);
    } catch (const std::invalid_argument &) {
      std::cerr << "exit: " << tokens[1] << ": numeric argument required"
                << std::endl;
      code = 2;
    } catch (const std::out_of_range &) {
      std::cerr << "exit: " << tokens[1] << ": numeric argument required"
                << std::endl;
      code = 2;
    }
  }
  exit(code);
}
/*
stack<int> cd_directories;
void handle_cd(std::string &input) {
  vector<string> argv = tokenize(input);
  std::string current_directory = argv[1];
  if (argv.size() == 1) {
    std::filesystem::current_path().string() = original_path;
    while (!cd_directories.empty()) {
      cd_directories.pop();
    }
  } else if (current_directory == "..") {
    if (!st.empty()) {
      cd_directories.pop();
      std::filesystem::current_path().string() = cd_directories.top();
    }
  } else if (std::filesystem::exists(current_directory)) {
    if (cd_directories.empty) {
      cd_directories.push(original_path);
    }
    cd_directories.push(current_directory);
    std::filesystem::current_path.string();
  } else {
    cout << "cd: " << current_directory << "; No such file or directory";
  }
}
*/
std::string previous_directory;

void handle_cd(const std::string &input) {
  auto tokens = tokenize(input);
  std::string current = std::filesystem::current_path().string();

  if (tokens.size() == 1) {
    const char *home = std::getenv("HOME");
    if (home) {
      previous_directory = current;
      try {
        std::filesystem::current_path(home);
      } catch (const std::filesystem::filesystem_error &) {
        std::cerr << "cd: failed to change to HOME" << std::endl;
      }
    }
  } else if (tokens[1] == "-") {
    if (!previous_directory.empty()) {
      std::string temp = current;
      try {
        std::filesystem::current_path(previous_directory);
        std ::cout << previous_directory << std::endl;
        previous_directory = temp;
      } catch (const std::filesystem::filesystem_error &) {
        std::cerr << "cd: failed to change directory" << std::endl;
      }
    } else {
      std::cerr << "cd OLDPWD not set" << std::endl;
    }
  } else {
    std::string target = tokens[1];
    if (target == "~") {
      const char *home = std::getenv("HOME");
      if (home)
        target = home;
    } else if (target.rfind("~/", 0) == 0) {
      const char *home = std::getenv("HOME");
      if (home)
        target = std::string(home) + target.substr(1);
    }
    std::filesystem::path target_path;
    if (target[0] == '/') {
      target_path = target;
    } else {
      target_path = std::filesystem::current_path() / target;
      target_path = std::filesystem::absolute(target_path);
    }
    try {
      previous_directory = current;
      std::filesystem::current_path(target_path);
    } catch (const std::filesystem::filesystem_error &) {
      std::cout << "cd: " << target << ": No such file or directory"
                << std::endl;
    }
  }
}

int main() {
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  while (1) {
    std::cout << "$ ";
    std::string input;
    std::getline(std::cin, input);

    if (input.empty()) {
      continue;
    }
    auto tokens = tokenize(input);
    if (tokens.empty())
      continue;

    Command cmd = parseCommand(tokens);
    if (cmd.args.empty())
      continue;

    if (cmd.args[0] == "exit") {
      handle_exit(input);
    } else if (cmd.args[0] == "echo") {
      handle_echo(input);
    } else if (cmd.args[0] == "type") {
      handle_type(input);
    } else if (cmd.args[0] == "pwd") {
      std::cout << std::filesystem::current_path().string() << std::endl;
    } else if (cmd.args[0] == "cd") {
      handle_cd(input);
    } else {
      executeExternal(cmd);
    }
  }
}