#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include "DatabaseManager.hpp"
#include "FinanceManager.hpp"
#include "Transaction.hpp"
#include "sqlite3.h"

static void printMenu() {
    std::cout << "\n=== Finance Manager ===\n";
    std::cout << "1) List categories\n";
    std::cout << "2) Add category\n";
    std::cout << "3) Add transaction\n";
    std::cout << "4) List transactions\n";
    std::cout << "5) Remove transaction\n";
    std::cout << "6) Reports\n";
    std::cout << "7) Search transactions\n";
    std::cout << "0) Exit\n";
    std::cout << "Choice: ";
}

static void addCategoryFlow(FinanceManager &manager) {
    std::string name;
    int isIncomeInt = 0;

    std::cout << "Category name: ";
    std::getline(std::cin >> std::ws, name);

    std::cout << "Is income? (1 - yes, 0 - no): ";
    std::cin >> isIncomeInt;

    if (manager.addCategory(name, isIncomeInt != 0)) {
        std::cout << "Category added.\n";
    } else {
        std::cout << "Failed to add category (empty name or already exists).\n";
    }
}

static void addTransactionFlow(FinanceManager &manager) {
    std::string date;
    double amount = 0.0;
    std::string category;
    std::string description;

    std::cout << "Date (YYYY-MM-DD): ";
    std::cin >> date;

    std::cout << "Amount (double): ";
    std::cin >> amount;

    std::cout << "Category (exact name): ";
    std::getline(std::cin >> std::ws, category);

    std::cout << "Description: ";
    std::getline(std::cin, description);

    if (!manager.categoryExists(category)) {
        std::cout << "Error: category does not exist. Please add it first.\n";
        return;
    }

    if (manager.addTransaction(date, amount, category, description)) {
        std::cout << "Transaction added.\n";
    } else {
        std::cout << "Failed to add transaction.\n";
    }
}

static void removeTransactionFlow(FinanceManager &manager) {
    int id = 0;
    std::cout << "Transaction ID to remove: ";
    std::cin >> id;

    if (manager.removeTransaction(id)) {
        std::cout << "Transaction removed.\n";
    } else {
        std::cout << "Failed to remove transaction.\n";
    }
}

static void printTransactions(const std::vector<Transaction> &transactions, const std::string &title) {
    if (transactions.empty()) {
        std::cout << "No transactions found.\n";
        return;
    }

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\n" << title << " (" << transactions.size() << "):\n";
    for (const auto &t : transactions) {
        std::cout << "#" << t.id
                  << " | " << t.date
                  << " | " << t.amount
                  << " | " << t.category
                  << " | " << t.description
                  << "\n";
    }
}

static void printSearchMenu() {
    std::cout << "\n--- Search transactions ---\n";
    std::cout << "1) By description (substring)\n";
    std::cout << "2) By category\n";
    std::cout << "3) By date range\n";
    std::cout << "4) By amount range\n";
    std::cout << "0) Back\n";
    std::cout << "Choice: ";
}

static void searchFlow(FinanceManager &manager) {
    while (true) {
        printSearchMenu();

        int choice = -1;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1: {
                std::string substr;
                std::cout << "Description substring: ";
                std::getline(std::cin >> std::ws, substr);
                printTransactions(
                    manager.findTransactionsByDescription(substr),
                    "Search results (description)");
                break;
            }
            case 2: {
                std::string category;
                std::cout << "Category name: ";
                std::getline(std::cin >> std::ws, category);
                printTransactions(
                    manager.findTransactionsByCategory(category),
                    "Search results (category)");
                break;
            }
            case 3: {
                std::string fromDate, toDate;
                std::cout << "From date (YYYY-MM-DD): ";
                std::cin >> fromDate;
                std::cout << "To date (YYYY-MM-DD): ";
                std::cin >> toDate;
                printTransactions(
                    manager.findTransactionsByDateRange(fromDate, toDate),
                    "Search results (date range)");
                break;
            }
            case 4: {
                double minAmount = 0.0;
                double maxAmount = 0.0;
                std::cout << "Min amount: ";
                std::cin >> minAmount;
                std::cout << "Max amount: ";
                std::cin >> maxAmount;
                printTransactions(
                    manager.findTransactionsByAmountRange(minAmount, maxAmount),
                    "Search results (amount range)");
                break;
            }
            case 0:
                return;
            default:
                std::cout << "Unknown option.\n";
                break;
        }
    }
}

static void printReportsMenu() {
    std::cout << "\n--- Reports ---\n";
    std::cout << "1) Total balance\n";
    std::cout << "2) Expenses by category\n";
    std::cout << "3) Balance for period\n";
    std::cout << "0) Back\n";
    std::cout << "Choice: ";
}

static void reportsFlow(FinanceManager &manager) {
    while (true) {
        printReportsMenu();

        int choice = -1;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1: {
                const double balance = manager.getTotalBalance();
                std::cout << std::fixed << std::setprecision(2);
                std::cout << "Total balance: " << balance << "\n";
                break;
            }
            case 2: {
                auto expenses = manager.getExpensesByCategory();
                std::cout << std::fixed << std::setprecision(2);
                if (expenses.empty()) {
                    std::cout << "No expenses yet.\n";
                    break;
                }
                std::cout << "Expenses by category:\n";
                for (const auto &kv : expenses) {
                    std::cout << "- " << kv.first << ": " << kv.second << "\n";
                }
                break;
            }
            case 3: {
                std::string startDate, endDate;
                std::cout << "Start date (YYYY-MM-DD): ";
                std::cin >> startDate;
                std::cout << "End date (YYYY-MM-DD): ";
                std::cin >> endDate;

                const double balance = manager.getBalanceForPeriod(startDate, endDate);
                std::cout << std::fixed << std::setprecision(2);
                std::cout << "Balance for period [" << startDate << " .. " << endDate << "]: " << balance << "\n";
                break;
            }
            case 0:
                return;
            default:
                std::cout << "Unknown option.\n";
                break;
        }
    }
}

int main() {
    std::cout << "SQLite version: " << sqlite3_libversion() << "\n";

    DatabaseManager db;
    if (!db.init()) {
        std::cerr << "Failed to initialize database.\n";
        return 1;
    }

    FinanceManager manager(db);

    while (true) {
        printMenu();

        int choice = -1;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1:
                manager.printAllCategories();
                break;
            case 2:
                addCategoryFlow(manager);
                break;
            case 3:
                addTransactionFlow(manager);
                break;
            case 4:
                manager.printAllTransactions();
                break;
            case 5:
                removeTransactionFlow(manager);
                break;
            case 6:
                reportsFlow(manager);
                break;
            case 7:
                searchFlow(manager);
                break;
            case 0:
                db.close();
                return 0;
            default:
                std::cout << "Unknown menu option.\n";
                break;
        }
    }
}
