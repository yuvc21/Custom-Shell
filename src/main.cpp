#include <iostream>
#include <string>
#include "tokenizer.h"
#include "parser.h"
#include "builtins.h"
#include "executor.h"

int main() {
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  while (true) {
    std::cout << "$ ";
    std::string input;
    std::getline(std::cin, input);

    if (input.empty()) continue;

    auto tokens = tokenize(input);
    if (tokens.empty()) continue;

    Command cmd = parseCommand(tokens);
    if (cmd.args.empty()) continue;

    if (cmd.args[0] == "exit") {
      handle_exit(cmd);
    } else if (cmd.args[0] == "echo") {
      handle_echo(cmd);
    } else if (cmd.args[0] == "type") {
      handle_type(cmd);
    } else if (cmd.args[0] == "pwd") {
      handle_pwd(cmd);
    } else if (cmd.args[0] == "cd") {
      handle_cd(cmd);
    } else {
      executeExternal(cmd);
    }
  }

  return 0;
}