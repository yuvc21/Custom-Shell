# Custom Shell

<div align="center">

![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-064F8C?style=for-the-badge&logo=cmake&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)
![License](https://img.shields.io/badge/license-MIT-green?style=for-the-badge)

A UNIX shell built from scratch in C++ as part of the [CodeCrafters](https://codecrafters.io) challenge.

[Features](#features) • [Installation](#installation) • [Usage](#usage) • [Architecture](#architecture) • [Roadmap](#roadmap)

</div>

---

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Tech Stack](#tech-stack)
- [Project Structure](#project-structure)
- [Installation](#installation)
- [Usage](#usage)
- [Architecture](#architecture)
- [Roadmap](#roadmap)
- [Contributing](#contributing)
- [License](#license)

---

## Overview

A lightweight UNIX shell implementation written in modern C++. This project demonstrates low-level terminal programming, command parsing, process management, and system design principles.

Built as part of the CodeCrafters "Build Your Own Shell" challenge, this shell handles user input, parses commands, executes builtins and external programs, and supports I/O redirection.

### Key Highlights

- **Pure C++ Implementation**: No external shell libraries used
- **POSIX Compliant**: Direct system call usage for process and terminal operations
- **Custom Tokenizer & Parser**: Handles quotes, escaping, and redirection
- **Trie-based Autocomplete**: Fast tab completion using a Trie data structure
- **Modular Architecture**: Clean separation of concerns for maintainability

---

## Features

### Currently Implemented ✅

- ✅ **REPL Loop** - Read, evaluate, print loop with raw terminal input
- ✅ **Command Parsing** - Tokenizer and parser with quote and escape handling
- ✅ **Builtin Commands** - `echo`, `exit`, `type`, `pwd`, `cd`
- ✅ **External Command Execution** - Fork/exec with PATH resolution
- ✅ **I/O Redirection** - `>`, `>>`, `2>`, `2>>` support
- ✅ **Tab Completion** - Trie-based autocomplete for builtins and PATH executables
- ✅ **Directory Navigation** - `cd` with `~`, `-`, and relative/absolute paths
- ✅ **Previous Directory** - `cd -` to switch back to last directory

---

## Tech Stack

| Component | Technology |
|-----------|-----------|
| **Language** | C++23 |
| **Build System** | CMake 3.13+ |
| **Package Manager** | vcpkg |
| **Terminal API** | POSIX termios |
| **Process API** | fork/exec/waitpid |
| **Platform** | Linux (Ubuntu/Debian recommended) |

---

## Project Structure

```
src/
├── main.cpp        - Entry point and REPL loop
├── input.cpp       - Raw terminal input and tab completion
├── tokenizer.cpp   - Lexer for shell input
├── parser.cpp      - Command parser and AST
├── builtins.cpp    - Builtin command implementations + Trie
├── executor.cpp    - External command execution
└── utils.cpp       - PATH resolution and helpers
```

### Core Components

**`input.cpp`**
- Raw terminal mode via termios
- Character-by-character input handling
- Trie-based tab completion for builtins and PATH executables

**`tokenizer.cpp / parser.cpp`**
- Handles single/double quotes and backslash escaping
- Parses I/O redirection operators
- Builds structured Command objects

**`builtins.cpp`**
- Implements echo, exit, type, pwd, cd
- AutocompleteTrie class with insert and prefix search

**`executor.cpp`**
- Resolves commands via PATH
- Fork/exec for external processes

---

## Installation

### Prerequisites

```bash
sudo apt update
sudo apt install -y build-essential cmake g++ git
```

### Build from Source

```bash
# Clone the repository
git clone https://github.com/yuvc21/Custom-Shell.git
cd Custom-Shell

# Build
cmake -B build -S .
cmake --build build

# Run
./build/shell
```

---

## Usage

### Starting the Shell

```bash
./build/shell
```

### Example Commands

```bash
# Builtin commands
$ echo hello world
hello world

$ pwd
/home/yuvaraj

$ cd ~/projects
$ cd -        # go back to previous directory

$ type echo
echo is a shell builtin

$ type git
git is /usr/bin/git

# I/O Redirection
$ echo hello > out.txt
$ echo world >> out.txt
$ cat out.txt
hello
world

# Tab Completion
$ ech[TAB] → echo
$ gi[TAB]  → git
```

---

## Architecture

### REPL Flow

```
User Input (raw terminal)
        ↓
  read_command_line()
  (tab completion here)
        ↓
    Tokenizer
        ↓
      Parser
        ↓
  Builtin? → handle_builtin()
      ↓ No
  executor → fork() → exec()
        ↓
   Output to terminal
```

### Trie Autocomplete Design

```cpp
class AutocompleteTrie {
  void insert(const std::string& word);
  std::vector<std::string> findMatches(const std::string& prefix);
  std::string autocomplete(const std::string& prefix); // returns single match
};
```

---

## Roadmap

### Phase 1: Core Shell ✅
- [x] REPL loop
- [x] Builtin commands
- [x] External command execution
- [x] PATH resolution

### Phase 2: Advanced Features ✅
- [x] Quote and escape handling
- [x] I/O redirection (stdout, stderr, append)
- [x] Tab completion with Trie
- [x] cd with ~, -, relative/absolute paths

---

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

---

## License

This project is licensed under the MIT License.

---

## Acknowledgments

- **[CodeCrafters](https://codecrafters.io)** - For the excellent learning platform
- **POSIX Specification** - Terminal and process APIs
- **Advanced Programming in the UNIX Environment** - Essential systems reference

---

<div align="center">

Made with ❤️ and C++

</div>