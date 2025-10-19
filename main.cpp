#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include <cctype>
#include <ctime>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "transaction.h"
#include "item.h"
#include "data_manager.h"
#include "store_manager.h"
#include "bank_manager.h"

using namespace std;

enum PrimaryPrompt { LOGIN = 1, REGISTER, STORE_MENU = 3, BANK_MENU = 4, EXIT = 5 };
enum StoreMenu { SM_LATEST_K = 1, SM_PAID_NOT_COMPLETED = 2, SM_TOP_M_ITEMS = 3, SM_ACTIVE_BUYERS = 4, SM_ACTIVE_SELLERS = 5, SM_BACK = 6 };
enum BankMenu { BM_WEEKLY_TRANS = 1, BM_CUSTOMERS = 2, BM_DORMANT = 3, BM_TOP_USERS = 4, BM_BACK = 5 };


string lowerit(string s) {
    for (char &c : s)
        c = (char)tolower((unsigned char)c);
    return s;
}

int findByNameId(const vector<Account>& v, const string& nm, int id) {
    string q = lowerit(nm);
    for (int i = 0; i < (int)v.size(); ++i)
        if (v[i].id == id && lowerit(v[i].name) == q)
            return i;
    return -1;
}

void printSummary(const Account& a) {
    cout << "ID: " << a.id
         << " | Name: " << a.name
         << " | Role: " << (a.isSeller ? "Seller" : "Buyer")
         << " | Bank: " << (a.bankLinked ? "Linked" : "Not linked");
    if (a.isSeller && !a.storeName.empty())
        cout << " | Store: " << a.storeName;
    cout << "\n";
}

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

bool readLine(string& out, const string& prompt = "") {
    if (!prompt.empty()) cout << prompt;
    getline(cin, out);
    return !out.empty();
}

void addItemToInventory(Account& seller) {
    if (!seller.isSeller) {
        cout << "Hanya seller yang bisa menambah item.\n";
        return;
    }

    int itemId, qty;
    string itemName;
    double price;

    cout << "\n[Add Item to Inventory]\n";
    if (!readNumber(itemId, "Item ID: ")) {
        cout << "ID tidak valid.\n";
        return;
    }

    if (!readLine(itemName, "Nama barang: ")) {
        cout << "Nama barang tidak boleh kosong.\n";
        return;
    }

    if (!readNumber(qty, "Jumlah: ") || qty <= 0) {
        cout << "Jumlah harus > 0.\n";
        return;
    }

    if (!readNumber(price, "Harga per barang: ") || price < 0) {
        cout << "Harga harus >= 0.\n";
        return;
    }

    for (auto& item : seller.inventory) {
        if (item.id == itemId) {
            if (item.name != itemName) {
                cout << "Nama item berbeda untuk ID " << itemId
                     << ". Item lama: '" << item.name << "', Input: '" << itemName << "'.\n";
            }
            if (item.price != price) {
                cout << "Harga berbeda untuk ID " << itemId
                     << ". Harga lama: " << item.price << ", Input: " << price << ".\n";
            }
            item.quantity += qty;
            cout << "Stok item '" << item.name << "' (ID: " << itemId
                 << ") diperbarui. Jumlah baru: " << item.quantity << ".\n";
            return;
        }
    }

    seller.inventory.emplace_back(itemId, itemName, qty, price);
    cout << "Item baru ditambahkan ke inventory " << seller.storeName << ".\n";
}

void viewInventory(const Account& seller) {
    if (!seller.isSeller) {
        cout << "Hanya seller yang bisa melihat inventory.\n";
        return;
    }
    cout << "\nView Inventory " << seller.storeName << "\n";
    if (seller.inventory.empty()) {
        cout << "Inventory kosong.\n";
        return;
    }
    cout << "Daftar Item:\n";
    for (const auto& item : seller.inventory) {
        cout << "ID:" << item.id << ", Nama:" << item.name
             << ", Jumlah:" << item.quantity << ", Harga:" << item.price << "\n";
    }
}

void purchaseItem(vector<Account>& accounts, vector<Transaction>& transactions, int buyerId, int& nextTransactionId) {
    int buyerIndex = -1;
    for (int i = 0; i < (int)accounts.size(); i++) {
        if (accounts[i].id == buyerId) {
            buyerIndex = i;
            break;
        }
    }
    if (buyerIndex == -1) {
        cout << "Buyer tidak ditemukan.\n";
        return;
    }
    Account& buyer = accounts[buyerIndex];

    cout << "\n[Available Items for Purchase]\n";
    bool hasItems = false;
    for (auto& acc : accounts) {
        if (acc.isSeller) {
            bool storeHasItems = false;
            for (const auto& item : acc.inventory) {
                if (item.quantity > 0) {
                    if (!storeHasItems) {
                        cout << "\nStore: " << acc.storeName << " (Seller ID: " << acc.id << ")\n";
                        storeHasItems = true;
                        hasItems = true;
                    }
                    cout << "  ID:" << item.id << ", Nama:" << item.name
                         << ", Qty:" << item.quantity << ", Harga:" << item.price << "\n";
                }
            }
        }
    }

    if (!hasItems) {
        cout << "Tidak ada item tersedia untuk dibeli.\n";
        return;
    }

    int sellerId, itemId, quantity;
    if (!readNumber(sellerId, "\nMasukkan Seller ID: ")) {
        cout << "Seller ID tidak valid.\n";
        return;
    }
    if (!readNumber(itemId, "Masukan ID Item: ")) {
        cout << "Item ID tidak valid.\n";
        return;
    }
    if (!readNumber(quantity, "Masukkan Kuantitas: ") || quantity <= 0) {
        cout << "Quantity harus > 0.\n";
        return;
    }

    int sellerIndex = -1;
    for (int i = 0; i < (int)accounts.size(); i++) {
        if (accounts[i].id == sellerId && accounts[i].isSeller) {
            sellerIndex = i;
            break;
        }
    }
    if (sellerIndex == -1) {
        cout << "Seller tidak ditemukan.\n";
        return;
    }

    int itemIndex = -1;
    auto& inventory = accounts[sellerIndex].inventory;
    for (int i = 0; i < (int)inventory.size(); i++) {
        if (inventory[i].id == itemId) {
            itemIndex = i;
            break;
        }
    }
    if (itemIndex == -1) {
        cout << "Item tidak ditemukan di inventory seller.\n";
        return;
    }

    Item& item = inventory[itemIndex];
    if (item.quantity < quantity) {
        cout << "Stok tidak mencukupi. Tersedia: " << item.quantity << "\n";
        return;
    }

    double totalCost = item.price * quantity;
    if (buyer.balance < totalCost) {
        cout << "Saldo tidak mencukupi. Dibutuhkan: " << totalCost << ", Tersedia: " << buyer.balance << "\n";
        return;
    }

    buyer.balance -= totalCost;
    accounts[sellerIndex].balance += totalCost;
    item.quantity -= quantity;

    Transaction newTransaction(nextTransactionId, buyerId, sellerId, itemId, item.name, quantity, totalCost, PAID);
    transactions.push_back(newTransaction);
    buyer.orders.push_back(nextTransactionId);
    nextTransactionId++;

    cout << "Pembelian berhasil! Total: " << totalCost << "\n";
    cout << "Sisa saldo: " << buyer.balance << "\n";
}

void viewOrders(const Account& buyer, const vector<Transaction>& transactions) {
    cout << "\n[Your Orders]\n";
    if (buyer.orders.empty()) {
        cout << "Tidak ada order yang ditemukan.\n";
        return;
    }

    for (int transId : buyer.orders) {
        for (const auto& trans : transactions) {
            if (trans.id == transId) {
                string statusStr = (trans.status == PAID) ? "DIBAYAR" :
                                  (trans.status == COMPLETE) ? "COMPLETE" : "DIBATALKAN";
                cout << "Trans ID: " << trans.id
                     << ", Item: " << trans.itemName
                     << ", Qty: " << trans.quantity
                     << ", Price: " << trans.price
                     << ", Status: " << statusStr
                     << ", Date: " << ctime(&trans.timestamp);
                break;
            }
        }
    }
}

void checkSpending(const Account& buyer, const vector<Transaction>& transactions, int k) {
    time_t now = time(nullptr);
    time_t cutoff = now - (k * 24 * 60 * 60);

    cout << "\n[Spending in Last " << k << " Days]\n";
    double totalSpending = 0.0;
    int count = 0;

    for (int transId : buyer.orders) {
        for (const auto& trans : transactions) {
            if (trans.id == transId && trans.timestamp >= cutoff && trans.buyerId == buyer.id) {
                cout << "Date: " << ctime(&trans.timestamp)
                     << "  Item: " << trans.itemName
                     << ", Amount: " << trans.price << "\n";
                totalSpending += trans.price;
                count++;
                break;
            }
        }
    }

    cout << "Total pengeluaran dalam " << k << " hari: " << totalSpending << "\n";
    if (count == 0) {
        cout << "Tidak ditemukan pengeluaran dalam " << k << " hari.\n";
    }
}


void handleBanking(Account& a) {
    if (!a.bankLinked) {
        double d;
        cout << "\n[Create Banking]\nInitial deposit (Rp): ";
        if (readNumber(d) && d >= 0) {
            a.bankLinked = true;
            a.balance = d;
            cout << "Akun bank tertaut. Saldo: Rp " << a.balance << "\n";
        } else {
            cout << "Deposit tidak valid.\n";
        }
        return;
    }

    while (true) {
        int choice;
        cout << "\n[Banking]\n1. Balance\n2. Deposit\n3. Withdraw\n4. Back\nChoose: ";
        if (!readNumber(choice)) continue;

        if (choice == 1) {
            cout << "Saldo: Rp " << a.balance << "\n";
        } else if (choice == 2) {
            double d;
            if (readNumber(d, "Jumlah deposit: ") && d > 0) {
                a.balance += d;
                cout << "Saldo Saat Ini: Rp " << a.balance << "\n";
            } else {
                cout << "Harus > 0.\n";
            }
        } else if (choice == 3) {
            double w;
            if (readNumber(w, "Jumlah withdraw: ") && w > 0 && w <= a.balance) {
                a.balance -= w;
                cout << "Saldo Saat Ini: Rp " << a.balance << "\n";
            } else {
                cout << "Tidak valid atau saldo kurang.\n";
            }
        } else if (choice == 4) {
            break;
        } else {
            cout << "Pilihan tidak valid.\n";
        }
    }
}

void handleLoggedInMenu(vector<Account>& accounts, vector<Transaction>& transactions, int cur, int& nextTransactionId) {
    Account& user = accounts[cur];
    bool inLogin = true;

    while (inLogin) {
        cout << "\n Login Menu \n"
             << "Akun: [ID " << user.id << "] " << user.name << "\n";

        vector<string> options = {
            "Check Account Status",
            "Upgrade Account to Seller",
            "Banking (Create/Balance/Deposit/Withdraw)"
        };

        if (user.isSeller) {
            options.insert(options.end(), {
                "Add Item to Inventory",
                "View Inventory",
                "Purchase Item",
                "View My Orders",
                "Check Spending",
                "Logout & Back to Main Menu\n"
                "Choose:" 
            });
        } else {
            options.insert(options.end(), {
                "Purchase Item",
                "View My Orders",
                "Check Spending",
                "Logout & Back to Main Menu\n"
                "Choose:"
            });
        }

        for (size_t i = 0; i < options.size(); ++i) {
            cout << (i + 1) << ". " << options[i] << "\n";
        }

        int choice;
        if (!readNumber(choice) || choice < 1 || choice > (int)options.size()) {
            cout << "Pilihan tidak valid.\n";
            continue;
        }

        switch (choice) {
            case 1: {
                cout << "\n[Status]\n";
                printSummary(user);
                cout << "Email : " << user.email << "\n"
                     << "Addr  : " << user.address << "\n"
                     << "Phone : " << user.phone << "\n";
                break;
            }
            case 2: {
                if (!user.isBuyer) {
                    cout << "Harus Buyer dulu.\n";
                } else if (user.isSeller) {
                    cout << "Sudah Seller.\n";
                } else if (!user.bankLinked) {
                    cout << "Tautkan akun bank dulu.\n";
                } else {
                    string n, e, store, p;
                    if (readLine(n, "Nama lengkap   : ") &&
                        readLine(e, "Email          : ") &&
                        readLine(store, "Nama toko      : ") &&
                        readLine(p, "No. telepon    : ") &&
                        e.find('@') != string::npos) {
                        user.name = n; user.email = e;
                        user.storeName = store; user.phone = p;
                        user.isSeller = true;
                        cout << "Upgrade berhasil.\n";
                        printSummary(user);
                    } else {
                        cout << "Data tidak valid.\n";
                    }
                }
                break;
            }
            case 3:
                handleBanking(user);
                break;
            case 4:
                if (user.isSeller) addItemToInventory(user);
                else purchaseItem(accounts, transactions, user.id, nextTransactionId);
                break;
            case 5:
                if (user.isSeller) viewInventory(user);
                else viewOrders(user, transactions);
                break;
            case 6:
                if (user.isSeller) purchaseItem(accounts, transactions, user.id, nextTransactionId);
                else {
                    int k;
                    if (readNumber(k, "Enter number of days to check spending: ") && k > 0)
                        checkSpending(user, transactions, k);
                    else
                        cout << "Invalid number of days.\n";
                }
                break;
            case 7:
                if (user.isSeller) viewOrders(user, transactions);
                else inLogin = false;
                break;
            case 8:
                if (user.isSeller) {
                    int k;
                    if (readNumber(k, "Enter number of days to check spending: ") && k > 0)
                        checkSpending(user, transactions, k);
                    else
                        cout << "Invalid number of days.\n";
                }
                break;
            case 9:
                if (user.isSeller) inLogin = false;
                break;
            default:
                cout << "Pilihan tidak valid.\n";
        }
    }
}

void handleStoreMenu(const vector<Transaction>& transactions) {
    StoreManager storeMgr(transactions);
    bool inStore = true;
    while (inStore) {
        cout << "\n Store Management \n"
                "1. List transactions in latest k days\n"
                "2. List paid but not completed transactions\n"
                "3. List most m frequent items\n"
                "4. List most active buyers\n"
                "5. List most active sellers\n"
                "6. Back\n"
                "Choose: ";
        int sm;
        if (!readNumber(sm)) continue;

        int val;
        switch (sm) {
            case SM_LATEST_K:
                if (readNumber(val, "Enter number of days: ") && val > 0)
                    storeMgr.listLatestKDaysTransactions(val);
                break;
            case SM_PAID_NOT_COMPLETED:
                storeMgr.listPaidNotCompleted();
                break;
            case SM_TOP_M_ITEMS:
                if (readNumber(val, "Enter number of items: ") && val > 0)
                    storeMgr.listMostMFrequentItems(val);
                break;
            case SM_ACTIVE_BUYERS:
                if (readNumber(val, "Enter number of buyers: ") && val > 0)
                    storeMgr.listMostActiveBuyers(val);
                break;
            case SM_ACTIVE_SELLERS:
                if (readNumber(val, "Enter number of sellers: ") && val > 0)
                    storeMgr.listMostActiveSellers(val);
                break;
            case SM_BACK:
                inStore = false;
                break;
            default:
                cout << "Invalid choice.\n";
        }
    }
}

void handleBankMenu(const vector<Transaction>& transactions, const vector<Account>& accounts) {
    BankManager bankMgr(transactions);
    bool inBank = true;
    while (inBank) {
        cout << "\n Bank Management \n"
                "1. List transactions in a week\n"
                "2. List all customers\n"
                "3. List dormant accounts\n"
                "4. List top users today\n"
                "5. Back\n"
                "Choose: ";
        int bm;
        if (!readNumber(bm)) continue;

        if (bm == BM_WEEKLY_TRANS) {
            bankMgr.listWeeklyTransactions();
        } else if (bm == BM_CUSTOMERS) {
            bankMgr.listAllCustomers(accounts);
        } else if (bm == BM_DORMANT) {
            bankMgr.listDormantAccounts(accounts);
        } else if (bm == BM_TOP_USERS) {
            int n;
            if (readNumber(n, "Enter number of top users: ") && n > 0)
                bankMgr.listTopUsersToday(n);
        } else if (bm == BM_BACK) {
            inBank = false;
        } else {
            cout << "Invalid choice.\n";
        }
    }
}

int main() {
    vector<Account> accounts = DataManager::loadAccounts();
    vector<Transaction> transactions = DataManager::loadTransactions();
    int nextId = 1001, nextTransactionId = 1;

    for (const auto& acc : accounts) nextId = max(nextId, acc.id + 1);
    for (const auto& trans : transactions) nextTransactionId = max(nextTransactionId, trans.id + 1);

    bool running = true;
    while (running) {
        cout << "\n Main Menu \n"
                "1. Login\n"
                "2. Register\n"
                "3. Store Management\n"
                "4. Bank Management\n"
                "5. Exit\n"
                "Choose: ";

        int ch;
        if (!readNumber(ch)) continue;

        if (ch == LOGIN) {
            if (accounts.empty()) {
                cout << "Belum ada akun. Register dulu.\n";
                continue;
            }

            string nama;
            int id;
            if (!readLine(nama, "Masukkan Nama: ")) {
                cout << "Nama tidak boleh kosong.\n";
                continue;
            }
            if (!readNumber(id, "Masukkan ID: ")) {
                cout << "ID tidak valid.\n";
                continue;
            }

            int cur = findByNameId(accounts, nama, id);
            if (cur == -1) {
                cout << "Akun tidak ditemukan.\n";
                continue;
            }

            cout << "Berhasil login sebagai " << accounts[cur].name << ".\n";
            printSummary(accounts[cur]);
            handleLoggedInMenu(accounts, transactions, cur, nextTransactionId);
        }
        else if (ch == REGISTER) {
            cout << "\n Register \n"
                    "1. Seller (butuh bank tertaut)\n"
                    "2. Buyer\n"
                    "Choose: ";
            int rt;
            if (!readNumber(rt)) continue;

            Account a;
            a.id = nextId++;
            a.isBuyer = true;

            if (!readLine(a.name, "Nama lengkap : ") ||
                !readLine(a.email, "Email        : ") ||
                !readLine(a.address, "Alamat       : ") ||
                !readLine(a.phone, "No. telepon  : ") ||
                a.email.find('@') == string::npos) {
                cout << "Data tidak valid. Register gagal.\n";
                nextId--;
                continue;
            }

            if (rt == 1) {
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
            } else {
                a.isSeller = false;
            }

            accounts.push_back(a);
            cout << "Register OK. Simpan ID: " << a.id << "\n";
            printSummary(a);
        }
        else if (ch == STORE_MENU) {
            handleStoreMenu(transactions);
        }
        else if (ch == BANK_MENU) {
            handleBankMenu(transactions, accounts);
        }
        else if (ch == EXIT) {
            DataManager::saveAccounts(accounts);
            DataManager::saveTransactions(transactions);
            cout << "Exit Program.\n";
            running = false;
        }
        else {
            cout << "Pilihan Tidak Valid.\n";
        }
    }
    return 0;
}