#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include "FinanceManager.hpp"

static void printMenu() {
    std::cout << "\n=== Финансовый менеджер ===\n";
    std::cout << "1) Показать все категории\n";
    std::cout << "2) Добавить категорию\n";
    std::cout << "3) Добавить транзакцию (заглушка ввода)\n";
    std::cout << "4) Отчёты\n";
    std::cout << "0) Выход\n";
    std::cout << "Выбор: ";
}

static void printCategories(const FinanceManager &manager) {
    const auto &cats = manager.getCategories();
    std::cout << "\nКатегории (" << cats.size() << "):\n";
    for (const auto &c : cats) {
        std::cout << "- " << c.name << " [" << (c.isIncome ? "income" : "expense") << "]\n";
    }
}

static void addCategoryFlow(FinanceManager &manager) {
    std::string name;
    int isIncomeInt = 0;

    std::cout << "Название категории: ";
    std::getline(std::cin >> std::ws, name);

    std::cout << "Это доход? (1 - да, 0 - нет): ";
    std::cin >> isIncomeInt;

    manager.addCategory(Category(name, isIncomeInt != 0));
    std::cout << "Готово.\n";
}

static void addTransactionFlow(FinanceManager &manager) {
    int id = 0;
    std::string date;
    double amount = 0.0;
    std::string category;
    std::string description;

    std::cout << "ID (int): ";
    std::cin >> id;

    std::cout << "Дата (YYYY-MM-DD): ";
    std::cin >> date;

    std::cout << "Сумма (double): ";
    std::cin >> amount;

    std::cout << "Категория (точное имя): ";
    std::getline(std::cin >> std::ws, category);

    std::cout << "Описание: ";
    std::getline(std::cin, description);

    if (!manager.categoryExists(category)) {
        std::cout << "Ошибка: такой категории нет. Сначала добавьте категорию.\n";
        return;
    }

    manager.addTransaction(Transaction(id, date, amount, category, description));
    std::cout << "Транзакция добавлена.\n";
}

static void printReportsMenu() {
    std::cout << "\n--- Отчёты ---\n";
    std::cout << "1) Общий баланс\n";
    std::cout << "2) Расходы по категориям\n";
    std::cout << "3) Баланс за период\n";
    std::cout << "0) Назад\n";
    std::cout << "Выбор: ";
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
                std::cout << "Общий баланс: " << balance << "\n";
                break;
            }
            case 2: {
                auto expenses = manager.getExpensesByCategory();
                std::cout << std::fixed << std::setprecision(2);
                if (expenses.empty()) {
                    std::cout << "Расходов пока нет.\n";
                    break;
                }
                std::cout << "Расходы по категориям:\n";
                for (const auto &kv : expenses) {
                    std::cout << "- " << kv.first << ": " << kv.second << "\n";
                }
                break;
            }
            case 3: {
                std::string startDate, endDate;
                std::cout << "Начальная дата (YYYY-MM-DD): ";
                std::cin >> startDate;
                std::cout << "Конечная дата (YYYY-MM-DD): ";
                std::cin >> endDate;

                const double balance = manager.getBalanceForPeriod(startDate, endDate);
                std::cout << std::fixed << std::setprecision(2);
                std::cout << "Баланс за период [" << startDate << " .. " << endDate << "]: " << balance << "\n";
                break;
            }
            case 0:
                return;
            default:
                std::cout << "Неизвестный пункт.\n";
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
                std::cout << "Неизвестный пункт меню.\n";
                break;
        }
    }

    return 0;
}

