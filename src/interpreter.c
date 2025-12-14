#include <stdio.h>
#include <stdlib.h>
#include "interpreter.h"
#include "lexer.h"

void init_interpreter(const char *source) {
    init_lexer(source);
}

void run_interpreter() {
    Token token = next_token();

    while (token.type != TOKEN_EOF) {
        if (token.type == TOKEN_TULIS) {
            free(token.value); // Hapus token TULIS

            token = next_token(); // Harapkan String
            if (token.type == TOKEN_STRING) {
                printf("%s\n", token.value);
            } else {
                fprintf(stderr, "Error Baris %d: Diharapkan string setelah 'tulis'.\n", token.line);
            }
        } else {
            // Abaikan token tak dikenal untuk saat ini
        }

        if (token.value) free(token.value);
        token = next_token();
    }
}
