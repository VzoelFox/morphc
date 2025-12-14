#include <stdio.h>
#include <string.h>
#include "env.h"

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
    // Duplicate string to own the memory
    v.as.string = strdup(s);
    return v;
}

void free_value(Value v) {
    if (v.type == VAL_STRING && v.as.string) {
        free(v.as.string);
    }
}

// --- Environment Implementation (Simple Linked List) ---

Environment* env_create() {
    Environment *env = malloc(sizeof(Environment));
    env->head = NULL;
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
    // Check if update existing
    Entry *current = env->head;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Update value: free old, set new
            free_value(current->value);
            current->value = value;
            return;
        }
        current = current->next;
    }

    // Create new entry
    Entry *new_entry = malloc(sizeof(Entry));
    new_entry->key = strdup(key);
    new_entry->value = value;
    new_entry->next = env->head;
    env->head = new_entry;
}

int env_get(Environment *env, const char *key, Value *out_value) {
    Entry *current = env->head;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            if (out_value) {
                // Return a copy (shallow copy for primitives, deep for string)
                // For strings, we need to decide ownership.
                // Currently, `Value` struct owns the string pointer.
                // If we return, we probably want to return a COPY of the value for usage,
                // OR a pointer to the value in the env.

                // Let's implement deep copy for safety in retrieval to avoid double free issues
                // if the caller frees the retrieved value.
                *out_value = current->value;
                if (current->value.type == VAL_STRING) {
                    out_value->as.string = strdup(current->value.as.string);
                }
            }
            return 1; // Found
        }
        current = current->next;
    }
    return 0; // Not found
}
