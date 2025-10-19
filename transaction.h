#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <ctime>

enum TransactionStatus { PAID, COMPLETE, CANCELLED };

struct Transaction {
    int id;
    int buyerId;
    int sellerId;
    int itemId;
    std::string itemName;
    int quantity;
    double price;
    TransactionStatus status;
    std::time_t timestamp;

    Transaction(int tid, int bid, int sid, int iid, const std::string& iname, int qty, double p, TransactionStatus s)
        : id(tid), buyerId(bid), sellerId(sid), itemId(iid), itemName(iname), quantity(qty), price(p), status(s) {
        timestamp = std::time(nullptr);
    }
};

#endif