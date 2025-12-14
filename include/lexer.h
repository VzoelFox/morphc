#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOKEN_EOF,
    TOKEN_TULIS,
    TOKEN_BIAR,
    TOKEN_JIKA,
    TOKEN_MAKA,
    TOKEN_AKHIR,
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_IDENTIFIER,
    TOKEN_EQUALS,
    TOKEN_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char *value;
    int line;
} Token;

void init_lexer(const char *source);
Token next_token();
Token peek_token(); // Helper baru untuk parser

#endif
