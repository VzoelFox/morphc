#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "lexer.h"

static const char *src;
static int pos = 0;
static int line = 1;

// Buffer untuk peek_token
static Token current_token;
static int token_consumed = 1;

void init_lexer(const char *source) {
    src = source;
    pos = 0;
    line = 1;
    token_consumed = 1;
}

static char peek_char() {
    return src[pos];
}

static char peek_next_char() {
    if (src[pos] == '\0') return '\0';
    return src[pos + 1];
}

static char advance_char() {
    char c = src[pos];
    if (c != '\0') pos++;
    if (c == '\n') line++;
    return c;
}

static void skip_whitespace() {
    while (isspace(peek_char())) {
        advance_char();
    }
}

static Token make_token(TokenType type, const char *start, int length) {
    Token token;
    token.type = type;
    token.line = line;
    token.value = malloc(length + 1);
    strncpy(token.value, start, length);
    token.value[length] = '\0';
    return token;
}

static Token scan_token() {
    skip_whitespace();

    char c = peek_char();

    if (c == '\0') return (Token){TOKEN_EOF, NULL, line};

    // Single-char & Double-char tokens
    const char *start = &src[pos];

    if (c == '(') { advance_char(); return make_token(TOKEN_LPAREN, "(", 1); }
    if (c == ')') { advance_char(); return make_token(TOKEN_RPAREN, ")", 1); }
    if (c == ',') { advance_char(); return make_token(TOKEN_COMMA, ",", 1); }
    if (c == '+') { advance_char(); return make_token(TOKEN_PLUS, "+", 1); }
    if (c == '-') { advance_char(); return make_token(TOKEN_MINUS, "-", 1); }
    if (c == '*') { advance_char(); return make_token(TOKEN_STAR, "*", 1); }
    if (c == '/') { advance_char(); return make_token(TOKEN_SLASH, "/", 1); }

    if (c == '=') {
        advance_char();
        if (peek_char() == '=') {
            advance_char();
            return make_token(TOKEN_EQ_EQ, "==", 2);
        }
        return make_token(TOKEN_EQUALS, "=", 1);
    }

    if (c == '!') {
        advance_char();
        if (peek_char() == '=') {
            advance_char();
            return make_token(TOKEN_BANG_EQ, "!=", 2);
        }
        return make_token(TOKEN_UNKNOWN, "!", 1);
    }

    if (c == '<') {
        advance_char();
        if (peek_char() == '=') {
            advance_char();
            return make_token(TOKEN_LT_EQ, "<=", 2);
        }
        return make_token(TOKEN_LT, "<", 1);
    }

    if (c == '>') {
        advance_char();
        if (peek_char() == '=') {
            advance_char();
            return make_token(TOKEN_GT_EQ, ">=", 2);
        }
        return make_token(TOKEN_GT, ">", 1);
    }

    // String literals
    if (c == '"') {
        advance_char();
        const char *s_start = &src[pos];
        int length = 0;
        while (peek_char() != '"' && peek_char() != '\0') {
            advance_char();
            length++;
        }
        if (peek_char() == '"') advance_char();
        return make_token(TOKEN_STRING, s_start, length);
    }

    // Numbers
    if (isdigit(c)) {
        const char *n_start = &src[pos];
        int length = 0;
        while (isdigit(peek_char())) {
            advance_char();
            length++;
        }
        return make_token(TOKEN_NUMBER, n_start, length);
    }

    // Identifiers & Keywords
    if (isalpha(c) || c == '_') {
        const char *i_start = &src[pos];
        int length = 0;
        while (isalnum(peek_char()) || peek_char() == '_') {
            advance_char();
            length++;
        }

        if (length == 5 && strncmp(i_start, "tulis", 5) == 0) return make_token(TOKEN_TULIS, i_start, length);
        if (length == 4 && strncmp(i_start, "biar", 4) == 0) return make_token(TOKEN_BIAR, i_start, length);
        if (length == 4 && strncmp(i_start, "jika", 4) == 0) return make_token(TOKEN_JIKA, i_start, length);
        if (length == 4 && strncmp(i_start, "maka", 4) == 0) return make_token(TOKEN_MAKA, i_start, length);
        if (length == 5 && strncmp(i_start, "akhir", 5) == 0) return make_token(TOKEN_AKHIR, i_start, length);

        // New Keywords
        if (length == 6 && strncmp(i_start, "fungsi", 6) == 0) return make_token(TOKEN_FUNGSI, i_start, length);
        if (length == 7 && strncmp(i_start, "kembali", 7) == 0) return make_token(TOKEN_KEMBALI, i_start, length);
        if (length == 3 && strncmp(i_start, "dan", 3) == 0) return make_token(TOKEN_DAN, i_start, length);
        if (length == 4 && strncmp(i_start, "atau", 4) == 0) return make_token(TOKEN_ATAU, i_start, length);

        return make_token(TOKEN_IDENTIFIER, i_start, length);
    }

    advance_char();
    return make_token(TOKEN_UNKNOWN, start, 1);
}

Token next_token() {
    if (!token_consumed) {
        token_consumed = 1;
        return current_token;
    }
    return scan_token();
}

Token peek_token() {
    if (token_consumed) {
        current_token = scan_token();
        token_consumed = 0;
    }
    return current_token;
}
