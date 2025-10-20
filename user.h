#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <ctime>

// Struktur data untuk Kontak
struct Contact {
    int id;
    std::string name;
    std::string phone;
    std::string address;

    Contact() = default;
    Contact(int i, const std::string& n, const std::string& p, const std::string& addr)
        : id(i), name(n), phone(p), address(addr) {}
};

#endif // USER_H