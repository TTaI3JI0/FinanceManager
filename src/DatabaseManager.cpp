#include "DatabaseManager.hpp"

#include <iostream>
#include "sqlite3.h"

DatabaseManager::DatabaseManager() = default;

DatabaseManager::~DatabaseManager() {
    close();
}

bool DatabaseManager::open() {
    if (db_) return true;

    const int rc = sqlite3_open(dbPath_.c_str(), &db_);
    if (rc != SQLITE_OK) {
        std::cerr << "SQLite open failed: " << sqlite3_errmsg(db_) << "\n";
        close();
        return false;
    }

    // Ensure foreign keys are enforced.
    return execute("PRAGMA foreign_keys = ON;");
}

void DatabaseManager::close() {
    if (!db_) return;
    sqlite3_close(db_);
    db_ = nullptr;
}

bool DatabaseManager::execute(const std::string &sql) {
    if (!db_ && !open()) return false;

    char *errMsg = nullptr;
    const int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQLite exec failed: " << (errMsg ? errMsg : "unknown error") << "\n";
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool DatabaseManager::createTables() {
    const char *sql =
        "CREATE TABLE IF NOT EXISTS categories ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    name TEXT UNIQUE NOT NULL,"
        "    is_income INTEGER NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS transactions ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    date TEXT NOT NULL,"
        "    amount REAL NOT NULL,"
        "    category_id INTEGER NOT NULL,"
        "    description TEXT,"
        "    FOREIGN KEY (category_id) REFERENCES categories(id)"
        ");";

    return execute(sql);
}

bool DatabaseManager::insertInitialCategories() {
    // Inserts defaults if they don't exist.
    const char *sql =
        "INSERT OR IGNORE INTO categories(name, is_income) VALUES"
        "('Salary', 1),"
        "('Gift', 1),"
        "('Other Income', 1),"
        "('Food', 0),"
        "('Transport', 0),"
        "('Bills', 0),"
        "('Entertainment', 0),"
        "('Health', 0),"
        "('Other Expense', 0);";

    return execute(sql);
}

bool DatabaseManager::init() {
    if (!open()) return false;
    if (!createTables()) return false;
    if (!insertInitialCategories()) return false;
    return true;
}

long long DatabaseManager::getLastInsertId() const {
    if (!db_) return 0;
    return static_cast<long long>(sqlite3_last_insert_rowid(db_));
}

