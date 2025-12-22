#ifndef BUILTINS_H
#define BUILTINS_H

#include "parser.h"
#include <string>
#include <unordered_map>
#include <vector>

struct TrieNode {
  std::unordered_map<char, TrieNode *> children;
  bool isEndOfWord;

  // stores complete word at the end for easy retrieval
  std::string fullWord;

  TrieNode() : isEndOfWord(false), fullWord("") {};
};

class AutocompleteTrie {
private:
  TrieNode *root;

  // recursively collect all words from a node
  void collectWords(TrieNode *node, std::vector<std::string> &results);

  // clean up memory when destroying the trie
  void deleteNode(TrieNode *node);

public:
  AutocompleteTrie();
  ~AutocompleteTrie();

  void insert(const std::string word);
  std::vector<std::string> findMatches(const std::string &prefix);

  std::string autocomplete(const std::string &prefix);
};

void handle_echo(const Command &cmd);
void handle_type(const Command &cmd);
void handle_pwd(const Command &cmd);
void handle_cd(const Command &cmd);
void handle_exit(const Command &cmd);

#endif