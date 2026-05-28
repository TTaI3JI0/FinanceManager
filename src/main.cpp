#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include "DatabaseManager.hpp"
#include "FinanceManager.hpp"
#include "Transaction.hpp"
#include "Utils.hpp"
#include "sqlite3.h"

static void printMenu() {
    std::cout << "\n" << Ansi::Bold << Ansi::Magenta << "=== Finance Manager ===" << Ansi::Reset << "\n";
    std::cout << "1) List categories\n";
    std::cout << "2) Add category\n";
    std::cout << "3) Add transaction\n";
    std::cout << "4) List transactions\n";
    std::cout << "5) Remove transaction\n";
    std::cout << "6) Reports\n";
    std::cout << "7) Search transactions\n";
    std::cout << "8) Edit transaction\n";
    std::cout << "0) Exit\n";
    std::cout << Ansi::Cyan << "Choice: " << Ansi::Reset;
}

static void addCategoryFlow(FinanceManager &manager) {
    std::string name;
    int isIncomeInt = 0;

    std::cout << "Category name: ";
    std::getline(std::cin >> std::ws, name);

    std::cout << "Is income? (1 - yes, 0 - no): ";
    std::cin >> isIncomeInt;

    if (manager.addCategory(name, isIncomeInt != 0)) {
        printSuccess("Category added.");
    } else {
        printError("Failed to add category (empty name or already exists).");
    }
}

static void addTransactionFlow(FinanceManager &manager) {
    const std::string date = readValidDate("Date (YYYY-MM-DD): ");

    double amount = 0.0;
    std::cout << "Amount (double): ";
    std::cin >> amount;

    std::string category;
    std::cout << "Category (exact name): ";
    std::getline(std::cin >> std::ws, category);

    std::string description;
    std::cout << "Description: ";
    std::getline(std::cin, description);

    if (!manager.categoryExists(category)) {
        printError("Category does not exist. Please add it first.");
        return;
    }

    if (manager.addTransaction(date, amount, category, description)) {
        printSuccess("Transaction added.");
    } else {
        printError("Failed to add transaction.");
    }
}

static void editTransactionFlow(FinanceManager &manager) {
    int id = 0;
    std::cout << "Transaction ID to edit: ";
    std::cin >> id;

    const std::string newDate = readOptionalValidDate("New date (YYYY-MM-DD, empty = keep): ");

    std::string amountInput;
    std::cout << "New amount (empty = keep): ";
    std::getline(std::cin, amountInput);

    std::string newCategory;
    std::cout << "New category (empty = keep): ";
    std::getline(std::cin, newCategory);

    std::string newDescription;
    std::cout << "New description (empty = keep): ";
    std::getline(std::cin, newDescription);

    bool updateAmount = !amountInput.empty();
    double newAmount = 0.0;
    if (updateAmount) {
        try {
            newAmount = std::stod(amountInput);
        } catch (...) {
            printError("Invalid amount.");
            return;
        }
    }

    if (!newCategory.empty() && !manager.categoryExists(newCategory)) {
        printError("Category does not exist.");
        return;
    }

    if (manager.editTransaction(id, newDate, newAmount, newCategory, newDescription, updateAmount)) {
        printSuccess("Transaction updated.");
    } else {
        printError("Failed to update transaction (ID not found or DB error).");
    }
}

static void removeTransactionFlow(FinanceManager &manager) {
    int id = 0;
    std::cout << "Transaction ID to remove: ";
    std::cin >> id;

    if (manager.removeTransaction(id)) {
        printSuccess("Transaction removed.");
    } else {
        printError("Failed to remove transaction.");
    }
}

static void printTransactions(const std::vector<Transaction> &transactions, const std::string &title) {
    if (transactions.empty()) {
        printInfo("No transactions found.");
        return;
    }

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\n" << Ansi::Bold << title << Ansi::Reset
              << " (" << transactions.size() << "):\n";
    for (const auto &t : transactions) {
        std::cout << Ansi::Blue << "#" << t.id << Ansi::Reset
                  << " | " << t.date
                  << " | " << Ansi::Yellow << t.amount << Ansi::Reset
                  << " | " << t.category
                  << " | " << t.description
                  << "\n";
    }
}

static void printSearchMenu() {
    std::cout << "\n" << Ansi::Bold << "--- Search transactions ---" << Ansi::Reset << "\n";
    std::cout << "1) By description (substring)\n";
    std::cout << "2) By category\n";
    std::cout << "3) By date range\n";
    std::cout << "4) By amount range\n";
    std::cout << "0) Back\n";
    std::cout << Ansi::Cyan << "Choice: " << Ansi::Reset;
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
                const std::string fromDate = readValidDate("From date (YYYY-MM-DD): ");
                const std::string toDate = readValidDate("To date (YYYY-MM-DD): ");

                if (fromDate > toDate) {
                    printError("From date must be <= to date.");
                    break;
                }

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

                if (minAmount > maxAmount) {
                    printError("Min amount must be <= max amount.");
                    break;
                }

                printTransactions(
                    manager.findTransactionsByAmountRange(minAmount, maxAmount),
                    "Search results (amount range)");
                break;
            }
            case 0:
                return;
            default:
                printError("Unknown option.");
                break;
        }
    }
}

static void printReportsMenu() {
    std::cout << "\n" << Ansi::Bold << "--- Reports ---" << Ansi::Reset << "\n";
    std::cout << "1) Total balance\n";
    std::cout << "2) Expenses by category\n";
    std::cout << "3) Balance for period\n";
    std::cout << "4) Monthly expenses (list)\n";
    std::cout << "5) Monthly expense dynamics (chart)\n";
    std::cout << "0) Back\n";
    std::cout << Ansi::Cyan << "Choice: " << Ansi::Reset;
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
                std::cout << Ansi::Green << "Total balance: " << balance << Ansi::Reset << "\n";
                break;
            }
            case 2: {
                auto expenses = manager.getExpensesByCategory();
                std::cout << std::fixed << std::setprecision(2);
                if (expenses.empty()) {
                    printInfo("No expenses yet.");
                    break;
                }
                std::cout << Ansi::Bold << "Expenses by category:\n" << Ansi::Reset;
                for (const auto &kv : expenses) {
                    std::cout << "- " << kv.first << ": " << kv.second << "\n";
                }
                break;
            }
            case 3: {
                const std::string startDate = readValidDate("Start date (YYYY-MM-DD): ");
                const std::string endDate = readValidDate("End date (YYYY-MM-DD): ");

                if (startDate > endDate) {
                    printError("Start date must be <= end date.");
                    break;
                }

                const double balance = manager.getBalanceForPeriod(startDate, endDate);
                std::cout << std::fixed << std::setprecision(2);
                std::cout << Ansi::Green
                          << "Balance for period [" << startDate << " .. " << endDate << "]: "
                          << balance << Ansi::Reset << "\n";
                break;
            }
            case 4: {
                auto monthly = manager.getMonthlyExpenses();
                std::cout << std::fixed << std::setprecision(2);
                if (monthly.empty()) {
                    printInfo("No monthly expenses yet.");
                    break;
                }
                std::cout << Ansi::Bold << "Monthly expenses:\n" << Ansi::Reset;
                for (const auto &kv : monthly) {
                    std::cout << "- " << kv.first << ": " << kv.second << "\n";
                }
                break;
            }
            case 5: {
                auto monthly = manager.getMonthlyExpenses();
                if (monthly.empty()) {
                    printInfo("No monthly expenses yet.");
                    break;
                }
                std::cout << "\n" << Ansi::Bold << Ansi::Magenta
                          << "Monthly expense dynamics / Динамика расходов по месяцам"
                          << Ansi::Reset << "\n\n";
                printHorizontalBarChart(monthly, 40);
                break;
            }
            case 0:
                return;
            default:
                printError("Unknown option.");
                break;
        }
    }
}

int main() {
    enableAnsiSupport();

    printInfo(std::string("SQLite version: ") + sqlite3_libversion());

    DatabaseManager db;
    if (!db.init()) {
        printError("Failed to initialize database.");
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
            case 8:
                editTransactionFlow(manager);
                break;
            case 0:
                db.close();
                printInfo("Goodbye.");
                return 0;
            default:
                printError("Unknown menu option.");
                break;
        }
    }
}
