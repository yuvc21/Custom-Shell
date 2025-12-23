#include "builtins.h"
#include "input.h"
#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>

static AutocompleteTrie trie;
static struct termios orig_termios;

static void init_trie_once() {
  static bool initialized = false;
  if (initialized) return;
  initialized = true;
  
  trie.insert("echo");
  trie.insert("exit");
  trie.insert("type");
  trie.insert("pwd");
  trie.insert("cd");
}

static void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_termios);
  struct termios raw = orig_termios;
  raw.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

static void disableRawMode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

std::string read_command_line() {
  init_trie_once();
  enableRawMode();
  
  std::string input;
  char c;
  
  while (read(STDIN_FILENO, &c, 1) == 1) {
    if (c == '\t') {  // Tab pressed
      std::string first_word = input;
      auto space_pos = input.find(' ');
      if (space_pos != std::string::npos) {
        first_word = input.substr(0, space_pos);
      }
      
      std::string completed = trie.autocomplete(first_word);
      if (!completed.empty()) {
        // Erase old first word
        for (size_t i = 0; i < first_word.length(); i++) {
          std::cout << "\b \b" << std::flush;
        }
        
        // Update input
        if (space_pos == std::string::npos) {
          input = completed + " ";
        } else {
          input = completed + input.substr(space_pos);
        }
        
        // Print completed word
        std::cout << completed << " " << std::flush;
      }
    } else if (c == '\n' || c == '\r') {  // Enter
      std::cout << '\n' << std::flush;
      break;
    } else if (c == 127 || c == 8) {  // Backspace
      if (!input.empty()) {
        input.pop_back();
        std::cout << "\b \b" << std::flush;
      }
    } else if (c >= 32 && c < 127) {  // Printable characters
      input += c;
      std::cout << c << std::flush;
    }
  }
  
  disableRawMode();
  return input;
}