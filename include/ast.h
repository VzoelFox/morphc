#ifndef AST_H
#define AST_H

#include "lexer.h"

typedef enum {
    NODE_PROGRAM,
    NODE_BLOCK,
    NODE_VAR_DECL,
    NODE_PRINT,
    NODE_IF,
    NODE_LITERAL,
    NODE_VAR_ACCESS
} NodeType;

typedef struct ASTNode {
    NodeType type;
    struct ASTNode *next; // Untuk linked list statement dalam block/program
} ASTNode;

// --- Expressions ---

typedef struct {
    ASTNode base;
    TokenType type; // TOKEN_STRING atau TOKEN_NUMBER
    char *string_val;
    int int_val;
} LiteralNode;

typedef struct {
    ASTNode base;
    char *name;
} VarAccessNode;

// --- Statements ---

typedef struct {
    ASTNode base;
    ASTNode *statements; // Linked list of statements
} ProgramNode;

typedef struct {
    ASTNode base;
    ASTNode *statements;
} BlockNode;

typedef struct {
    ASTNode base;
    char *name;
    ASTNode *value; // Expression
} VarDeclNode;

typedef struct {
    ASTNode base;
    ASTNode *expression;
} PrintNode;

typedef struct {
    ASTNode base;
    ASTNode *condition;
    ASTNode *then_branch; // BlockNode
    // ASTNode *else_branch; // Nanti
} IfNode;

// --- Constructors ---
ASTNode* new_program(ASTNode *stmts);
ASTNode* new_block(ASTNode *stmts);
ASTNode* new_var_decl(const char *name, ASTNode *val);
ASTNode* new_print(ASTNode *expr);
ASTNode* new_if(ASTNode *cond, ASTNode *then_block);
ASTNode* new_literal_string(const char *val);
ASTNode* new_literal_number(int val);
ASTNode* new_var_access(const char *name);

void free_ast(ASTNode *node);

#endif
