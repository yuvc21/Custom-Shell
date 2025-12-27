# Custom-Shell

A feature-rich Unix shell built from scratch in C++, implementing core shell functionalities including command execution, I/O redirection, pipelines, and intelligent tab completion.

## ✨ Features

### Core Functionality
- **Builtin Commands**: `echo`, `exit`, `cd`, `pwd`, `type`
- **External Command Execution**: Execute any program in PATH
- **Environment Variable Support**: Access and use environment variables
- **Command History**: Navigate through previous commands

### Advanced Features
- **I/O Redirection**: Support for `>`, `>>`, `<`, `2>` operators
- **Pipelines**: Chain multiple commands with `|`
- **Intelligent Tab Completion**: 
  - Autocomplete builtin commands
  - Autocomplete external executables from PATH
  - Bell notification for ambiguous/invalid completions
- **Background Processes**: Run commands with `&`
- **Signal Handling**: Proper handling of Ctrl+C and other signals

## 📁 Project Structure

## 📁 Project Structure

- `src/` - Source code directory
  - `main.cpp` - Entry point
  - `input.cpp/h` - Raw mode input handling & tab completion
  - `parser.cpp/h` - Command parsing logic
  - `tokenizer.cpp/h` - Lexical analysis and tokenization
  - `executor.cpp/h` - Command execution engine
  - `builtins.cpp/h` - Builtin command implementations
  - `utils.cpp/h` - Utility functions (PATH search, etc.)
- `build/` - Compiled binaries
- `.vscode/` - VSCode configuration

text

## 🔨 Building

### Prerequisites
- C++17 or later
- GCC/Clang compiler
- POSIX-compliant system (Linux/macOS)

### Compilation

Using g++

g++ -std=c++17 -o custom-shell src/*.cpp
Or use your build system

cd build && make

text

## 🚀 Usage

Run the shell

./custom-shell
Execute commands

$ echo Hello World
Hello World

$ pwd
/home/user/custom-shell

$ ls -la | grep cpp
-rw-r--r-- 1 user user 1234 Dec 27 main.cpp
-rw-r--r-- 1 user user 2345 Dec 27 executor.cpp

text

### Tab Completion Examples

$ ech<TAB> # Completes to: echo
$ custom<TAB> # Completes to: custom_executable

text

### I/O Redirection Examples

$ echo "Hello" > output.txt
$ cat < input.txt > output.txt 2> errors.txt

text

## 🔧 Implementation Details

### Tokenizer
Breaks input into tokens while respecting:
- Quoted strings (single and double quotes)
- Special operators (`|`, `>`, `>>`, `<`, `2>`, `&`)
- Whitespace handling

### Parser
Constructs command structures with:
- Command arguments
- Redirection specifications
- Pipeline chains

### Executor
Handles:
- Process creation with `fork()` and `execvp()`
- File descriptor management for redirections
- Pipe setup for command chaining
- Background process handling

### Tab Completion
Uses a **Trie data structure** for builtins and **dynamic PATH scanning** for executables:
- O(L) lookup for builtin commands
- O(D × F × L) search for PATH executables (on-demand)
- Bell character (ASCII 0x07) for invalid/ambiguous completions

## 💡 Technical Highlights

- **Raw Mode Terminal**: Custom input handling without canonical mode
- **Trie-based Autocomplete**: Efficient prefix matching for builtin commands
- **Lazy PATH Scanning**: Searches executables only on Tab press
- **Robust Error Handling**: Graceful handling of missing files, permission errors
- **Memory Safe**: Proper resource cleanup and signal handling

## 🗺️ Roadmap

- [ ] Command history with arrow keys
- [ ] Job control (`fg`, `bg`, `jobs`)
- [ ] Shell scripting support
- [ ] Aliases and functions
- [ ] Configuration file (`.custom-shellrc`)

## 👨‍💻 Development

### Adding a New Builtin

**Step 1**: Add implementation in `builtins.cpp`

void builtin_newcmd(const std::vectorstd::string &args) {
// Implementation
}

text

**Step 2**: Register in Trie (`input.cpp`)

trie.insert("newcmd");

text

**Step 3**: Add case in executor (`executor.cpp`)

if (cmd == "newcmd") builtin_newcmd(args);

text

## ✅ Testing

The shell has been tested against automated test suites including:
- Basic command execution
- Complex pipelines and redirections
- Tab completion edge cases
- Error handling scenarios

## 📝 License

MIT License - feel free to use and modify!

## Acknowledgments

Built as part of the [CodeCrafters Shell Challenge](https://codecrafters.io/challenges/shell)

---

**Note**: This is an educational project demonstrating Unix system programming concepts including process management, file descriptors, and terminal I/O.
