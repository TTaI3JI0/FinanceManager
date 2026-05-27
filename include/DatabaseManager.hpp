#pragma once

#include <string>

struct sqlite3;

class DatabaseManager {
public:
    DatabaseManager();
    ~DatabaseManager();

    // Creates/open DB, creates tables, inserts initial categories.
    bool init();

    // Closes DB connection (safe to call multiple times).
    void close();

    // Executes an SQL statement without returning rows.
    bool execute(const std::string &sql);

    long long getLastInsertId() const;

private:
    bool open();
    bool createTables();
    bool insertInitialCategories();

private:
    sqlite3 *db_{nullptr};
    std::string dbPath_{"finance.db"};
};

