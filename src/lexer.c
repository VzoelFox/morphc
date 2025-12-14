#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "lexer.h"

static const char *src;
static int pos = 0;
static int line = 1;

void init_lexer(const char *source) {
    src = source;
    pos = 0;
    line = 1;
}

static char peek() {
    return src[pos];
}

static char advance() {
    char c = src[pos];
    if (c != '\0') pos++;
    if (c == '\n') line++;
    return c;
}

static void skip_whitespace() {
    while (isspace(peek())) {
        advance();
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

Token next_token() {
    skip_whitespace();

    if (peek() == '\0') {
        return (Token){TOKEN_EOF, NULL, line};
    }

    // Single character tokens
    if (peek() == '=') {
        advance();
        return make_token(TOKEN_EQUALS, "=", 1);
    }

    // String literals
    if (peek() == '"') {
        advance(); // Skip opening quote
        const char *start = &src[pos];
        int length = 0;
        while (peek() != '"' && peek() != '\0') {
            advance();
            length++;
        }
        if (peek() == '"') advance(); // Skip closing quote
        return make_token(TOKEN_STRING, start, length);
    }

    // Numbers
    if (isdigit(peek())) {
        const char *start = &src[pos];
        int length = 0;
        while (isdigit(peek())) {
            advance();
            length++;
        }
        return make_token(TOKEN_NUMBER, start, length);
    }

    // Identifiers and Keywords
    if (isalpha(peek()) || peek() == '_') {
        const char *start = &src[pos];
        int length = 0;
        while (isalnum(peek()) || peek() == '_') {
            advance();
            length++;
        }

        // Keywords Check
        if (length == 5 && strncmp(start, "tulis", 5) == 0) {
            return make_token(TOKEN_TULIS, start, length);
        }
        if (length == 4 && strncmp(start, "biar", 4) == 0) {
            return make_token(TOKEN_BIAR, start, length);
        }

        return make_token(TOKEN_IDENTIFIER, start, length);
    }

    // Unknown
    const char *start = &src[pos];
    advance();
    return make_token(TOKEN_UNKNOWN, start, 1);
}
