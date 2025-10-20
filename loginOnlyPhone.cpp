#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include <cctype>
#include <ctime>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "user.h" // Asumsikan file user.h yang telah dimodifikasi

using namespace std;

// Enum untuk pilihan utama
enum PrimaryPrompt { LOGIN = 1, REGISTER, EXIT = 3 };
enum LoggedInMenu { VIEW_PROFILE = 1, LOGOUT = 2 };

// Fungsi bantu untuk konversi string ke huruf kecil
string lowerit(string s) {
    for (char &c : s)
        c = (char)tolower((unsigned char)c);
    return s;
}

// Fungsi untuk mencari akun berdasarkan nama dan nomor telepon
int findByNamePhone(const vector<Contact>& v, const string& nm, const string& phone) {
    string q_name = lowerit(nm);
    string q_phone = phone; // Nomor telepon biasanya tidak diubah ke huruf kecil
    for (int i = 0; i < (int)v.size(); ++i) {
        if (lowerit(v[i].name) == q_name && v[i].phone == q_phone) {
            return i;
        }
    }
    return -1;
}

// Fungsi untuk mencari akun berdasarkan ID
int findById(const vector<Contact>& v, int id) {
    for (int i = 0; i < (int)v.size(); ++i) {
        if (v[i].id == id) {
            return i;
        }
    }
    return -1;
}

// Fungsi untuk menampilkan ringkasan kontak
void printContactSummary(const Contact& c) {
    cout << "ID: " << c.id
         << " | Name: " << c.name
         << " | Phone: " << c.phone
         << " | Address: " << c.address << "\n";
}

// Template untuk membaca angka dari input
template<typename T>
bool readNumber(T& out, const string& prompt = "") {
    if (!prompt.empty()) cout << prompt;
    if (!(cin >> out)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return true;
}

// Fungsi untuk membaca satu baris string dari input
bool readLine(string& out, const string& prompt = "") {
    if (!prompt.empty()) cout << prompt;
    getline(cin, out);
    return !out.empty();
}

// Fungsi untuk menangani menu setelah login
void handleLoggedInMenu(vector<Contact>& contacts, int cur) {
    Contact& user = contacts[cur];
    bool inLogin = true;

    while (inLogin) {
        cout << "\n --- Menu Login --- \n"
             << "Akun: [ID " << user.id << "] " << user.name << "\n"
             << "1. View Profile\n"
             << "2. Logout & Back to Main Menu\n"
             << "Choose: ";

        int choice;
        if (!readNumber(choice) || choice < 1 || choice > 2) {
            cout << "Pilihan tidak valid.\n";
            continue;
        }

        switch (choice) {
            case VIEW_PROFILE:
                cout << "\n--- Profile ---\n";
                printContactSummary(user);
                break;
            case LOGOUT:
                inLogin = false;
                cout << "Logout berhasil.\n";
                break;
            default:
                cout << "Pilihan tidak valid.\n";
        }
    }
}

// Fungsi untuk menangani proses login
void handleLogin(vector<Contact>& contacts) {
    if (contacts.empty()) {
        cout << "Belum ada kontak. Register dulu.\n";
        return;
    }

    string nama, phone;
    if (!readLine(nama, "Masukkan Nama: ")) {
        cout << "Nama tidak boleh kosong.\n";
        return;
    }
    if (!readLine(phone, "Masukkan Nomor Telepon: ")) {
        cout << "Nomor telepon tidak boleh kosong.\n";
        return;
    }

    int cur = findByNamePhone(contacts, nama, phone);
    if (cur == -1) {
        cout << "Kontak tidak ditemukan.\n";
        return;
    }

    cout << "Berhasil login sebagai " << contacts[cur].name << ".\n";
    printContactSummary(contacts[cur]);

    // Masuk ke menu setelah login
    handleLoggedInMenu(contacts, cur);
}

// Fungsi untuk menangani proses registrasi
void handleRegister(vector<Contact>& contacts, int& nextId) {
    Contact c;
    c.id = nextId++; // ID otomatis

    if (!readLine(c.name, "Nama lengkap : ") ||
        !readLine(c.phone, "No. telepon  : ") ||
        !readLine(c.address, "Alamat       : ")) {
        cout << "Data tidak valid. Register gagal.\n";
        nextId--; // Kembalikan ID karena register gagal
        return;
    }

    // Validasi nomor telepon sederhana (misalnya harus angka)
    for (char ch : c.phone) {
        if (!isdigit(ch)) {
            cout << "Nomor telepon harus berisi angka.\n";
            nextId--;
            return;
        }
    }

    contacts.push_back(c);
    cout << "Register OK. Simpan ID: " << c.id << "\n";
    printContactSummary(c);
}

int main() {
    vector<Contact> contacts; // Simpan data kontak di memori
    int nextId = 1001; // ID otomatis dimulai dari 1001

    bool running = true;
    while (running) {
        cout << "\n Main Menu \n"
                "1. Login\n"
                "2. Register\n"
                "3. Exit\n"
                "Choose: ";

        int ch;
        if (!readNumber(ch)) continue;

        if (ch == LOGIN) {
            handleLogin(contacts);
        }
        else if (ch == REGISTER) {
            handleRegister(contacts, nextId);
        }
        else if (ch == EXIT) {
            cout << "Exit Program.\n";
            running = false;
        }
        else {
            cout << "Pilihan Tidak Valid.\n";
        }
    }
    return 0;
}