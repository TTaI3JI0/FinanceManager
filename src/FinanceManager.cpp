#include "FinanceManager.hpp"

void FinanceManager::addTransaction(const Transaction &transaction) {
    transactions_.push_back(transaction);
}

const std::vector<Transaction> &FinanceManager::getTransactions() const {
    return transactions_;
}

void FinanceManager::printSummary() const {
    // Заглушка: реализация будет добавлена позже
}

