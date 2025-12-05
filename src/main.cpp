#include <iostream>
#include <string>

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
    } else if (input.rfind("type",0) == 0) {
      std::string text = input.substr(4);
      if(!text.empty() && text[0] == ' '){
      text = text.substr(1);
      }
      if (text == "echo" || text == "exit"  || text == "type") {
        std::cout << text << " is a shell builtin" << std::endl;
      } else
        std::cout << text << ": not found" << std::endl;
    } else
      std::cout << input << ": command not found" << std::endl;
  }
}
