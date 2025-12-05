# Repo for PPL Lexical and Syntax Analyzer

A simple lexer & parser for a story-telling focused programming language written in C. Implemented using principles of finite state machines.

## Makefile Usage

**Build and Run**

- **Lexer (GenerateTable):**
  ```sh
  make run-lexer
  ```
- **Parser (ParserG2PPL):**
  ```sh
  make run-parser
  ```
- **Clean build artifacts:**
  ```sh
  make clean
  ```
## Notes
- **Indentation must use spaces, not tabs (`\t`).**  
  The lexer only counts space characters for indentation. Tabs will cause incorrect INDENT/DEDENT tokens.
  
  To check for tabs in your source file:
  ```bash
  cat -A yourfile.st
  ```
  Tabs appear as `^I`. If found, replace them with spaces.

- All commands should be run from the project root directory.



