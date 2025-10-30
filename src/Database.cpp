#include "Database.h"
#include <iostream>

Database::Database(const std::string& dbPath) : m_dbPath(dbPath), m_db(nullptr) {}

Database::~Database() { close(); }

bool Database::open() {
    return sqlite3_open(m_dbPath.c_str(), &m_db) == SQLITE_OK;
}

void Database::close() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

bool Database::initialize() {
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS vms (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            path TEXT NOT NULL,
            type TEXT NOT NULL,
            folder TEXT
        );
    )";
    char* errMsg = nullptr;
    if (sqlite3_exec(m_db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "DB Init Error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool Database::addVM(const VMEntry& vm) {
    std::string sql = "INSERT INTO vms(name, path, type, folder) VALUES('" + 
                      vm.name + "','" + vm.path + "','" + vm.type + "','" + vm.folder + "');";
    char* errMsg = nullptr;
    if (sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Add VM Error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool Database::removeVM(int id) {
    std::string sql = "DELETE FROM vms WHERE id=" + std::to_string(id) + ";";
    char* errMsg = nullptr;
    if (sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Remove VM Error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

std::vector<VMEntry> Database::getAllVMs() {
    std::vector<VMEntry> vms;
    const char* sql = "SELECT id, name, path, type, folder FROM vms;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            VMEntry vm;
            vm.id = sqlite3_column_int(stmt, 0);
            vm.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            vm.path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            vm.type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            vm.folder = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            vms.push_back(vm);
        }
    }
    sqlite3_finalize(stmt);
    return vms;
}

std::vector<VMEntry> Database::getVMsByFolder(const std::string& folder) {
    std::vector<VMEntry> vms;
    std::string sql = "SELECT id, name, path, type, folder FROM vms WHERE folder='" + folder + "';";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            VMEntry vm;
            vm.id = sqlite3_column_int(stmt, 0);
            vm.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            vm.path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            vm.type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            vm.folder = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            vms.push_back(vm);
        }
    }
    sqlite3_finalize(stmt);
    return vms;
}
