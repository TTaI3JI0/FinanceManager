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

    return execute("PRAGMA foreign_keys = ON;");
}

void DatabaseManager::close() {
    if (!db_) return;
    sqlite3_close(db_);
    db_ = nullptr;
}

bool DatabaseManager::bindValue(sqlite3_stmt *stmt, int index, const SqlBind &bind) {
    switch (bind.type) {
        case SqlBind::Type::Text:
            return sqlite3_bind_text(stmt, index, bind.text.c_str(), -1, SQLITE_TRANSIENT) == SQLITE_OK;
        case SqlBind::Type::Int64:
            return sqlite3_bind_int64(stmt, index, bind.i64) == SQLITE_OK;
        case SqlBind::Type::Double:
            return sqlite3_bind_double(stmt, index, bind.d) == SQLITE_OK;
        case SqlBind::Type::Null:
            return sqlite3_bind_null(stmt, index) == SQLITE_OK;
    }
    return false;
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

bool DatabaseManager::executePrepared(const std::string &sql, const std::vector<SqlBind> &binds) {
    if (!db_ && !open()) return false;

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "SQLite prepare failed: " << sqlite3_errmsg(db_) << "\n";
        return false;
    }

    for (size_t i = 0; i < binds.size(); ++i) {
        if (!bindValue(stmt, static_cast<int>(i + 1), binds[i])) {
            sqlite3_finalize(stmt);
            return false;
        }
    }

    const int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        std::cerr << "SQLite step failed: " << sqlite3_errmsg(db_) << "\n";
        return false;
    }
    return true;
}

bool DatabaseManager::queryPrepared(const std::string &sql,
                                    const std::vector<SqlBind> &binds,
                                    const RowCallback &onRow) const {
    if (!db_) return false;

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "SQLite prepare failed: " << sqlite3_errmsg(db_) << "\n";
        return false;
    }

    for (size_t i = 0; i < binds.size(); ++i) {
        if (!bindValue(stmt, static_cast<int>(i + 1), binds[i])) {
            sqlite3_finalize(stmt);
            return false;
        }
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        onRow(stmt);
    }

    sqlite3_finalize(stmt);
    return true;
}

long long DatabaseManager::queryInt64(const std::string &sql, const std::string &textParam) const {
    if (!db_) return 0;

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return 0;
    }

    if (!textParam.empty()) {
        sqlite3_bind_text(stmt, 1, textParam.c_str(), -1, SQLITE_TRANSIENT);
    }

    long long result = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        result = sqlite3_column_int64(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return result;
}

bool DatabaseManager::queryExists(const std::string &sql, const std::string &textParam) const {
    return queryInt64(sql, textParam) > 0;
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
