#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpreter.h"
#include "lexer.h"
#include "env.h"

static Environment *global_env;

void init_interpreter(const char *source) {
    init_lexer(source);
    global_env = env_create();
}

// Helper untuk membersihkan environment saat selesai
void cleanup_interpreter() {
    if (global_env) {
        env_free(global_env);
    }
}

// Helper untuk memproses expression sederhana (Value atau Identifier)
static int evaluate_expression(Token current_token, Value *out_val) {
    if (current_token.type == TOKEN_STRING) {
        *out_val = make_string(current_token.value);
        return 1;
    }
    else if (current_token.type == TOKEN_NUMBER) {
        *out_val = make_number(atoi(current_token.value));
        return 1;
    }
    else if (current_token.type == TOKEN_IDENTIFIER) {
        if (env_get(global_env, current_token.value, out_val)) {
            return 1;
        } else {
            fprintf(stderr, "Error Baris %d: Variabel '%s' tidak ditemukan.\n", current_token.line, current_token.value);
            return 0;
        }
    }
    return 0;
}

void run_interpreter() {
    Token token = next_token();

    while (token.type != TOKEN_EOF) {

        // --- STATEMENT: TULIS ---
        if (token.type == TOKEN_TULIS) {
            free(token.value);
            token = next_token(); // Expect Expression

            Value val;
            if (evaluate_expression(token, &val)) {
                if (val.type == VAL_STRING) {
                    printf("%s\n", val.as.string);
                } else if (val.type == VAL_NUMBER) {
                    printf("%d\n", val.as.number);
                }
                free_value(val);
            } else {
                 if (token.type != TOKEN_UNKNOWN) // Don't double error if evaluate_expression already printed one
                    fprintf(stderr, "Error Baris %d: Invalid expression untuk 'tulis'.\n", token.line);
            }
        }

        // --- STATEMENT: BIAR (Variable Declaration) ---
        else if (token.type == TOKEN_BIAR) {
            free(token.value);

            // 1. Expect Identifier
            token = next_token();
            if (token.type != TOKEN_IDENTIFIER) {
                fprintf(stderr, "Error Baris %d: Diharapkan nama variabel setelah 'biar'.\n", token.line);
                if(token.value) free(token.value);
                break; // Panic recovery: stop for now
            }
            char *var_name = strdup(token.value); // Simpan nama variabel sementara
            free(token.value);

            // 2. Expect Equals '='
            token = next_token();
            if (token.type != TOKEN_EQUALS) {
                fprintf(stderr, "Error Baris %d: Diharapkan '=' setelah nama variabel.\n", token.line);
                free(var_name);
                if(token.value) free(token.value);
                break;
            }
            free(token.value);

            // 3. Expect Expression (Value)
            token = next_token();
            Value val;
            if (evaluate_expression(token, &val)) {
                env_set(global_env, var_name, val); // Store deep copy logic is in env_set if needed, but here val is fresh
                // env_set takes ownership or copy?
                // Our env.c: env_set stores `Value` struct assignment.
                // Value struct has `char* string` which is strdup'd in make_string.
                // So environment now owns the resource inside `val`.
                // However, env_set implementation does NOT strdup the value content again unless we change it.
                // Looking at env.c: `new_entry->value = value;` -> Shallow copy of struct.
                // This means `val` ownership is transferred to Env. We should NOT free_value(val) here.

                // Correction on `evaluate_expression` for Identifier:
                // It calls `env_get` which does a deep copy for Strings.
                // So `val` is always a fresh object that needs to be managed.
                // If we pass it to `env_set`, `env_set` takes it.
                // So we are good.
            } else {
                fprintf(stderr, "Error Baris %d: Nilai invalid untuk variabel '%s'.\n", token.line, var_name);
            }
            free(var_name);
        }

        else {
            // Ignore unknown tokens or extra newlines for now to prevent infinite loop on errors
            // but for now loop continues
        }

        if (token.value) free(token.value);
        token = next_token();
    }

    cleanup_interpreter();
}
