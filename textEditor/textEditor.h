#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

// Definisi panjang maksimum untuk nama file dan teks
#define MAX_NAME_LENGTH 100
#define MAX_TEXT_LENGTH 256

// Struktur data untuk menyimpan node dari setiap baris teks
struct LineNode {
    char text[MAX_TEXT_LENGTH]; // Menggunakan array karakter untuk teks
    LineNode* next;
    LineNode* prev;
};

// Struktur data untuk menyimpan aksi undo/redo
struct StackNode {
    char action[10]; // Untuk menyimpan aksi seperti "add" atau "delete"
    char text[MAX_TEXT_LENGTH];
    int position;
    StackNode* next;
};

// Struktur data untuk stack (untuk undo/redo)
struct Stack {
    StackNode* top;
};

// Struktur data untuk file yang menyimpan baris-baris teks
struct File {
    char name[MAX_NAME_LENGTH]; // Nama file sebagai array karakter
    LineNode* head;
    LineNode* tail;
};

// Struktur data untuk menyimpan daftar file
struct FileList {
    File* files; // Array dinamis dari file
    int size;    // Jumlah file saat ini
    int capacity; // Kapasitas array
};

// Fungsi untuk inisialisasi stack
void initStack(Stack& stack);

// Fungsi untuk menginisialisasi FileList
void initFileList(FileList& fileList) ;

// Fungsi untuk mengecek apakah stack kosong
bool isStackEmpty(Stack& stack);

// Fungsi untuk menambahkan aksi ke dalam stack
void push(Stack& stack, const char* action, const char* text, int position);

// Fungsi untuk mengeluarkan aksi dari stack
StackNode* pop(Stack& stack);

// Fungsi untuk inisialisasi FileList
void initFileList(FileList& fileList);

// Fungsi untuk menambah file baru ke dalam FileList
void newFile(FileList& fileList, const char* filename);

// Fungsi untuk menambah baris
void addLine(File& file, const char* text, int position, Stack& undoStack);

// Fungsi untuk mencari kata dalam teks
void searchWord(const File& file, const char* word);

// Fungsi untuk menghapus baris
void deleteLine(File& file, int position, Stack& undoStack);

// Fungsi untuk menampilkan teks dari file
void displayText(const File& file);

// Fungsi untuk menampilkan semua file
void showFiles(const FileList& fileList);

// Fungsi untuk menangani undo
void undo(File& file, Stack& undoStack, Stack& redoStack);

// Fungsi untuk menangani redo
void redo(File& file, Stack& undoStack, Stack& redoStack);

// Fungsi untuk menutup dan menghapus file
void deleteFile(FileList& fileList, int fileIndex);

// Fungsi untuk menampilkan menu
void displayMenu();

// Fungsi untuk menjalankan menu
void runMenu(FileList& fileList);

#endif // TEXTEDITOR_H
