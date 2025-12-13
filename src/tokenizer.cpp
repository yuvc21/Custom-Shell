#include "tokenizer.h"

std::vector<std::string> tokenize(const std::string &input) {
  std::vector<std::string> tokens;
  std::string current_token;
  bool in_single_quotes = false;
  bool in_double_quotes = false;
  for (size_t i = 0; i < input.size(); ++i) {
    char c = input[i];
    if (c == '\'' && !in_double_quotes) {
      in_single_quotes = !in_single_quotes;
      continue;
    } else if (c == '"' && !in_single_quotes) {
      in_double_quotes = !in_double_quotes;
      continue;
    } else if (c == ' ' && !in_single_quotes && !in_double_quotes) {
      if (!current_token.empty()) {
        tokens.push_back(current_token);
        current_token.clear();
      }
      // handling the backslash with ans without quotes
    } else if (c == '\\' && i + 1 < input.length()) {
      char next = input[i + 1];
      if (in_single_quotes) {
        current_token += c;
      } else if (in_double_quotes) {
        if (next == '"' || next == '\\' || next == '$' || next == '`') {
          current_token += next;
          ++i;
        } else {
          current_token += c;
        }
      } else {
        current_token += next;
        ++i;
      }
    } else if (c == '>' && i + 1 < input.size() && input[i + 1] == '>') {
      if (!current_token.empty()) {
        char last = current_token.back();
        if (last == '1' || last == '2') {
          current_token.pop_back();
          if (!current_token.empty()) {
            tokens.push_back(current_token);
          }
          current_token.clear();
          tokens.push_back(std::string(1, last) + ">>");
          ++i; // skipping the second '>'
        } else {
          tokens.push_back(current_token);
          current_token.clear();
          tokens.push_back(">>");
          ++i; // skipping the second '>'
        }
      } else {
        if (!tokens.empty() && (tokens.back() == "1" || tokens.back() == "2")) {
          std::string digit = tokens.back();
          tokens.pop_back();
          tokens.push_back(digit + ">>");
        } else {
          tokens.push_back(">>");
        }
        ++i; // skipping the second '>'
      }
    } else if (c == '>') {
      if (!current_token.empty()) {
        char last = current_token.back();
        if (last == '1' || last == '2') {
          current_token.pop_back();

          if (!current_token.empty()) {
            tokens.push_back(current_token);
          }
          current_token.clear();
          tokens.push_back(std::string(1, last) + ">");
        } else {
          tokens.push_back(current_token);
          current_token.clear();
          tokens.push_back(">");
        }
      }
      // current_token is empty that means case might be 1 > or 2 > like this
      else {
        if (!tokens.empty() && (tokens.back() == "1" || tokens.back() == "2")) {
          std::string digit = tokens.back();
          tokens.pop_back();
          tokens.push_back(digit + ">");
        } else {
          tokens.push_back(">");
        }
      }
    } else {
      current_token += c;
    }
  }
  if (!current_token.empty()) {
    tokens.push_back(current_token);
  }
  return tokens;
}