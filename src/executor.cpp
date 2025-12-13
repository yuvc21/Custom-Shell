#include "executor.h"
#include "utils.h"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

extern char** environ;

void executeExternal(const Command& cmd) {
  if (cmd.args.empty()) return;
  
  std::string command_path = find_executables_in_path(cmd.args[0]);
  if (command_path.empty()) {
    std::cout << cmd.args[0] << ": command not found" << std::endl;
    return;
  }
  
  pid_t pid = fork();
  if (pid == -1) {
    std::cerr << "Failed to fork\n";
    return;
  }
  
  if (pid == 0) {
    // handle stdout redirection
    if (cmd.has_output_redirect) {
        int flags = O_WRONLY | O_CREAT;
        if(cmd.append_output){
            flags |= O_APPEND;
        }else{
            flags |= O_TRUNC;
        }
      int fd = open(cmd.output_file.c_str(), flags, 0644);
      if (fd != -1) {
        dup2(fd, STDOUT_FILENO);
        close(fd);
      }
    }
    
    // handle stderr redirection
    if (cmd.has_error_redirect) {
        int flags = O_WRONLY | O_CREAT;
        if(cmd.append_error){
            flags |= O_APPEND;
        }else{
            flags |= O_TRUNC;
        }
      int fd = open(cmd.error_file.c_str(), flags, 0644);
      if (fd != -1) {
        dup2(fd, STDERR_FILENO);
        close(fd);
      }
    }
    
    std::vector<char*> argv;
    for (const auto& arg : cmd.args) {
      argv.push_back(const_cast<char*>(arg.c_str()));
    }
    argv.push_back(nullptr);
    
    execve(command_path.c_str(), argv.data(), environ);
    std::cerr << "Failed to execute " << cmd.args[0] << std::endl;
    exit(1);
  } else {
    int status;
    waitpid(pid, &status, 0);
  }
}
