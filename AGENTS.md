# MorphC: The Reborn Native Interpreter

**Status:** Awal Perjalanan (Rebirth Phase)
**Language:** C (C11 Standard)
**Architect:** Vzoel Fox's
**Engineer:** Jules

Dokumen ini adalah satu-satunya sumber kebenaran untuk pengembangan `morphc`. Proyek ini adalah reinkarnasi dari `morphupgrade` dengan pendekatan yang lebih radikal: **Pure C, No Dependencies, Interpreter First.**

## 1. Filosofi
*   **Efisien bukan Kecepatan:** Kode harus bersih, mudah dimengerti, dan hemat memori. Kecepatan eksekusi adalah bonus.
*   **Jujur:** Tidak menyembunyikan kompleksitas di balik library "magic". Kita bangun dari nol.
*   **Berkesan:** Fokus pada pengalaman pengguna (Developer Experience) yang unik dengan sintaks Bahasa Indonesia.

## 2. Arsitektur Teknis
MorphC adalah *Tree-Walk Interpreter* (awalnya) yang ditulis dalam C murni.
*   **Lexer:** Mengubah source code `.fox` menjadi Token.
*   **Parser:** Membaca Token dan membuat struktur eksekusi (atau langsung eksekusi untuk tahap awal).
*   **Runtime:** Eksekusi langsung (tidak ada Virtual Machine bytecode untuk iterasi pertama).

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

## 5. Roadmap
1.  **Iterasi 1:** Hello World (`tulis`).
2.  **Iterasi 2:** Variabel & Tipe Data Dasar.
3.  **Iterasi 3:** Kontrol Alur (`jika`, `ulang`).

---
*Catatan: Jangan lupa bernafas. Coding itu seni.*
