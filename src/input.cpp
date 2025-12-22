#include "builtins.h"
#include <iostream>
#include <termios.h>
#include <unistd.h>

// global variable to store the original terminal settings so
// they can be restored later
struct termios orig_termios;

static AutocompleteTrie trie;

static void disableRawMode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

static void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_termios);
  struct termios raw = orig_termios;
  raw.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

// initializing the trie once
static void init_trie_once() {
  static bool initialized = false;
  if (initialized) {
    return;
  }
  initialized = true;
  trie.insert("echo");
  trie.insert("exit");
  trie.insert("type");
  trie.insert("pwd");
  trie.insert("cd");
}

std::string read_command_line() {
  init_trie_once();
  enableRawMode();

  std::string input;
  char c;

  while (read(STDIN_FILENO, &c, 1) == 1) {
    // check if the character is TAB key (ASCII 9);
    if (c == '\t') {
      std::string completed = trie.autocomplete(input);
      if (!completed.empty()) {
        for (size_t i = 0; i < input.length(); ++i) {
          std::cout << "\b \b" << std::flush;
        }
        input = completed;
        std::cout << input << std::flush;
      }
    } else if (c == '\n' || c == '\r') {
      std::cout << '\n' << std::flush;
      break;
    } else if (c == 127 || c == 8) {
      if (!input.empty()) {
        input.pop_back();
        std::cout << "\b \b" << std::flush;
      }
    } else if (c >= 32 && c < 127) {
      input += c;
      std::cout << c << std::flush;
    }
  }
  disableRawMode();
  return input;
}
