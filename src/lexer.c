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

    if (isalpha(peek())) {
        const char *start = &src[pos];
        int length = 0;
        while (isalnum(peek()) || peek() == '_') {
            advance();
            length++;
        }

        // Cek keywords
        if (length == 5 && strncmp(start, "tulis", 5) == 0) {
            return make_token(TOKEN_TULIS, start, length);
        }

        return make_token(TOKEN_UNKNOWN, start, length); // Identifier sementara dianggap unknown
    }

    // Skip unknown char
    advance();
    return next_token();
}
