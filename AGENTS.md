# MorphC: The Reborn Native Interpreter

**Status:** Iterasi 3 (AST Transformation)
**Language:** C (C11 Standard)
**Architect:** Vzoel Fox's
**Engineer:** Jules

Dokumen ini adalah satu-satunya sumber kebenaran untuk pengembangan `morphc`. Proyek ini adalah reinkarnasi dari `morphupgrade` dengan pendekatan yang lebih radikal: **Pure C, No Dependencies, AST-Based Interpreter.**

## 1. Filosofi
*   **Efisien bukan Kecepatan:** Kode harus bersih, mudah dimengerti, dan hemat memori. Kecepatan eksekusi adalah bonus.
*   **Jujur:** Tidak menyembunyikan kompleksitas di balik library "magic". Kita bangun dari nol.
*   **Berkesan:** Fokus pada pengalaman pengguna (Developer Experience) yang unik dengan sintaks Bahasa Indonesia.

## 2. Arsitektur Teknis
MorphC bertransformasi menjadi *Tree-Walk Interpreter* berbasis AST.
*   **Lexer:** Source Code -> Tokens.
*   **Parser:** Tokens -> Abstract Syntax Tree (AST).
*   **Evaluator:** AST -> Execution.
*   **Environment:** Penyimpanan variabel (Scope).

## 3. Standar Kode
*   **Bahasa Pemrograman:** C (Standard C11).
*   **Style:** K&R atau Linux Kernel style. Gunakan `snake_case` untuk fungsi dan variabel.
*   **Sintaks Morph:**
    *   Ekstensi: `.fox`
    *   Keyword: `tulis`, `jika`, `maka`, `akhir`, `biar`.
    *   Block: Menggunakan keyword penutup (`akhir`), bukan kurung kurawal `{}` (kecuali internal C).

## 4. Struktur Direktori
*   `src/`: Source code C (`.c`).
*   `include/`: Header files (`.h`).
*   `examples/`: Kode contoh Morph (`.fox`).
*   `bin/`: Hasil kompilasi executable.

## 5. Referensi
*   Konsep AST dan COTC (Core of The Core) diadopsi dari `morphupgrade` (Greenfield).
