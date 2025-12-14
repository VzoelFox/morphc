#ifndef AST_H
#define AST_H

#include "lexer.h"

typedef enum {
    NODE_PROGRAM,
    NODE_BLOCK,
    NODE_VAR_DECL,
    NODE_PRINT,
    NODE_IF,

    // Expressions
    NODE_LITERAL,
    NODE_VAR_ACCESS,
    NODE_BINARY_EXPR,
    NODE_CALL_EXPR,

    // Functions
    NODE_FUNC_DECL,
    NODE_RETURN
} NodeType;

typedef struct ASTNode {
    NodeType type;
    struct ASTNode *next;
} ASTNode;

// --- Expressions ---

typedef struct {
    ASTNode base;
    TokenType type;
    char *string_val;
    int int_val;
} LiteralNode;

typedef struct {
    ASTNode base;
    char *name;
} VarAccessNode;

typedef struct {
    ASTNode base;
    ASTNode *left;
    TokenType op; // Operator token type
    ASTNode *right;
} BinaryExprNode;

typedef struct {
    ASTNode base;
    char *callee;
    ASTNode *arguments; // Linked list of expressions
} CallExprNode;

// --- Statements ---

typedef struct {
    ASTNode base;
    ASTNode *statements;
} ProgramNode;

typedef struct {
    ASTNode base;
    ASTNode *statements;
} BlockNode;

typedef struct {
    ASTNode base;
    char *name;
    ASTNode *value;
} VarDeclNode;

typedef struct {
    ASTNode base;
    ASTNode *expression;
} PrintNode;

typedef struct {
    ASTNode base;
    ASTNode *condition;
    ASTNode *then_branch;
} IfNode;

// --- Functions ---

typedef struct {
    ASTNode base;
    char *name;
    ASTNode *params; // Linked list of VarAccessNode (abusing it for param names) or similar
    ASTNode *body;   // BlockNode
} FuncDeclNode;

typedef struct {
    ASTNode base;
    ASTNode *value;
} ReturnNode;


// --- Constructors ---
ASTNode* new_program(ASTNode *stmts);
ASTNode* new_block(ASTNode *stmts);
ASTNode* new_var_decl(const char *name, ASTNode *val);
ASTNode* new_print(ASTNode *expr);
ASTNode* new_if(ASTNode *cond, ASTNode *then_block);

ASTNode* new_literal_string(const char *val);
ASTNode* new_literal_number(int val);
ASTNode* new_var_access(const char *name);

ASTNode* new_binary_expr(ASTNode *left, TokenType op, ASTNode *right);
ASTNode* new_call_expr(const char *callee, ASTNode *args);

ASTNode* new_func_decl(const char *name, ASTNode *params, ASTNode *body);
ASTNode* new_return(ASTNode *val);

// Helpers
ASTNode* append_node(ASTNode *head, ASTNode *node); // Helper to append to linked list

void free_ast(ASTNode *node);

#endif
