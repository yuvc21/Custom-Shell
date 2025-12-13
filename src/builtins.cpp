#include "builtins.h"
#include "utils.h"
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <unistd.h>

void handle_echo(const Command &cmd) {
  int saved_stdout = -1;
  int saved_stderr = -1;

  if (cmd.has_output_redirect) {
    int flags = O_WRONLY | O_CREAT;
    flags |= cmd.append_output ? O_APPEND : O_TRUNC;
    saved_stdout = dup(STDOUT_FILENO);
    int fd = open(cmd.output_file.c_str(), flags, 0644);
    if (fd != -1) {
      dup2(fd, STDOUT_FILENO);
      close(fd);
    }
  }

  if (cmd.has_error_redirect) {
    int flags = O_WRONLY | O_CREAT;
    flags != cmd.append_error ? O_APPEND : O_TRUNC;
    saved_stderr = dup(STDERR_FILENO);
    int fd = open(cmd.error_file.c_str(), flags, 0644);
    if (fd != -1) {
      dup2(fd, STDERR_FILENO);
      close(fd);
    }
  }

  for (size_t i = 1; i < cmd.args.size(); ++i) {
    if (i > 1)
      std::cout << ' ';
    std::cout << cmd.args[i];
  }
  std::cout << std::endl;

  if (saved_stdout != -1) {
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);
  }
  if (saved_stderr != -1) {
    dup2(saved_stderr, STDERR_FILENO);
    close(saved_stderr);
  }
}

void handle_type(const Command &cmd) {
  if (cmd.args.size() < 2)
    return;
  std::string text = cmd.args[1];

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

void handle_pwd(const Command &cmd) {
  // Add redirection support if needed
  std::cout << std::filesystem::current_path().string() << std::endl;
}

void handle_cd(const Command &cmd) {
  std::string current = std::filesystem::current_path().string();

  if (cmd.args.size() == 1) {
    const char *home = std::getenv("HOME");
    if (home) {
      previous_directory = current;
      try {
        std::filesystem::current_path(home);
      } catch (const std::filesystem::filesystem_error &) {
        std::cerr << "cd: failed to change to HOME" << std::endl;
      }
    }
  } else if (cmd.args[1] == "-") {
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
    std::string target = cmd.args[1];
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

void handle_exit(const Command &cmd) {
  int code = 0;
  if (cmd.args.size() > 1) {
    try {
      code = std::stoi(cmd.args[1]);
    } catch (const std::invalid_argument &) {
      std::cerr << "exit: " << cmd.args[1] << ": numeric argument required"
                << std::endl;
      code = 2;
    } catch (const std::out_of_range &) {
      std::cerr << "exit: " << cmd.args[1] << ": numeric argument required"
                << std::endl;
      code = 2;
    }
  }
  exit(code);
}