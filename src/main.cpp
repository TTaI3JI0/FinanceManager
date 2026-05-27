#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include "FinanceManager.hpp"

static void printMenu() {
    std::cout << "\n=== Finance Manager ===\n";
    std::cout << "1) List categories\n";
    std::cout << "2) Add category\n";
    std::cout << "3) Add transaction (input stub)\n";
    std::cout << "4) Reports\n";
    std::cout << "0) Exit\n";
    std::cout << "Choice: ";
}

static void printCategories(const FinanceManager &manager) {
    const auto &cats = manager.getCategories();
    std::cout << "\nCategories (" << cats.size() << "):\n";
    for (const auto &c : cats) {
        std::cout << "- " << c.name << " [" << (c.isIncome ? "income" : "expense") << "]\n";
    }
}

static void addCategoryFlow(FinanceManager &manager) {
    std::string name;
    int isIncomeInt = 0;

    std::cout << "Category name: ";
    std::getline(std::cin >> std::ws, name);

    std::cout << "Is income? (1 - yes, 0 - no): ";
    std::cin >> isIncomeInt;

    manager.addCategory(Category(name, isIncomeInt != 0));
    std::cout << "Done.\n";
}

static void addTransactionFlow(FinanceManager &manager) {
    int id = 0;
    std::string date;
    double amount = 0.0;
    std::string category;
    std::string description;

    std::cout << "ID (int): ";
    std::cin >> id;

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

    manager.addTransaction(Transaction(id, date, amount, category, description));
    std::cout << "Transaction added.\n";
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
    FinanceManager manager;

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
                printCategories(manager);
                break;
            case 2:
                addCategoryFlow(manager);
                break;
            case 3:
                addTransactionFlow(manager);
                break;
            case 4:
                reportsFlow(manager);
                break;
            case 0:
                return 0;
            default:
                std::cout << "Unknown menu option.\n";
                break;
        }
    }

    return 0;
}

