#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOKEN_EOF,

    // Keywords
    TOKEN_TULIS,
    TOKEN_BIAR,
    TOKEN_JIKA,
    TOKEN_MAKA,
    TOKEN_AKHIR,
    TOKEN_FUNGSI,
    TOKEN_KEMBALI,
    TOKEN_DAN,
    TOKEN_ATAU,

    // Literals
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_IDENTIFIER,

    // Operators & Punctuation
    TOKEN_EQUALS,    // =
    TOKEN_PLUS,      // +
    TOKEN_MINUS,     // -
    TOKEN_STAR,      // *
    TOKEN_SLASH,     // /
    TOKEN_LPAREN,    // (
    TOKEN_RPAREN,    // )
    TOKEN_COMMA,     // ,

    // Comparison
    TOKEN_EQ_EQ,     // ==
    TOKEN_BANG_EQ,   // !=
    TOKEN_LT,        // <
    TOKEN_GT,        // >
    TOKEN_LT_EQ,     // <=
    TOKEN_GT_EQ,     // >=

    TOKEN_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char *value;
    int line;
} Token;

void init_lexer(const char *source);
Token next_token();
Token peek_token();

#endif
