#include <stdio.h>
#include <string.h>
#include "evaluator.h"
#include "env.h"

static Environment *global_env;

void init_evaluator() {
    global_env = env_create();
}

void cleanup_evaluator() {
    if (global_env) {
        env_free(global_env);
    }
}

// Forward decl
static void exec_statement(ASTNode *node);
static int eval_expression(ASTNode *node, Value *out_val);

static int eval_expression(ASTNode *node, Value *out_val) {
    if (node->type == NODE_LITERAL) {
        LiteralNode *l = (LiteralNode*)node;
        if (l->type == TOKEN_STRING) {
            *out_val = make_string(l->string_val);
        } else {
            *out_val = make_number(l->int_val);
        }
        return 1;
    }
    else if (node->type == NODE_VAR_ACCESS) {
        VarAccessNode *v = (VarAccessNode*)node;
        if (!env_get(global_env, v->name, out_val)) {
            fprintf(stderr, "Runtime Error: Variable '%s' not defined.\n", v->name);
            return 0;
        }
        return 1;
    }
    return 0;
}

static void exec_block(ASTNode *node) {
    if (node->type != NODE_BLOCK && node->type != NODE_PROGRAM) return;

    // BlockNode or ProgramNode stores statements in 'statements' field?
    // Wait, ProgramNode struct: ASTNode *statements;
    // ASTNode is a linked list via 'next'.

    ASTNode *current;
    if (node->type == NODE_BLOCK) current = ((BlockNode*)node)->statements;
    else current = ((ProgramNode*)node)->statements;

    while (current) {
        exec_statement(current);
        current = current->next;
    }
}

static void exec_statement(ASTNode *node) {
    switch (node->type) {
        case NODE_PRINT: {
            PrintNode *p = (PrintNode*)node;
            Value val;
            if (eval_expression(p->expression, &val)) {
                if (val.type == VAL_STRING) {
                    printf("%s\n", val.as.string);
                } else {
                    printf("%d\n", val.as.number);
                }
                free_value(val);
            }
            break;
        }
        case NODE_VAR_DECL: {
            VarDeclNode *v = (VarDeclNode*)node;
            Value val;
            if (eval_expression(v->value, &val)) {
                env_set(global_env, v->name, val);
                // Note: env_set takes ownership/copy as discussed before.
                // In our implementation env_set does a SHALLOW copy of the value struct.
                // So if it's a string, the pointer is copied.
                // We should NOT free_value(val) here because the environment now owns the string buffer.
            }
            break;
        }
        case NODE_IF: {
            IfNode *i = (IfNode*)node;
            Value cond;
            if (eval_expression(i->condition, &cond)) {
                // Simplest truthiness: number != 0 is true, string is true
                int is_true = 0;
                if (cond.type == VAL_NUMBER) is_true = (cond.as.number != 0);
                else is_true = 1; // String is always true for now

                if (is_true) {
                    exec_block(i->then_branch);
                }

                // We must free the condition value (it's a temp copy)
                // Wait, if it's a string from env_get, it was duplicated.
                // If it's a literal, it was duplicated.
                // So yes, free it.
                free_value(cond);
            }
            break;
        }
        default:
            break;
    }
}

void evaluate(ASTNode *node) {
    if (node->type == NODE_PROGRAM) {
        exec_block(node);
    }
}
