#include <iostream>
#include <string>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  std::cout << "$ ";

  std::string command;

  std::getline(std::cin, command);
  if (command == "exit") {
    return 0;
  } else {
    std::cout << command << ": command not found" << std::endl;
  }
  main();
}
