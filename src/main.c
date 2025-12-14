#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "interpreter.h"

void print_usage(const char *prog_name) {
    printf("Penggunaan: %s <file_source.fox>\n", prog_name);
}

char* read_file(const char* path) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "Gagal membuka file: %s\n", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(length + 1);
    if (!buffer) {
        fprintf(stderr, "Gagal mengalokasi memori.\n");
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    fclose(file);

    return buffer;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    const char *filepath = argv[1];
    char *source = read_file(filepath);

    if (!source) {
        return 1;
    }

    // Inisialisasi Interpreter
    init_interpreter(source);

    // Jalankan
    run_interpreter();

    free(source);
    return 0;
}
