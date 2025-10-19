#ifndef STORE_MANAGER_H
#define STORE_MANAGER_H

#include <vector>
#include <map>
#include <algorithm>
#include <ctime>
#include "transaction.h"
#include <iostream>

class StoreManager {
private:
    std::vector<Transaction> transactions;

public:
    StoreManager(const std::vector<Transaction>& trans) : transactions(trans) {}
    
    void listLatestKDaysTransactions(int k) {
        std::time_t now = std::time(nullptr);
        std::time_t cutoff = now - (k * 24 * 60 * 60);
        
        std::cout << "\n[Transactions in Last " << k << " Days]\n";
        bool found = false;
        for (const Transaction& trans : transactions) {
            if (trans.timestamp >= cutoff) {
                std::cout << "ID: " << trans.id 
                         << ", Buyer: " << trans.buyerId 
                         << ", Seller: " << trans.sellerId
                         << ", Item: " << trans.itemName 
                         << ", Qty: " << trans.quantity 
                         << ", Price: " << trans.price
                         << ", Date: " << ctime(&trans.timestamp);
                found = true;
            }
        }
        if (!found) {
            std::cout << "No transactions found in the last " << k << " days.\n";
        }
    }
    
    void listPaidNotCompleted() {
        std::cout << "\n[Paid but Not Completed Transactions]\n";
        bool found = false;
        for (const Transaction& trans : transactions) {
            if (trans.status == PAID) {
                std::cout << "ID: " << trans.id 
                         << ", Buyer: " << trans.buyerId 
                         << ", Seller: " << trans.sellerId
                         << ", Item: " << trans.itemName 
                         << ", Qty: " << trans.quantity 
                         << ", Price: " << trans.price
                         << ", Date: " << ctime(&trans.timestamp);
                found = true;
            }
        }
        if (!found) {
            std::cout << "No paid transactions that are not completed.\n";
        }
    }
    
    void listMostMFrequentItems(int m) {
        std::map<std::string, int> itemFreq;
        for (const Transaction& trans : transactions) {
            itemFreq[trans.itemName]++;
        }
        
        std::vector<std::pair<std::string, int>> freqVec(itemFreq.begin(), itemFreq.end());
        std::sort(freqVec.begin(), freqVec.end(), [](const auto& a, const auto& b) {
            return a.second > b.second; 
        });
        
        std::cout << "\n[Top " << m << " Most Frequent Items]\n";
        int count = 0;
        for (const auto& pair : freqVec) {
            if (count >= m) break;
            std::cout << count + 1 << ". " << pair.first << " - " << pair.second << " transactions\n";
            count++;
        }
        if (count == 0) {
            std::cout << "No transactions found.\n";
        }
    }
    
    void listMostActiveBuyers(int m) {
        std::map<int, int> buyerTransCount;
        for (const auto& trans : transactions) {
            buyerTransCount[trans.buyerId]++;
        }
        
        std::vector<std::pair<int, int>> buyerVec(buyerTransCount.begin(), buyerTransCount.end());
        std::sort(buyerVec.begin(), buyerVec.end(), [](const auto& a, const auto& b) {
            return a.second > b.second; 
        });
        
        std::cout << "\n[Top " << m << " Most Active Buyers]\n";
        int count = 0;
        for (const auto& pair : buyerVec) {
            if (count >= m) break;
            std::cout << count + 1 << ". Buyer ID: " << pair.first << " - " << pair.second << " transactions\n";
            count++;
        }
        if (count == 0) {
            std::cout << "No transactions found.\n";
        }
    }
    
    void listMostActiveSellers(int m) {
        std::map<int, int> sellerTransCount;
        for (const auto& trans : transactions) {
            sellerTransCount[trans.sellerId]++;
        }
        
        std::vector<std::pair<int, int>> sellerVec(sellerTransCount.begin(), sellerTransCount.end());
        std::sort(sellerVec.begin(), sellerVec.end(), [](const auto& a, const auto& b) {
            return a.second > b.second; 
        });
        
        std::cout << "\n[Top " << m << " Most Active Sellers]\n";
        int count = 0;
        for (const auto& pair : sellerVec) {
            if (count >= m) break;
            std::cout << count + 1 << ". Seller ID: " << pair.first << " - " << pair.second << " transactions\n";
            count++;
        }
        if (count == 0) {
            std::cout << "No transactions found.\n";
        }
    }
};

#endif