#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "lexer.h"

// Forward declarations
static ASTNode* parse_statement();
static ASTNode* parse_expression();
static ASTNode* parse_block();
static ASTNode* parse_logic_or();

void init_parser(const char *source) {
    init_lexer(source);
}

static Token consume(TokenType type, const char *err_msg) {
    Token t = next_token();
    if (t.type == type) return t;

    fprintf(stderr, "Parser Error Line %d: %s. Found token type %d ('%s')\n", t.line, err_msg, t.type, t.value);
    exit(1);
}

// --- Expression Parsing (Precedence) ---

// Primary: Literal, Var, Grouping, Call
static ASTNode* parse_primary() {
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
        // Check if function call
        if (peek_token().type == TOKEN_LPAREN) {
            char *callee = strdup(t.value); // Copy name before freeing token
            free(t.value);

            Token lp = consume(TOKEN_LPAREN, "Expected '('");
            if(lp.value) free(lp.value);

            ASTNode *args = NULL;
            if (peek_token().type != TOKEN_RPAREN) {
                args = parse_expression();
                while (peek_token().type == TOKEN_COMMA) {
                    Token cm = next_token(); // consume ,
                    if(cm.value) free(cm.value);

                    ASTNode *next_arg = parse_expression();
                    append_node(args, next_arg);
                }
            }

            Token rp = consume(TOKEN_RPAREN, "Expected ')'");
            if(rp.value) free(rp.value);

            return new_call_expr(callee, args);
        } else {
            // Just Variable Access
            ASTNode *node = new_var_access(t.value);
            free(t.value);
            return node;
        }
    }
    else if (t.type == TOKEN_LPAREN) {
        free(t.value);
        ASTNode *expr = parse_expression();
        Token rp = consume(TOKEN_RPAREN, "Expected ')' after expression");
        if(rp.value) free(rp.value);
        return expr;
    }

    fprintf(stderr, "Parser Error Line %d: Unexpected primary token type %d\n", t.line, t.type);
    if(t.value) free(t.value);
    return NULL;
}

// Unary: - (Negation), ! (Not) - Skip for now, straight to Mult

// Multiplication: * /
static ASTNode* parse_factor() {
    ASTNode *expr = parse_primary();

    while (peek_token().type == TOKEN_STAR || peek_token().type == TOKEN_SLASH) {
        Token op = next_token();
        TokenType type = op.type;
        if(op.value) free(op.value);

        ASTNode *right = parse_primary();
        expr = new_binary_expr(expr, type, right);
    }
    return expr;
}

// Addition: + -
static ASTNode* parse_term() {
    ASTNode *expr = parse_factor();

    while (peek_token().type == TOKEN_PLUS || peek_token().type == TOKEN_MINUS) {
        Token op = next_token();
        TokenType type = op.type;
        if(op.value) free(op.value);

        ASTNode *right = parse_factor();
        expr = new_binary_expr(expr, type, right);
    }
    return expr;
}

// Comparison: < > <= >=
static ASTNode* parse_comparison() {
    ASTNode *expr = parse_term();

    while (peek_token().type == TOKEN_LT || peek_token().type == TOKEN_GT ||
           peek_token().type == TOKEN_LT_EQ || peek_token().type == TOKEN_GT_EQ) {
        Token op = next_token();
        TokenType type = op.type;
        if(op.value) free(op.value);

        ASTNode *right = parse_term();
        expr = new_binary_expr(expr, type, right);
    }
    return expr;
}

// Equality: == !=
static ASTNode* parse_equality() {
    ASTNode *expr = parse_comparison();

    while (peek_token().type == TOKEN_EQ_EQ || peek_token().type == TOKEN_BANG_EQ) {
        Token op = next_token();
        TokenType type = op.type;
        if(op.value) free(op.value);

        ASTNode *right = parse_comparison();
        expr = new_binary_expr(expr, type, right);
    }
    return expr;
}

// Entry Point for Expressions
static ASTNode* parse_expression() {
    return parse_equality(); // Start from lowest precedence
}


// --- Statement Parsing ---

static ASTNode* parse_block() {
    ASTNode *head = NULL;
    ASTNode *current = NULL;

    // Block stops at AKHIR. But does it stop at KEMBALI?
    // A block inside a function might contain KEMBALI as one of its statements.
    // So parse_block should NOT stop at KEMBALI.
    // It should consume KEMBALI as a statement.
    // The only stopper is AKHIR (end of block).

    while (peek_token().type != TOKEN_AKHIR && peek_token().type != TOKEN_EOF) {

        ASTNode *stmt = parse_statement();
        if (!stmt) break; // Should not happen unless error

        if (head == NULL) {
            head = stmt;
            current = head;
        } else {
            current->next = stmt;
            current = stmt;
        }
    }
    return new_block(head);
}

static ASTNode* parse_statement() {
    Token t = peek_token();

    // 1. Tulis
    if (t.type == TOKEN_TULIS) {
        Token tok = next_token();
        if(tok.value) free(tok.value);
        ASTNode *expr = parse_expression();
        return new_print(expr);
    }

    // 2. Biar (Variable)
    if (t.type == TOKEN_BIAR) {
        Token tok = next_token();
        if(tok.value) free(tok.value);

        Token id = consume(TOKEN_IDENTIFIER, "Diharapkan nama variabel");
        Token eq = consume(TOKEN_EQUALS, "Diharapkan '='");
        if(eq.value) free(eq.value);

        ASTNode *expr = parse_expression();
        ASTNode *node = new_var_decl(id.value, expr);
        free(id.value);
        return node;
    }

    // 3. Jika (If)
    if (t.type == TOKEN_JIKA) {
        Token tok = next_token();
        if(tok.value) free(tok.value);

        ASTNode *cond = parse_expression();
        Token maka = consume(TOKEN_MAKA, "Diharapkan 'maka'");
        if(maka.value) free(maka.value);

        ASTNode *then_block = parse_block();

        Token akhir = consume(TOKEN_AKHIR, "Diharapkan 'akhir'");
        if(akhir.value) free(akhir.value);

        return new_if(cond, then_block);
    }

    // 4. Fungsi
    if (t.type == TOKEN_FUNGSI) {
        Token tok = next_token();
        if(tok.value) free(tok.value);

        Token name = consume(TOKEN_IDENTIFIER, "Diharapkan nama fungsi");
        Token lp = consume(TOKEN_LPAREN, "Diharapkan '('");
        if(lp.value) free(lp.value);

        ASTNode *params = NULL;
        if (peek_token().type != TOKEN_RPAREN) {
            Token p = consume(TOKEN_IDENTIFIER, "Diharapkan nama parameter");
            params = new_var_access(p.value); // Use VarAccess as Param Node holder
            free(p.value);

            while (peek_token().type == TOKEN_COMMA) {
                Token cm = next_token();
                if(cm.value) free(cm.value);

                Token pn = consume(TOKEN_IDENTIFIER, "Diharapkan nama parameter");
                append_node(params, new_var_access(pn.value));
                free(pn.value);
            }
        }

        Token rp = consume(TOKEN_RPAREN, "Diharapkan ')'");
        if(rp.value) free(rp.value);

        ASTNode *body = parse_block();

        Token akhir = consume(TOKEN_AKHIR, "Diharapkan 'akhir' setelah fungsi");
        if(akhir.value) free(akhir.value);

        ASTNode *node = new_func_decl(name.value, params, body);
        free(name.value);
        return node;
    }

    // 5. Kembali (Return)
    if (t.type == TOKEN_KEMBALI) {
        Token tok = next_token();
        if(tok.value) free(tok.value);

        ASTNode *val = parse_expression();
        return new_return(val);
    }

    // Default: Expression statement (like function call: print())
    ASTNode *expr = parse_expression();
    return expr;
    // Note: This effectively allows "1+1" as a statement (no-op but parsed).
}

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
