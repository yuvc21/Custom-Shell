#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>

struct Command {
  std::vector<std::string> args;
  std::string output_file;
  std::string error_file;
  bool has_output_redirect = false;
  bool has_error_redirect = false;
  bool append_output = false;
  bool append_error = false;
};

Command parseCommand(const std::vector<std::string>& tokens);

#endif