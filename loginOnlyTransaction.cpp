#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include <cctype>
#include <ctime>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

// Enum untuk pilihan utama
enum PrimaryPrompt { LOGIN = 1, REGISTER, EXIT = 3 };

// Struktur data sederhana
struct Account {
    int id;
    string name;
    string email;
    string address;
    string phone;
    string storeName; // Untuk seller
    bool isBuyer = true;
    bool isSeller = false;
    bool bankLinked = false;
    double balance = 0.0;
};

// Fungsi bantu untuk konversi string ke huruf kecil
string lowerit(string s) {
    for (char &c : s)
        c = (char)tolower((unsigned char)c);
    return s;
}

// Fungsi untuk mencari akun berdasarkan nama dan ID
int findByNameId(const vector<Account>& v, const string& nm, int id) {
    string q = lowerit(nm);
    for (int i = 0; i < (int)v.size(); ++i)
        if (v[i].id == id && lowerit(v[i].name) == q)
            return i;
    return -1;
}

// Fungsi untuk menampilkan ringkasan akun
void printSummary(const Account& a) {
    cout << "ID: " << a.id
         << " | Name: " << a.name
         << " | Role: " << (a.isSeller ? "Seller" : "Buyer")
         << " | Bank: " << (a.bankLinked ? "Linked" : "Not linked");
    if (a.isSeller && !a.storeName.empty())
        cout << " | Store: " << a.storeName;
    cout << "\n";
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

// Fungsi untuk menangani proses login
void handleLogin(vector<Account>& accounts) {
    if (accounts.empty()) {
        cout << "Belum ada akun. Register dulu.\n";
        return;
    }

    string nama;
    int id;
    if (!readLine(nama, "Masukkan Nama: ")) {
        cout << "Nama tidak boleh kosong.\n";
        return;
    }
    if (!readNumber(id, "Masukkan ID: ")) {
        cout << "ID tidak valid.\n";
        return;
    }

    int cur = findByNameId(accounts, nama, id);
    if (cur == -1) {
        cout << "Akun tidak ditemukan.\n";
        return;
    }

    cout << "Berhasil login sebagai " << accounts[cur].name << ".\n";
    printSummary(accounts[cur]);

    // Di sini bisa ditambahkan menu setelah login jika diperlukan
    // Untuk versi ini, kita hanya menampilkan pesan dan kembali
    cout << "Login berhasil. Anda bisa melanjutkan ke fitur lain.\n";
}

// Fungsi untuk menangani proses registrasi
void handleRegister(vector<Account>& accounts, int& nextId) {
    cout << "\n Register \n"
            "1. Seller (butuh bank tertaut)\n"
            "2. Buyer\n"
            "Choose: ";
    int rt;
    if (!readNumber(rt)) return;

    Account a;
    a.id = nextId++;
    a.isBuyer = true;

    if (!readLine(a.name, "Nama lengkap : ") ||
        !readLine(a.email, "Email        : ") ||
        !readLine(a.address, "Alamat       : ") ||
        !readLine(a.phone, "No. telepon  : ") ||
        a.email.find('@') == string::npos) {
        cout << "Data tidak valid. Register gagal.\n";
        nextId--; // Kembalikan ID karena register gagal
        return;
    }

    if (rt == 1) { // Register sebagai Seller
        int link;
        cout << "Tautkan akun bank sekarang? (1=Ya, 2=Tidak): ";
        if (readNumber(link) && link == 1) {
            double d;
            if (readNumber(d, "Initial deposit (Rp): ") && d >= 0) {
                a.bankLinked = true;
                a.balance = d;
                a.isSeller = true;
                cout << "Bank tertaut. Akun Buyer & Seller.\n";
            } else {
                a.isSeller = false;
                cout << "Deposit tidak valid. Akun diset Buyer dulu.\n";
            }
        } else {
            a.isSeller = false;
            cout << "Akun diset Buyer dulu. Bisa upgrade nanti.\n";
        }
    } else { // Register sebagai Buyer
        a.isSeller = false;
    }

    accounts.push_back(a);
    cout << "Register OK. Simpan ID: " << a.id << "\n";
    printSummary(a);
}

int main() {
    vector<Account> accounts; // Simpan data akun di memori
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
            handleLogin(accounts);
        }
        else if (ch == REGISTER) {
            handleRegister(accounts, nextId);
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