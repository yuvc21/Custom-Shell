#include "builtins.h"
#include "input.h"
#include <iostream>
#include <string>

static AutocompleteTrie trie;

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
  
  std::string input;
  if (!std::getline(std::cin, input)) {
    return "";
  }

  if (!input.empty() && input.back() == '\t') {
    input.pop_back();

    std::string first_word = input;
    auto space_pos = input.find(' ');
    if (space_pos != std::string::npos) {
      first_word = input.substr(0, space_pos);
    }

    std::string completed = trie.autocomplete(first_word);
    if (!completed.empty()) {
      if (space_pos == std::string::npos) {
        input = completed;
      } else {
        input = completed + input.substr(space_pos);
      }
    }
  }

  return input;
}
