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
static int token_consumed = 1; // 1 artinya current_token sudah "dimakan", perlu baca baru

void init_lexer(const char *source) {
    src = source;
    pos = 0;
    line = 1;
    token_consumed = 1;
}

static char peek_char() {
    return src[pos];
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

// Internal function to actually read source
static Token scan_token() {
    skip_whitespace();

    if (peek_char() == '\0') {
        return (Token){TOKEN_EOF, NULL, line};
    }

    if (peek_char() == '=') {
        advance_char();
        return make_token(TOKEN_EQUALS, "=", 1);
    }

    if (peek_char() == '"') {
        advance_char();
        const char *start = &src[pos];
        int length = 0;
        while (peek_char() != '"' && peek_char() != '\0') {
            advance_char();
            length++;
        }
        if (peek_char() == '"') advance_char();
        return make_token(TOKEN_STRING, start, length);
    }

    if (isdigit(peek_char())) {
        const char *start = &src[pos];
        int length = 0;
        while (isdigit(peek_char())) {
            advance_char();
            length++;
        }
        return make_token(TOKEN_NUMBER, start, length);
    }

    if (isalpha(peek_char()) || peek_char() == '_') {
        const char *start = &src[pos];
        int length = 0;
        while (isalnum(peek_char()) || peek_char() == '_') {
            advance_char();
            length++;
        }

        if (length == 5 && strncmp(start, "tulis", 5) == 0) return make_token(TOKEN_TULIS, start, length);
        if (length == 4 && strncmp(start, "biar", 4) == 0) return make_token(TOKEN_BIAR, start, length);
        if (length == 4 && strncmp(start, "jika", 4) == 0) return make_token(TOKEN_JIKA, start, length);
        if (length == 4 && strncmp(start, "maka", 4) == 0) return make_token(TOKEN_MAKA, start, length);
        if (length == 5 && strncmp(start, "akhir", 5) == 0) return make_token(TOKEN_AKHIR, start, length);

        return make_token(TOKEN_IDENTIFIER, start, length);
    }

    const char *start = &src[pos];
    advance_char();
    return make_token(TOKEN_UNKNOWN, start, 1);
}

// Public API
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
    // Return copy (shallow copy of struct, pointers still valid)
    // Warning: Don't free value from peek unless you know what you are doing,
    // usually value is freed after next_token() usage.
    // However, our make_token mallocs value.
    // Ideally we need better ownership.
    // For this simple parser: peek returns a "view".
    // But scan_token creates a NEW string every time.
    // If we call peek 10 times, we don't want 10 mallocs if it's the same token.
    // Implementation above: `token_consumed` flag handles this buffering.
    return current_token;
}
