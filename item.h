#ifndef ITEM_H
#define ITEM_H

#include <string>

struct Item {
    int id;
    std::string name;
    int quantity;
    double price;

    Item(int i, const std::string& n, int q, double p) : id(i), name(n), quantity(q), price(p) {}
};

#endif