# Roadmap MorphC

## Visi
Membangun ulang ekosistem Morph dengan fondasi C murni yang efisien, mandiri, dan berkesan. Mengadopsi desain "Morph as Kernel" dengan arsitektur AST-Based Interpreter.

## Tahap 1: Fondasi (Selesai)
- [x] Inisialisasi Proyek & Makefile.
- [x] Lexer Dasar (`tulis`, String, Number).
- [x] Interpreter Dasar (Line-by-Line).
- [x] Variabel (`biar`) & Environment.

## Tahap 2: Transformasi Arsitektur (Iterasi Saat Ini)
- [ ] **AST (Abstract Syntax Tree):** Migrasi dari eksekusi langsung ke struktur pohon.
- [ ] **Parser:** Memisahkan logika parsing dari eksekusi.
- [ ] **Evaluator:** Tree-walker untuk mengeksekusi AST.
- [ ] **Kontrol Alur:** Implementasi `jika`, `maka`, `akhir`.

## Tahap 3: Ekspansi & Kompatibilitas
- [ ] **Operasi Matematika & Logika:** `+`, `-`, `*`, `/`, `==`, `!=`.
- [ ] **Perulangan:** `ulang`, `selama`.
- [ ] **Fungsi:** Definisi dan pemanggilan fungsi (`fungsi`, `kembali`).
- [ ] **Adopsi COTC:** Porting Standard Library dari `morphupgrade`.

## Tahap 4: Fitur Lanjut
- [ ] **Sistem Tipe:** Pengecekan tipe yang lebih ketat.
- [ ] **Modul:** Sistem import file lain.
- [ ] **Optimasi:** Garbage Collection sederhana.
- [ ] **Self-Hosting:** Mencoba menulis parser Morph dalam Morph.
