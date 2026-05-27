#include "FinanceManager.hpp"

// Простой регистр-независимый compare здесь не нужен; оставляем точное совпадение имён категорий.

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

