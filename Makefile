CC=gcc
CFLAGS=-Wall -Wextra

LEXICAL_SRC=Lexical-Analyzer/LexicalG2PPL.c
LEXICAL_HDR=Lexical-Analyzer/LexicalG2PPL.h

GENERATETABLE_SRC=Lexical-Analyzer/GenerateTable.c
PARSER_SRC=Syntax-Analyzer/ParserG2PPL.c

all: GenerateTable ParserG2PPL

GenerateTable: $(GENERATETABLE_SRC) $(LEXICAL_SRC) $(LEXICAL_HDR)
	$(CC) $(CFLAGS) $(GENERATETABLE_SRC) $(LEXICAL_SRC) -o GenerateTable

ParserG2PPL: $(PARSER_SRC) $(LEXICAL_SRC) $(LEXICAL_HDR)
	$(CC) $(CFLAGS) $(PARSER_SRC) $(LEXICAL_SRC) -o ParserG2PPL

run-lexer:
	$(MAKE) GenerateTable
	./GenerateTable

run-parser:
	$(MAKE) ParserG2PPL
	./ParserG2PPL

clean:
	rm -f GenerateTable ParserG2PPL SymbolTable.txt