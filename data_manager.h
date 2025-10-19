#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "transaction.h"
#include "item.h"

struct Account {
    int id;
    std::string name, email, address, phone;
    std::string storeName;      
    bool isBuyer = true;
    bool isSeller = false;
    bool bankLinked = false;
    double balance = 0.0;  
    std::vector<Item> inventory;
    std::vector<int> orders; 
};

class DataManager {
public:
    static void saveAccounts(const std::vector<Account>& accounts) {
        std::ofstream file("accounts.csv");
        if (!file.is_open()) return;
        
        file << "id,name,email,address,phone,storeName,isBuyer,isSeller,bankLinked,balance\n";
        for (const auto& acc : accounts) {
            file << acc.id << "," << acc.name << "," << acc.email << "," << acc.address << "," 
                 << acc.phone << "," << acc.storeName << "," << acc.isBuyer << "," 
                 << acc.isSeller << "," << acc.bankLinked << "," << acc.balance << "\n";
        }
        file.close();
    }
    
    static std::vector<Account> loadAccounts() {
        std::vector<Account> accounts;
        std::ifstream file("accounts.csv");
        if (!file.is_open()) return accounts;
        
        std::string line;
        std::getline(file, line); 
        
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string token;
            Account acc;
            
            std::getline(iss, token, ','); acc.id = std::stoi(token);
            std::getline(iss, acc.name, ',');
            std::getline(iss, acc.email, ',');
            std::getline(iss, acc.address, ',');
            std::getline(iss, acc.phone, ',');
            std::getline(iss, acc.storeName, ',');
            
            std::getline(iss, token, ','); acc.isBuyer = (token == "1");
            std::getline(iss, token, ','); acc.isSeller = (token == "1");
            std::getline(iss, token, ','); acc.bankLinked = (token == "1");
            std::getline(iss, token, ','); acc.balance = std::stod(token);
            
            accounts.push_back(acc);
        }
        file.close();
        return accounts;
    }
    
    static void saveTransactions(const std::vector<Transaction>& transactions) {
        std::ofstream file("transactions.csv");
        if (!file.is_open()) return;
        
        file << "id,buyerId,sellerId,itemId,itemName,quantity,price,status,timestamp\n";
        for (const auto& trans : transactions) {
            file << trans.id << "," << trans.buyerId << "," << trans.sellerId << "," 
                 << trans.itemId << "," << trans.itemName << "," << trans.quantity << "," 
                 << trans.price << "," << trans.status << "," << trans.timestamp << "\n";
        }
        file.close();
    }
    
    static std::vector<Transaction> loadTransactions() {
        std::vector<Transaction> transactions;
        std::ifstream file("transactions.csv");
        if (!file.is_open()) return transactions;
        
        std::string line;
        std::getline(file, line);
        
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string token;
            
            int id, buyerId, sellerId, itemId, quantity, status;
            std::string itemName;
            double price;
            std::time_t timestamp;
            
            std::getline(iss, token, ','); id = std::stoi(token);
            std::getline(iss, token, ','); buyerId = std::stoi(token);
            std::getline(iss, token, ','); sellerId = std::stoi(token);
            std::getline(iss, token, ','); itemId = std::stoi(token);
            std::getline(iss, itemName, ',');
            std::getline(iss, token, ','); quantity = std::stoi(token);
            std::getline(iss, token, ','); price = std::stod(token);
            std::getline(iss, token, ','); status = std::stoi(token);
            std::getline(iss, token, ','); timestamp = static_cast<std::time_t>(std::stol(token));
            
            Transaction trans(id, buyerId, sellerId, itemId, itemName, quantity, price, 
                             static_cast<TransactionStatus>(status));
            trans.timestamp = timestamp;
            transactions.push_back(trans);
        }
        file.close();
        return transactions;
    }
};

#endif