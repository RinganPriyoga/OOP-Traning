#ifndef BANK_MANAGER_H
#define BANK_MANAGER_H

#include <vector>
#include <map>
#include <algorithm>
#include <ctime>
#include "transaction.h"
#include "data_manager.h"
#include <iostream>

class BankManager {
private:
    std::vector<Transaction> transactions;

public:
    BankManager(const std::vector<Transaction>& trans) : transactions(trans) {}
    
    void listWeeklyTransactions() {
        std::time_t now = std::time(nullptr);
        std::time_t cutoff = now - (7 * 24 * 60 * 60); 
        
        std::cout << "\n[Transactions in Last Week]\n";
        bool found = false;
        for (const auto& trans : transactions) {
            if (trans.timestamp >= cutoff) {
                std::cout << "ID: " << trans.id 
                         << ", Buyer: " << trans.buyerId 
                         << ", Seller: " << trans.sellerId
                         << ", Amount: " << trans.price
                         << ", Date: " << ctime(&trans.timestamp);
                found = true;
            }
        }
        if (!found) {
            std::cout << "No transactions found in the last week.\n";
        }
    }
    
    void listAllCustomers(const std::vector<Account>& accounts) {
        std::cout << "\n[All Bank Customers]\n";
        int count = 0;
        for (const auto& acc : accounts) {
            if (acc.bankLinked) {
                std::cout << ++count << ". ID: " << acc.id 
                         << ", Name: " << acc.name 
                         << ", Balance: " << acc.balance << "\n";
            }
        }
        if (count == 0) {
            std::cout << "No bank customers found.\n";
        }
    }
    
    void listDormantAccounts(const std::vector<Account>& accounts) {
        std::cout << "\n[Dormant Accounts (no transactions in 1 month)]\n";
        std::time_t now = std::time(nullptr);
        std::time_t cutoff = now - (30 * 24 * 60 * 60); 
        
        int count = 0;
        for (const auto& acc : accounts) {
            if (acc.bankLinked) {
                bool active = false;
                for (const auto& trans : transactions) {
                    if ((trans.buyerId == acc.id || trans.sellerId == acc.id) && trans.timestamp >= cutoff) {
                        active = true;
                        break;
                    }
                }
                if (!active) {
                    std::cout << ++count << ". ID: " << acc.id 
                             << ", Name: " << acc.name 
                             << ", Balance: " << acc.balance << "\n";
                }
            }
        }
        if (count == 0) {
            std::cout << "No dormant accounts found.\n";
        }
    }
    
    void listTopUsersToday(int n) {
        std::time_t now = std::time(nullptr);
        std::tm* tm_now = std::localtime(&now);
        tm_now->tm_hour = 0;
        tm_now->tm_min = 0;
        tm_now->tm_sec = 0;
        std::time_t startOfDay = std::mktime(tm_now);
        
        std::map<int, int> userTransCount;
        for (const auto& trans : transactions) {
            if (trans.timestamp >= startOfDay) {
                userTransCount[trans.buyerId]++;
                userTransCount[trans.sellerId]++; 
            }
        }
        
        std::vector<std::pair<int, int>> userVec(userTransCount.begin(), userTransCount.end());
        std::sort(userVec.begin(), userVec.end(), [](const auto& a, const auto& b) {
            return a.second > b.second; 
        });
        
        std::cout << "\n[Top " << n << " Users with Most Transactions Today]\n";
        int count = 0;
        for (const auto& pair : userVec) {
            if (count >= n) break;
            std::cout << count + 1 << ". User ID: " << pair.first << " - " << pair.second << " transactions\n";
            count++;
        }
        if (count == 0) {
            std::cout << "No transactions found today.\n";
        }
    }
};

#endif