#ifndef ENV_H
#define ENV_H

#include <stdlib.h>

typedef enum {
    VAL_NUMBER,
    VAL_STRING,
    VAL_FUNCTION, // New: Function Value
    VAL_NULL
} ValueType;

struct ASTNode; // Forward declaration

typedef struct Value {
    ValueType type;
    union {
        int number;
        char *string;
        struct { // Function closure/pointer
            struct ASTNode *declaration; // Point to FuncDeclNode
        } function;
    } as;
} Value;

typedef struct Entry {
    char *key;
    Value value;
    struct Entry *next;
} Entry;

typedef struct Environment {
    Entry *head;
    struct Environment *parent; // Parent Scope
} Environment;

// Environment functions
Environment* env_create(Environment *parent);
void env_free(Environment *env);
void env_set(Environment *env, const char *key, Value value);
int env_get(Environment *env, const char *key, Value *out_value);

// Value helpers
Value make_number(int n);
Value make_string(const char *s);
Value make_function(struct ASTNode *decl);
Value make_null();
void free_value(Value v);

#endif
