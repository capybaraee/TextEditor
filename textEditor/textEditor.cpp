#include "textEditor.h"
#include <cstring> // Untuk strcpy, strcmp
#include <iostream>

// Fungsi untuk inisialisasi stack
void initStack(Stack& stack) {
    stack.top = nullptr;
}

void initFileList(FileList& fileList) {
    fileList.files = nullptr;
    fileList.size = 0;
    fileList.capacity = 0;
}

// Fungsi untuk mengecek apakah stack kosong
bool isStackEmpty(Stack& stack) {
    return stack.top == nullptr;
}

// Fungsi untuk menambahkan aksi ke dalam stack
void push(Stack& stack, const char* action, const char* text, int position) {
    StackNode* newNode = new StackNode;
    strcpy(newNode->action, action);
    strcpy(newNode->text, text);
    newNode->position = position;
    newNode->next = stack.top;
    stack.top = newNode;
}

// Fungsi untuk mengeluarkan aksi dari stack
StackNode* pop(Stack& stack) {
    if (isStackEmpty(stack)) return nullptr;
    StackNode* node = stack.top;
    stack.top = stack.top->next;
    return node;
}

// Fungsi untuk membuat file baru
void newFile(FileList& fileList, const char* filename) {
    // Periksa kapasitas dan tambahkan ruang jika perlu
    if (fileList.size == fileList.capacity) {
        int newCapacity = fileList.capacity == 0 ? 1 : fileList.capacity * 2;
        File* newFiles = new File[newCapacity];

        for (int i = 0; i < fileList.size; i++) {
            newFiles[i] = fileList.files[i]; // Salin file lama
        }

        delete[] fileList.files; // Hapus array lama
        fileList.files = newFiles;
        fileList.capacity = newCapacity;
    }

    // Tambahkan file baru
    File newFile;
    strcpy(newFile.name, filename);
    newFile.head = nullptr;
    newFile.tail = nullptr;

    fileList.files[fileList.size++] = newFile; // Tambahkan ke daftar
    std::cout << "File " << filename << " created." << std::endl;
}


// Fungsi untuk menambah teks
void addText(File& file, const char* text, Stack& undoStack) {
    LineNode* newNode = new LineNode;
    strcpy(newNode->text, text);
    newNode->next = nullptr;
    newNode->prev = nullptr;

    if (file.head == nullptr) {
        file.head = newNode;
        file.tail = newNode;
    } else {
        file.tail->next = newNode;
        newNode->prev = file.tail;
        file.tail = newNode;
    }
    push(undoStack, "add", text, -1); // -1 menunjukkan posisi tidak spesifik
    std::cout << "Text added to file: " << text << std::endl;
}


// Fungsi untuk mencari kata dalam teks
void searchWord(const File& file, const char* word) {
    LineNode* current = file.head;
    int lineNumber = 1;
    bool found = false;

    while (current) {
        if (strstr(current->text, word)) {
            std::cout << "Found '" << word << "' at line " << lineNumber << ": " << current->text << std::endl;
            found = true;
        }
        current = current->next;
        lineNumber++;
    }

    if (!found) {
        std::cout << "Word '" << word << "' not found in the file." << std::endl;
    }
}

// Fungsi untuk menghapus baris
void deleteLine(File& file, int position, Stack& undoStack) {
    if (file.head == nullptr) return;
    LineNode* current = file.head;
    int currentIndex = 0;
    while (current && currentIndex < position) {
        current = current->next;
        currentIndex++;
    }
    if (!current) {
        std::cout << "Invalid line number!" << std::endl;
        return;
    }
    // Perbarui pointer berdasarkan posisi current node
    if (current == file.head) {
        file.head = current->next;
    } else if (current->prev) {
        current->prev->next = current->next;
    }

    if (current == file.tail) {
        file.tail = current->prev;
    } else if (current->next) {
        current->next->prev = current->prev;
    }
    // Simpan aksi di undo stack dan hapus node
    push(undoStack, "delete", current->text, position);
    delete current;
    std::cout << "Line " << position + 1 << " deleted." << std::endl;
}

// Fungsi untuk menampilkan teks dari file
void displayText(const File& file) {
    LineNode* current = file.head;
    while (current) {
        std::cout << current->text << std::endl;
        current = current->next;
    }
}

// Fungsi untuk menampilkan semua file
void showFiles(const FileList& fileList) {
    std::cout << "\nList of Files:\n";
    for (int i = 0; i < fileList.size; i++) {
        std::cout << i + 1 << ". " << fileList.files[i].name << std::endl;
    }
    if (fileList.size == 0) {
        std::cout << "No files available." << std::endl;
    }
}

int getFileSize(const File& file) {
    int size = 0;
    LineNode* current = file.head;
    while (current) {
        size++;
        current = current->next;
    }
    return size;
}

// Fungsi untuk menangani undo
void undo(File& file, Stack& undoStack, Stack& redoStack) {
    if (isStackEmpty(undoStack)) {
        std::cout << "Nothing to undo!" << std::endl;
        return;
    }

    StackNode* action = pop(undoStack);

    if (strcmp(action->action, "add") == 0) {
        // Hapus baris terakhir yang ditambahkan
        int fileSize = getFileSize(file);
        deleteLine(file, fileSize - 1, redoStack); // Posisi baris terakhir
    } else if (strcmp(action->action, "delete") == 0) {
        // Tambahkan kembali baris yang dihapus
        addText(file, action->text, redoStack);
    }

    push(redoStack, action->action, action->text, action->position);
    delete action;
}


// Fungsi untuk menangani redo
void redo(File& file, Stack& undoStack, Stack& redoStack) {
    if (isStackEmpty(redoStack)) {
        std::cout << "Nothing to redo!" << std::endl;
        return;
    }

    StackNode* action = pop(redoStack);

    if (strcmp(action->action, "add") == 0) {
        // Tambahkan kembali teks yang sebelumnya dihapus
        addText(file, action->text, undoStack);
    } else if (strcmp(action->action, "delete") == 0) {
        // Hapus baris yang sebelumnya ditambahkan kembali
        deleteLine(file, action->position, undoStack);
    }

    push(undoStack, action->action, action->text, action->position);
    delete action;
}


// Fungsi untuk menutup dan menghapus file
void deleteFile(FileList& fileList, int fileIndex) {
    if (fileIndex < 0 || fileIndex >= fileList.size) {
        std::cout << "Invalid file index!" << std::endl;
        return;
    }
    for (int i = fileIndex; i < fileList.size - 1; i++) {
        fileList.files[i] = fileList.files[i + 1];
    }
    fileList.size--;
    std::cout << "File deleted." << std::endl;
}

// Fungsi untuk menampilkan menu
void displayMenu() {
    std::cout <<"\n------------------------" ;
    std::cout << "   \nText Editor Menu    " << std::endl;
    std::cout <<"------------------------" << std::endl;
    std::cout << "1. New File" << std::endl;
    std::cout << "2. Add Line" << std::endl;
    std::cout << "3. Search Word" << std::endl;
    std::cout << "4. Undo" << std::endl;
    std::cout << "5. Redo" << std::endl;
    std::cout << "6. Delete File" << std::endl;
    std::cout << "7. Display Text" << std::endl;
    std::cout << "8. Show Files" << std::endl;
    std::cout << "9. Exit" << std::endl;
    std::cout <<"------------------------" << std::endl;
}

// Fungsi untuk menjalankan menu
void runMenu(FileList& fileList) {
    Stack undoStack, redoStack;
    initStack(undoStack);
    initStack(redoStack);

    int choice;
    do {
        displayMenu();
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: { // Membuat file baru
                char filename[MAX_NAME_LENGTH];
                std::cout << "Enter filename: ";
                std::cin >> filename;
                newFile(fileList, filename);
                break;
            }

            case 2: { // Menambah teks
                    int fileIndex;
                    char text[MAX_TEXT_LENGTH];

                    std::cout << "Select file index: ";
                    std::cin >> fileIndex;

                    if (fileIndex <= 0 || fileIndex > fileList.size) {
                        std::cout << "Invalid file index!" << std::endl;
                        break;
                    }

                    std::cout << "Enter text: ";
                    std::cin.ignore(); // Membersihkan buffer
                    std::cin.getline(text, MAX_TEXT_LENGTH);

                    // Panggil addText tanpa meminta posisi
                    addText(fileList.files[fileIndex - 1], text, undoStack);
                    break;

            }

            case 3: { // Mencari kata
                int fileIndex;
                char word[MAX_TEXT_LENGTH];

                std::cout << "Select file index: ";
                std::cin >> fileIndex;

                if (fileIndex <= 0 || fileIndex > fileList.size) {
                    std::cout << "Invalid file index!" << std::endl;
                    break;
                }

                std::cout << "Enter word to search: ";
                std::cin >> word;

                searchWord(fileList.files[fileIndex - 1], word);
                break;
            }

            case 4: { // Undo
                int fileIndex;
                std::cout << "Select file index for undo: ";
                std::cin >> fileIndex;

                if (fileIndex <= 0 || fileIndex > fileList.size) {
                    std::cout << "Invalid file index!" << std::endl;
                    break;
                }

                undo(fileList.files[fileIndex - 1], undoStack, redoStack);
                break;
            }

            case 5: { // Redo
                int fileIndex;
                std::cout << "Select file index for redo: ";
                std::cin >> fileIndex;

                if (fileIndex <= 0 || fileIndex > fileList.size) {
                    std::cout << "Invalid file index!" << std::endl;
                    break;
                }

                redo(fileList.files[fileIndex - 1], undoStack, redoStack);
                break;
            }

            case 6: { // Menghapus file
                int fileIndex;
                std::cout << "Select file index to delete: ";
                std::cin >> fileIndex;

                deleteFile(fileList, fileIndex - 1);
                break;
            }

            case 7: { // Menampilkan teks file
                int fileIndex;
                std::cout << "Select file index to display: ";
                std::cin >> fileIndex;

                if (fileIndex <= 0 || fileIndex > fileList.size) {
                    std::cout << "Invalid file index!" << std::endl;
                    break;
                }

                displayText(fileList.files[fileIndex - 1]);
                break;
            }

            case 8: { // Menampilkan daftar file
                showFiles(fileList);
                break;
            }

            case 9: { // Keluar dari program
                std::cout << "Exiting program. Goodbye!" << std::endl;
                break;
            }

            default:
                std::cout << "Invalid choice! Please try again." << std::endl;
        }
    } while (choice != 9);
}
