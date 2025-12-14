#include <stdlib.h>
#include <string.h>
#include "ast.h"

static void* alloc_node(size_t size, NodeType type) {
    ASTNode *node = calloc(1, size);
    node->type = type;
    node->next = NULL;
    return node;
}

ASTNode* append_node(ASTNode *head, ASTNode *node) {
    if (!head) return node;
    ASTNode *current = head;
    while (current->next) {
        current = current->next;
    }
    current->next = node;
    return head;
}

ASTNode* new_program(ASTNode *stmts) {
    ProgramNode *node = alloc_node(sizeof(ProgramNode), NODE_PROGRAM);
    node->statements = stmts;
    return (ASTNode*)node;
}

ASTNode* new_block(ASTNode *stmts) {
    BlockNode *node = alloc_node(sizeof(BlockNode), NODE_BLOCK);
    node->statements = stmts;
    return (ASTNode*)node;
}

ASTNode* new_var_decl(const char *name, ASTNode *val) {
    VarDeclNode *node = alloc_node(sizeof(VarDeclNode), NODE_VAR_DECL);
    node->name = strdup(name);
    node->value = val;
    return (ASTNode*)node;
}

ASTNode* new_print(ASTNode *expr) {
    PrintNode *node = alloc_node(sizeof(PrintNode), NODE_PRINT);
    node->expression = expr;
    return (ASTNode*)node;
}

ASTNode* new_if(ASTNode *cond, ASTNode *then_block) {
    IfNode *node = alloc_node(sizeof(IfNode), NODE_IF);
    node->condition = cond;
    node->then_branch = then_block;
    return (ASTNode*)node;
}

ASTNode* new_literal_string(const char *val) {
    LiteralNode *node = alloc_node(sizeof(LiteralNode), NODE_LITERAL);
    node->type = TOKEN_STRING;
    node->string_val = strdup(val);
    return (ASTNode*)node;
}

ASTNode* new_literal_number(int val) {
    LiteralNode *node = alloc_node(sizeof(LiteralNode), NODE_LITERAL);
    node->type = TOKEN_NUMBER;
    node->int_val = val;
    return (ASTNode*)node;
}

ASTNode* new_var_access(const char *name) {
    VarAccessNode *node = alloc_node(sizeof(VarAccessNode), NODE_VAR_ACCESS);
    node->name = strdup(name);
    return (ASTNode*)node;
}

ASTNode* new_binary_expr(ASTNode *left, TokenType op, ASTNode *right) {
    BinaryExprNode *node = alloc_node(sizeof(BinaryExprNode), NODE_BINARY_EXPR);
    node->left = left;
    node->op = op;
    node->right = right;
    return (ASTNode*)node;
}

ASTNode* new_call_expr(const char *callee, ASTNode *args) {
    CallExprNode *node = alloc_node(sizeof(CallExprNode), NODE_CALL_EXPR);
    node->callee = strdup(callee);
    node->arguments = args;
    return (ASTNode*)node;
}

ASTNode* new_func_decl(const char *name, ASTNode *params, ASTNode *body) {
    FuncDeclNode *node = alloc_node(sizeof(FuncDeclNode), NODE_FUNC_DECL);
    node->name = strdup(name);
    node->params = params;
    node->body = body;
    return (ASTNode*)node;
}

ASTNode* new_return(ASTNode *val) {
    ReturnNode *node = alloc_node(sizeof(ReturnNode), NODE_RETURN);
    node->value = val;
    return (ASTNode*)node;
}

void free_ast(ASTNode *node) {
    if (!node) return;

    free_ast(node->next);

    switch (node->type) {
        case NODE_PROGRAM: {
            ProgramNode *n = (ProgramNode*)node;
            free_ast(n->statements);
            break;
        }
        case NODE_BLOCK: {
            BlockNode *n = (BlockNode*)node;
            free_ast(n->statements);
            break;
        }
        case NODE_VAR_DECL: {
            VarDeclNode *n = (VarDeclNode*)node;
            free(n->name);
            free_ast(n->value);
            break;
        }
        case NODE_PRINT: {
            PrintNode *n = (PrintNode*)node;
            free_ast(n->expression);
            break;
        }
        case NODE_IF: {
            IfNode *n = (IfNode*)node;
            free_ast(n->condition);
            free_ast(n->then_branch);
            break;
        }
        case NODE_LITERAL: {
            LiteralNode *n = (LiteralNode*)node;
            if (n->type == TOKEN_STRING) free(n->string_val);
            break;
        }
        case NODE_VAR_ACCESS: {
            VarAccessNode *n = (VarAccessNode*)node;
            free(n->name);
            break;
        }
        case NODE_BINARY_EXPR: {
            BinaryExprNode *n = (BinaryExprNode*)node;
            free_ast(n->left);
            free_ast(n->right);
            break;
        }
        case NODE_CALL_EXPR: {
            CallExprNode *n = (CallExprNode*)node;
            free(n->callee);
            free_ast(n->arguments);
            break;
        }
        case NODE_FUNC_DECL: {
            FuncDeclNode *n = (FuncDeclNode*)node;
            free(n->name);
            free_ast(n->params);
            free_ast(n->body);
            break;
        }
        case NODE_RETURN: {
            ReturnNode *n = (ReturnNode*)node;
            free_ast(n->value);
            break;
        }
    }
    free(node);
}
