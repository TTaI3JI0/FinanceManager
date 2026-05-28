#include "FinanceManager.hpp"

#include <iomanip>
#include <iostream>
#include "sqlite3.h"

FinanceManager::FinanceManager(DatabaseManager &db) : db_(db) {}

bool FinanceManager::addTransaction(const std::string &date,
                                    double amount,
                                    const std::string &categoryName,
                                    const std::string &description) {
    const long long categoryId = db_.queryInt64(
        "SELECT id FROM categories WHERE name = ?;",
        categoryName);

    if (categoryId <= 0) {
        return false;
    }

    return db_.executePrepared(
        "INSERT INTO transactions(date, amount, category_id, description) VALUES(?, ?, ?, ?);",
        {
            SqlBind::textVal(date),
            SqlBind::doubleVal(amount),
            SqlBind::int64Val(categoryId),
            SqlBind::textVal(description),
        });
}

bool FinanceManager::removeTransaction(int id) {
    return db_.executePrepared(
        "DELETE FROM transactions WHERE id = ?;",
        {SqlBind::int64Val(id)});
}

namespace {

constexpr const char *kTransactionSelectSql =
    "SELECT t.id, t.date, t.amount, c.name AS category_name, t.description "
    "FROM transactions t "
    "JOIN categories c ON t.category_id = c.id ";

} // namespace

std::vector<Transaction> FinanceManager::queryTransactions(const std::string &sql,
                                                           const std::vector<SqlBind> &binds) const {
    std::vector<Transaction> transactions;

    db_.queryPrepared(
        sql,
        binds,
        [&](sqlite3_stmt *stmt) {
            const int id = static_cast<int>(sqlite3_column_int(stmt, 0));
            const char *date = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            const double amount = sqlite3_column_double(stmt, 2);
            const char *categoryName = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
            const char *description = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));

            transactions.emplace_back(
                id,
                date ? date : "",
                amount,
                categoryName ? categoryName : "",
                description ? description : "");
        });

    return transactions;
}

std::vector<Transaction> FinanceManager::getAllTransactions() {
    return queryTransactions(
        std::string(kTransactionSelectSql) + "ORDER BY t.id;",
        {});
}

std::vector<Transaction> FinanceManager::findTransactionsByDescription(const std::string &substr) const {
    return queryTransactions(
        std::string(kTransactionSelectSql) +
        "WHERE t.description LIKE ? "
        "ORDER BY t.id;",
        {SqlBind::textVal("%" + substr + "%")});
}

std::vector<Transaction> FinanceManager::findTransactionsByCategory(const std::string &categoryName) const {
    return queryTransactions(
        std::string(kTransactionSelectSql) +
        "WHERE c.name = ? "
        "ORDER BY t.id;",
        {SqlBind::textVal(categoryName)});
}

std::vector<Transaction> FinanceManager::findTransactionsByDateRange(const std::string &fromDate,
                                                                     const std::string &toDate) const {
    return queryTransactions(
        std::string(kTransactionSelectSql) +
        "WHERE t.date BETWEEN ? AND ? "
        "ORDER BY t.id;",
        {SqlBind::textVal(fromDate), SqlBind::textVal(toDate)});
}

std::vector<Transaction> FinanceManager::findTransactionsByAmountRange(double minAmount,
                                                                       double maxAmount) const {
    return queryTransactions(
        std::string(kTransactionSelectSql) +
        "WHERE t.amount BETWEEN ? AND ? "
        "ORDER BY t.id;",
        {SqlBind::doubleVal(minAmount), SqlBind::doubleVal(maxAmount)});
}

void FinanceManager::printAllTransactions() {
    const auto transactions = getAllTransactions();

    if (transactions.empty()) {
        std::cout << "No transactions yet.\n";
        return;
    }

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\nTransactions (" << transactions.size() << "):\n";
    for (const auto &t : transactions) {
        std::cout << "#" << t.id
                  << " | " << t.date
                  << " | " << t.amount
                  << " | " << t.category
                  << " | " << t.description
                  << "\n";
    }
}

bool FinanceManager::addCategory(const std::string &name, bool isIncome) {
    if (name.empty() || categoryExists(name)) {
        return false;
    }

    return db_.executePrepared(
        "INSERT INTO categories(name, is_income) VALUES(?, ?);",
        {
            SqlBind::textVal(name),
            SqlBind::int64Val(isIncome ? 1 : 0),
        });
}

void FinanceManager::printAllCategories() {
    std::cout << "\nCategories:\n";

    db_.queryPrepared(
        "SELECT id, name, is_income FROM categories ORDER BY id;",
        {},
        [](sqlite3_stmt *stmt) {
            const int id = sqlite3_column_int(stmt, 0);
            const char *name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            const bool isIncome = sqlite3_column_int(stmt, 2) != 0;

            std::cout << "#" << id
                      << " - " << (name ? name : "")
                      << " [" << (isIncome ? "income" : "expense") << "]\n";
        });
}

bool FinanceManager::categoryExists(const std::string &name) const {
    return db_.queryExists(
        "SELECT 1 FROM categories WHERE name = ? LIMIT 1;",
        name);
}

double FinanceManager::getTotalBalance() const {
    double balance = 0.0;

    db_.queryPrepared(
        "SELECT SUM(CASE WHEN c.is_income = 1 THEN t.amount ELSE -t.amount END) AS balance "
        "FROM transactions t "
        "JOIN categories c ON t.category_id = c.id;",
        {},
        [&](sqlite3_stmt *row) {
            if (sqlite3_column_type(row, 0) != SQLITE_NULL) {
                balance = sqlite3_column_double(row, 0);
            }
        });

    return balance;
}

std::map<std::string, double> FinanceManager::getExpensesByCategory() const {
    std::map<std::string, double> result;

    db_.queryPrepared(
        "SELECT c.name, SUM(t.amount) AS total "
        "FROM transactions t "
        "JOIN categories c ON t.category_id = c.id "
        "WHERE c.is_income = 0 "
        "GROUP BY c.name;",
        {},
        [&](sqlite3_stmt *stmt) {
            const char *name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
            const double total = sqlite3_column_double(stmt, 1);
            if (name) {
                result[name] = total;
            }
        });

    return result;
}

double FinanceManager::getBalanceForPeriod(const std::string &startDate, const std::string &endDate) const {
    double balance = 0.0;

    db_.queryPrepared(
        "SELECT SUM(CASE WHEN c.is_income = 1 THEN t.amount ELSE -t.amount END) AS balance "
        "FROM transactions t "
        "JOIN categories c ON t.category_id = c.id "
        "WHERE t.date BETWEEN ? AND ?;",
        {SqlBind::textVal(startDate), SqlBind::textVal(endDate)},
        [&](sqlite3_stmt *row) {
            if (sqlite3_column_type(row, 0) != SQLITE_NULL) {
                balance = sqlite3_column_double(row, 0);
            }
        });

    return balance;
}

std::map<std::string, double> FinanceManager::getMonthlyExpenses() const {
    std::map<std::string, double> result;

    db_.queryPrepared(
        "SELECT strftime('%Y-%m', t.date) AS month, SUM(t.amount) AS total "
        "FROM transactions t "
        "JOIN categories c ON t.category_id = c.id "
        "WHERE c.is_income = 0 "
        "GROUP BY month "
        "ORDER BY month;",
        {},
        [&](sqlite3_stmt *stmt) {
            const char *month = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
            const double total = sqlite3_column_double(stmt, 1);
            if (month) {
                result[month] = total;
            }
        });

    return result;
}
