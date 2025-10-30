#pragma once
#include <string>
#include <vector>
#include <sqlite3.h>

struct VMEntry {
    int id;
    std::string name;
    std::string path;
    std::string type; // VNC or SPICE
    std::string folder;
};

class Database {
public:
    Database(const std::string& dbPath = "vm_database.db");
    ~Database();

    bool open();
    void close();

    bool initialize(); // Create tables if not exist

    bool addVM(const VMEntry& vm);
    bool removeVM(int id);
    std::vector<VMEntry> getAllVMs();
    std::vector<VMEntry> getVMsByFolder(const std::string& folder);

private:
    std::string m_dbPath;
    sqlite3* m_db;
};
