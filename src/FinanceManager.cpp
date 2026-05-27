#include "FinanceManager.hpp"

// Простой регистр-независимый compare здесь не нужен; оставляем точное совпадение имён категорий.

static const Category* findCategory(const std::vector<Category> &categories, const std::string &name) {
    for (const auto &c : categories) {
        if (c.name == name) return &c;
    }
    return nullptr;
}

FinanceManager::FinanceManager() {
    // Предопределённые категории (можно расширить позже)
    categories_.push_back(Category("Salary", true));
    categories_.push_back(Category("Gift", true));
    categories_.push_back(Category("Other Income", true));

    categories_.push_back(Category("Food", false));
    categories_.push_back(Category("Transport", false));
    categories_.push_back(Category("Bills", false));
    categories_.push_back(Category("Entertainment", false));
    categories_.push_back(Category("Health", false));
    categories_.push_back(Category("Other Expense", false));
}

void FinanceManager::addTransaction(const Transaction &transaction) {
    if (!categoryExists(transaction.category)) {
        // Категория должна быть заранее создана
        return;
    }
    transactions_.push_back(transaction);
}

const std::vector<Transaction> &FinanceManager::getTransactions() const {
    return transactions_;
}

void FinanceManager::printSummary() const {
    // Заглушка: реализация будет добавлена позже
}

void FinanceManager::addCategory(const Category &category) {
    if (category.name.empty()) return;
    if (categoryExists(category.name)) return;
    categories_.push_back(category);
}

void FinanceManager::printAllCategories() const {
    // Заглушка: печать будет реализована в следующем шаге (пока доступна через main)
}

bool FinanceManager::categoryExists(const std::string &name) const {
    for (const auto &c : categories_) {
        if (c.name == name) return true;
    }
    return false;
}

const std::vector<Category> &FinanceManager::getCategories() const {
    return categories_;
}

double FinanceManager::getTotalBalance() const {
    double balance = 0.0;
    for (const auto &t : transactions_) {
        const auto *cat = findCategory(categories_, t.category);
        if (!cat) continue;
        balance += (cat->isIncome ? t.amount : -t.amount);
    }
    return balance;
}

std::map<std::string, double> FinanceManager::getExpensesByCategory() const {
    std::map<std::string, double> result;
    for (const auto &t : transactions_) {
        const auto *cat = findCategory(categories_, t.category);
        if (!cat) continue;
        if (cat->isIncome) continue;
        result[t.category] += t.amount;
    }
    return result;
}

double FinanceManager::getBalanceForPeriod(const std::string &startDate, const std::string &endDate) const {
    // Так как формат YYYY-MM-DD, лексикографическое сравнение корректно для диапазона дат.
    double balance = 0.0;
    for (const auto &t : transactions_) {
        if (t.date < startDate || t.date > endDate) continue;
        const auto *cat = findCategory(categories_, t.category);
        if (!cat) continue;
        balance += (cat->isIncome ? t.amount : -t.amount);
    }
    return balance;
}

