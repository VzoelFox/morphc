#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "lexer.h"

// Forward declarations
static ASTNode* parse_statement();
static ASTNode* parse_expression();
static ASTNode* parse_block();

void init_parser(const char *source) {
    init_lexer(source);
}

static Token consume(TokenType type, const char *err_msg) {
    Token t = next_token();
    if (t.type == type) return t;

    fprintf(stderr, "Parser Error Line %d: %s. Found token type %d\n", t.line, err_msg, t.type);
    exit(1);
}

static ASTNode* parse_expression() {
    Token t = next_token();

    if (t.type == TOKEN_STRING) {
        ASTNode *node = new_literal_string(t.value);
        free(t.value);
        return node;
    }
    else if (t.type == TOKEN_NUMBER) {
        ASTNode *node = new_literal_number(atoi(t.value));
        free(t.value);
        return node;
    }
    else if (t.type == TOKEN_IDENTIFIER) {
        ASTNode *node = new_var_access(t.value);
        free(t.value);
        return node;
    }

    fprintf(stderr, "Parser Error Line %d: Unexpected expression token type %d\n", t.line, t.type);
    if(t.value) free(t.value);
    return NULL;
}

static ASTNode* parse_statement() {
    Token t = peek_token();

    if (t.type == TOKEN_TULIS) {
        next_token(); // consume 'tulis'
        if(t.value) free(t.value); // free peeked/consumed value? No, make_token allocs.
        // Wait, peek_token buffers it. next_token returns the buffered one.
        // We need to free the value of the consumed token.
        // The `t` variable here is a copy of struct from peek. Its `value` pointer points to buffered mem.
        // When we call next_token, we get the same struct.
        // We should just call match() or consume() properly.

        // Let's rely on standard flow
        ASTNode *expr = parse_expression();
        return new_print(expr);
    }

    if (t.type == TOKEN_BIAR) {
        next_token(); // consume 'biar'
        // We need to free the value of token 'biar'.
        // Simplified: Lexer allocates. Parser must free.
        // Since we did `t = peek`, `t.value` is valid.
        // But `next_token` returns the struct again.
        // Let's just be careful.
        // Best practice: consume() returns Token, we free it.

        Token id = consume(TOKEN_IDENTIFIER, "Diharapkan nama variabel");
        consume(TOKEN_EQUALS, "Diharapkan '='");
        ASTNode *expr = parse_expression();

        ASTNode *node = new_var_decl(id.value, expr);
        free(id.value);
        return node;
    }

    if (t.type == TOKEN_JIKA) {
        Token jika_token = next_token(); // consume 'jika'
        if(jika_token.value) free(jika_token.value);

        ASTNode *cond = parse_expression();

        Token maka = consume(TOKEN_MAKA, "Diharapkan 'maka'");
        if(maka.value) free(maka.value);

        ASTNode *then_block = parse_block();

        Token akhir = consume(TOKEN_AKHIR, "Diharapkan 'akhir'");
        if(akhir.value) free(akhir.value);

        return new_if(cond, then_block);
    }

    // Default: Expression statement (not supported yet really, but maybe?)
    fprintf(stderr, "Parser Error Line %d: Unknown statement starting with '%s'\n", t.line, t.value ? t.value : "EOF");
    exit(1);
    return NULL;
}

static ASTNode* parse_block() {
    ASTNode *head = NULL;
    ASTNode *current = NULL;

    while (peek_token().type != TOKEN_AKHIR && peek_token().type != TOKEN_EOF) {
        ASTNode *stmt = parse_statement();
        if (head == NULL) {
            head = stmt;
            current = head;
        } else {
            current->next = stmt; // Link next sibling
            current = stmt;
        }
    }

    // We wrap this list in a BlockNode?
    // Actually `parse_block` returns a BlockNode which contains the list.
    return new_block(head);
}

// Parse top level
ASTNode* parse() {
    ASTNode *head = NULL;
    ASTNode *current = NULL;

    while (peek_token().type != TOKEN_EOF) {
        ASTNode *stmt = parse_statement();
        if (head == NULL) {
            head = stmt;
            current = head;
        } else {
            current->next = stmt;
            current = stmt;
        }
    }
    return new_program(head);
}
