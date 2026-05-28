#pragma once

#include <functional>
#include <string>
#include <vector>

struct sqlite3;
struct sqlite3_stmt;

struct SqlBind {
    enum class Type { Text, Int64, Double, Null } type;
    std::string text;
    long long i64{};
    double d{};

    static SqlBind textVal(const std::string &v) { return {Type::Text, v, 0, 0.0}; }
    static SqlBind int64Val(long long v) { return {Type::Int64, "", v, 0.0}; }
    static SqlBind doubleVal(double v) { return {Type::Double, "", 0, v}; }
    static SqlBind nullVal() { return {Type::Null, "", 0, 0.0}; }
};

class DatabaseManager {
public:
    using RowCallback = std::function<void(sqlite3_stmt *)>;

    DatabaseManager();
    ~DatabaseManager();

    bool init();
    void close();

    bool execute(const std::string &sql);
    bool executePrepared(const std::string &sql, const std::vector<SqlBind> &binds = {});
    bool queryPrepared(const std::string &sql,
                       const std::vector<SqlBind> &binds,
                       const RowCallback &onRow) const;

    long long queryInt64(const std::string &sql, const std::string &textParam = "") const;
    bool queryExists(const std::string &sql, const std::string &textParam) const;

    long long getLastInsertId() const;

private:
    bool open();
    bool createTables();
    bool insertInitialCategories();
    static bool bindValue(sqlite3_stmt *stmt, int index, const SqlBind &bind);

private:
    sqlite3 *db_{nullptr};
    std::string dbPath_{"finance.db"};
};
