#include <stdio.h>
#include <string.h>
#include "evaluator.h"
#include "env.h"

static Environment *global_env;
static Value last_return_value;
static int is_returning = 0;

void init_evaluator() {
    global_env = env_create(NULL);
    is_returning = 0;
}

void cleanup_evaluator() {
    if (global_env) {
        env_free(global_env);
    }
}

// Forward decl
static void exec_statement(ASTNode *node, Environment *env);
static int eval_expression(ASTNode *node, Environment *env, Value *out_val);

// Helper for arithmetic
static Value eval_binary_op(Value left, TokenType op, Value right) {
    if (left.type == VAL_NUMBER && right.type == VAL_NUMBER) {
        int l = left.as.number;
        int r = right.as.number;
        switch (op) {
            case TOKEN_PLUS: return make_number(l + r);
            case TOKEN_MINUS: return make_number(l - r);
            case TOKEN_STAR: return make_number(l * r);
            case TOKEN_SLASH: return make_number(r != 0 ? l / r : 0);
            case TOKEN_LT: return make_number(l < r);
            case TOKEN_GT: return make_number(l > r);
            case TOKEN_LT_EQ: return make_number(l <= r);
            case TOKEN_GT_EQ: return make_number(l >= r);
            case TOKEN_EQ_EQ: return make_number(l == r);
            case TOKEN_BANG_EQ: return make_number(l != r);
            default: break;
        }
    }

    // String Concatenation
    if (op == TOKEN_PLUS) {
        if (left.type == VAL_STRING && right.type == VAL_STRING) {
            size_t len = strlen(left.as.string) + strlen(right.as.string) + 1;
            char *buf = malloc(len);
            snprintf(buf, len, "%s%s", left.as.string, right.as.string);
            Value v = make_string(buf);
            free(buf); // make_string duplicates it, so we free our temp buf
            return v;
        }
    }

    return make_null();
}

static int eval_expression(ASTNode *node, Environment *env, Value *out_val) {
    if (!node) return 0;

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
        if (!env_get(env, v->name, out_val)) {
            fprintf(stderr, "Runtime Error: Variable '%s' not defined.\n", v->name);
            return 0;
        }
        return 1;
    }
    else if (node->type == NODE_BINARY_EXPR) {
        BinaryExprNode *b = (BinaryExprNode*)node;
        Value left, right;
        if (eval_expression(b->left, env, &left) && eval_expression(b->right, env, &right)) {
            *out_val = eval_binary_op(left, b->op, right);
            free_value(left);
            free_value(right);
            return 1;
        }
        return 0;
    }
    else if (node->type == NODE_CALL_EXPR) {
        CallExprNode *c = (CallExprNode*)node;
        Value func_val;
        if (!env_get(env, c->callee, &func_val)) {
            fprintf(stderr, "Runtime Error: Function '%s' not defined.\n", c->callee);
            return 0;
        }

        if (func_val.type != VAL_FUNCTION) {
             fprintf(stderr, "Runtime Error: '%s' is not a function.\n", c->callee);
             return 0;
        }

        FuncDeclNode *func_decl = (FuncDeclNode*)func_val.as.function.declaration;

        // 1. Create new scope with parent = global (lexical scoping) or current (dynamic)?
        // For simple recursion, parent usually is the env where function was defined (Closure).
        // But we don't store closure env yet. Let's use global_env as parent for now to access globals,
        // or better, use current 'env' if we want dynamic scope (easier for now).
        // Let's use global_env as base to allow recursion accessing global func name.
        Environment *func_env = env_create(global_env);

        // 2. Bind arguments
        ASTNode *param = func_decl->params;
        ASTNode *arg = c->arguments;

        while (param && arg) {
            Value arg_val;
            eval_expression(arg, env, &arg_val); // Eval arg in caller scope

            // Bind to param name in func scope
            char *param_name = ((VarAccessNode*)param)->name; // We used VarAccess for param list
            env_set(func_env, param_name, arg_val);

            param = param->next;
            arg = arg->next;
        }

        // 3. Exec Body
        exec_statement(func_decl->body, func_env);

        // 4. Cleanup
        env_free(func_env);

        // 5. Return result
        if (is_returning) {
            *out_val = last_return_value; // Move ownership
            is_returning = 0;
        } else {
            *out_val = make_null();
        }
        return 1;
    }

    return 0;
}

static void exec_block(ASTNode *node, Environment *env) {
    ASTNode *current;
    if (node->type == NODE_BLOCK) current = ((BlockNode*)node)->statements;
    else current = ((ProgramNode*)node)->statements;

    while (current) {
        if (is_returning) break;
        exec_statement(current, env);
        current = current->next;
    }
}

static void exec_statement(ASTNode *node, Environment *env) {
    if (is_returning) return;

    switch (node->type) {
        case NODE_PRINT: {
            PrintNode *p = (PrintNode*)node;
            Value val;
            if (eval_expression(p->expression, env, &val)) {
                if (val.type == VAL_STRING) {
                    printf("%s\n", val.as.string);
                } else if (val.type == VAL_NUMBER) {
                    printf("%d\n", val.as.number);
                }
                free_value(val);
            }
            break;
        }
        case NODE_VAR_DECL: {
            VarDeclNode *v = (VarDeclNode*)node;
            Value val;
            if (eval_expression(v->value, env, &val)) {
                env_set(env, v->name, val);
            }
            break;
        }
        case NODE_FUNC_DECL: {
            FuncDeclNode *f = (FuncDeclNode*)node;
            Value val = make_function((ASTNode*)f);
            env_set(env, f->name, val);
            break;
        }
        case NODE_RETURN: {
            ReturnNode *r = (ReturnNode*)node;
            Value val;
            if (eval_expression(r->value, env, &val)) {
                last_return_value = val;
                is_returning = 1;
            }
            break;
        }
        case NODE_IF: {
            IfNode *i = (IfNode*)node;
            Value cond;
            if (eval_expression(i->condition, env, &cond)) {
                int is_true = 0;
                if (cond.type == VAL_NUMBER) is_true = (cond.as.number != 0);
                else is_true = 1;

                if (is_true) {
                    exec_block(i->then_branch, env);
                }
                free_value(cond);
            }
            break;
        }
        case NODE_BINARY_EXPR:
        case NODE_CALL_EXPR: {
            // Expression statement
            Value v;
            if (eval_expression(node, env, &v)) {
                free_value(v);
            }
            break;
        }
        default:
            break;
    }
}

void evaluate(ASTNode *node) {
    if (node->type == NODE_PROGRAM) {
        exec_block(node, global_env);
    }
}
