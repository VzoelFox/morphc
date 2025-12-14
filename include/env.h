#ifndef ENV_H
#define ENV_H

#include <stdlib.h>

typedef enum {
    VAL_NUMBER,
    VAL_STRING
} ValueType;

typedef struct {
    ValueType type;
    union {
        int number;
        char *string;
    } as;
} Value;

typedef struct Entry {
    char *key;
    Value value;
    struct Entry *next;
} Entry;

typedef struct {
    Entry *head;
} Environment;

// Environment functions
Environment* env_create();
void env_free(Environment *env);
void env_set(Environment *env, const char *key, Value value);
int env_get(Environment *env, const char *key, Value *out_value);

// Value helpers
Value make_number(int n);
Value make_string(const char *s);
void free_value(Value v);

#endif
