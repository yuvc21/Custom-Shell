#include "parser.h"

Command parseCommand(const std::vector<std::string> &tokens) {
  Command cmd;
  for (size_t i = 0; i < tokens.size(); ++i) {
    if (tokens[i] == ">>" || tokens[i] == "1>>") {
      if (i + 1 < tokens.size()) {
        cmd.output_file = tokens[i + 1];
        cmd.has_output_redirect = true;
        cmd.append_output = true;
        ++i;
      }
    } else if (tokens[i] == ">" || tokens[i] == "1>") {
      if (i + 1 < tokens.size()) {
        cmd.output_file = tokens[i + 1];
        cmd.has_output_redirect = true;
        cmd.append_output = false;
        ++i;
      }
    } else if (tokens[i] == "2>>") {
      if (i + 1 < tokens.size()) {
        cmd.error_file = tokens[i + 1];
        cmd.has_error_redirect = true;
        cmd.append_error = true;
        ++i;
      }
    } else if (tokens[i] == "2>") {
      if (i + 1 < tokens.size()) {
        cmd.error_file = tokens[i + 1];
        cmd.has_error_redirect = true;
        cmd.append_error = false;
        ++i;
      }
    } else {
      cmd.args.push_back(tokens[i]);
    }
  }
  return cmd;
}
