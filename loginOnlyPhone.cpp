#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include <cctype> // untuk isdigit
#include <ctime>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "user.h" // Memasukkan definisi struct Contact

using namespace std;

// Enum untuk pilihan utama di menu awal
enum PrimaryPrompt { LOGIN = 1, REGISTER, EXIT = 3 };

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
    return !out.empty(); // Kembalikan true jika string tidak kosong
}

// Fungsi untuk mencari akun berdasarkan nama dan nomor telepon
int findByNamePhone(const vector<Contact>& v, const string& nm, const string& phone) {
    for (int i = 0; i < (int)v.size(); ++i) {
        if (v[i].name == nm && v[i].phone == phone) { // Bandingkan langsung, case-sensitive
            return i;
        }
    }
    return -1; // Kembalikan -1 jika tidak ditemukan
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
    cout << "ID: " << contacts[cur].id
         << " | Name: " << contacts[cur].name
         << " | Phone: " << contacts[cur].phone
         << " | Address: " << contacts[cur].address << "\n";
    // Disini bisa ditambahkan menu setelah login jika diperlukan
    cout << "Login berhasil. Anda bisa melanjutkan ke fitur lain.\n";
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
    cout << "ID: " << c.id
         << " | Name: " << c.name
         << " | Phone: " << c.phone
         << " | Address: " << c.address << "\n";
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
        if (!readNumber(ch)) continue; // Baca pilihan, ulangi jika input bukan angka

        if (ch == LOGIN) {
            handleLogin(contacts); // Panggil fungsi login
        }
        else if (ch == REGISTER) {
            handleRegister(contacts, nextId); // Panggil fungsi register
        }
        else if (ch == EXIT) {
            cout << "Exit Program.\n";
            running = false; // Keluar dari loop utama
        }
        else { // Jika pilihan tidak valid (bukan 1, 2, atau 3)
            cout << "Pilihan Tidak Valid.\n";
        }
    }
    return 0; // Program selesai
}