#include <stdio.h>
#include <string.h>
#include "env.h"
#include "ast.h" // Need ASTNode definition for function

// --- Value Helpers ---

Value make_number(int n) {
    Value v;
    v.type = VAL_NUMBER;
    v.as.number = n;
    return v;
}

Value make_string(const char *s) {
    Value v;
    v.type = VAL_STRING;
    v.as.string = strdup(s);
    return v;
}

Value make_function(ASTNode *decl) {
    Value v;
    v.type = VAL_FUNCTION;
    v.as.function.declaration = decl;
    return v;
}

Value make_null() {
    Value v;
    v.type = VAL_NULL;
    return v;
}

void free_value(Value v) {
    if (v.type == VAL_STRING && v.as.string) {
        free(v.as.string);
    }
    // Function declaration is part of AST, owned by AST, not Value.
}

// --- Environment Implementation ---

Environment* env_create(Environment *parent) {
    Environment *env = malloc(sizeof(Environment));
    env->head = NULL;
    env->parent = parent;
    return env;
}

void env_free(Environment *env) {
    Entry *current = env->head;
    while (current) {
        Entry *next = current->next;
        free(current->key);
        free_value(current->value);
        free(current);
        current = next;
    }
    free(env);
}

void env_set(Environment *env, const char *key, Value value) {
    // 1. Check if variable exists in current scope to update
    Entry *current = env->head;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            free_value(current->value);
            current->value = value;
            return;
        }
        current = current->next;
    }

    // 2. If not in current, check parent?
    // Usually assignment updates nearest scope. Declaration creates in current.
    // Our 'biar' is declaration. So it always goes to current.
    // But what about reassignment? e.g. x = 5 (without biar).
    // We don't have assignment syntax yet, only 'biar'.
    // So 'biar' ALWAYS creates/updates in local scope.

    // Create new entry
    Entry *new_entry = malloc(sizeof(Entry));
    new_entry->key = strdup(key);
    new_entry->value = value;
    new_entry->next = env->head;
    env->head = new_entry;
}

int env_get(Environment *env, const char *key, Value *out_value) {
    Environment *current_env = env;
    while (current_env) {
        Entry *current = current_env->head;
        while (current) {
            if (strcmp(current->key, key) == 0) {
                if (out_value) {
                    *out_value = current->value;
                    if (current->value.type == VAL_STRING) {
                        out_value->as.string = strdup(current->value.as.string);
                    }
                    // Function is pointer copy (safe, AST owns it)
                }
                return 1;
            }
            current = current->next;
        }
        current_env = current_env->parent;
    }
    return 0; // Not found
}
