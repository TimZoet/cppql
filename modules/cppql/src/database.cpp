#include "cppql/database.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "sqlite3.h"

namespace sql
{
    Database::Database(sqlite3* database) : db(database) { initializeTables(); }

    Database::~Database()
    {
        // TODO: Is this the correct way to close a connection?
        if (db) sqlite3_close(db);
        sqlite3_shutdown();
    }

    ////////////////////////////////////////////////////////////////
    // Static open/create methods.
    ////////////////////////////////////////////////////////////////

    DatabasePtr Database::create(const std::filesystem::path& file)
    {
        if (std::filesystem::exists(file)) throw std::runtime_error("Database file already exists");

        // Try to create a new database.
        sqlite3*   db = nullptr;
        const auto res =
          sqlite3_open_v2(file.string().c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
        if (res != SQLITE_OK) throw std::runtime_error("");

        return std::make_unique<Database>(db);
    }

    DatabasePtr Database::open(const std::filesystem::path& file)
    {
        if (!std::filesystem::exists(file)) throw std::runtime_error("Database file does not exist");

        // Try to open database.
        sqlite3*   db  = nullptr;
        const auto res = sqlite3_open_v2(file.string().c_str(), &db, SQLITE_OPEN_READWRITE, nullptr);
        if (res != SQLITE_OK) throw std::runtime_error("");

        return std::make_unique<Database>(db);
    }

    std::pair<DatabasePtr, bool> Database::openOrCreate(const std::filesystem::path& file)
    {
        const bool created = !exists(file);

        // Try to open or create database.
        sqlite3*   db = nullptr;
        const auto res =
          sqlite3_open_v2(file.string().c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
        if (res != SQLITE_OK) throw std::runtime_error("");

        return std::make_pair(std::make_unique<Database>(db), created);
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    struct sqlite3* Database::get() const noexcept { return db; }

    Table& Database::getTable(const std::string& name)
    {
        const auto it = tables.find(name);
        if (it == tables.end()) throw std::runtime_error(std::format("A table with the name {} does not exist", name));
        return *it->second;
    }

    int64_t Database::getLastInsertRowId() const noexcept { return sqlite3_last_insert_rowid(db); }

    std::string Database::getErrorMessage() const { return std::string(sqlite3_errmsg(db)); }

    ////////////////////////////////////////////////////////////////
    // ...
    ////////////////////////////////////////////////////////////////

    Statement Database::createStatement(std::string code, const bool prepare)
    {
        return Statement(*this, std::move(code), prepare);
    }

    Table& Database::createTable(const std::string& name)
    {
        // Create new table.
        const auto res = tables.try_emplace(name, std::make_unique<Table>(this, name));
        if (!res.second) throw std::runtime_error("");
        return *res.first->second;
    }

    Table& Database::registerTable(const std::string& name)
    {
        if (tables.contains(name)) throw std::runtime_error("Table was already registered");

        // Map to keep track of foreign key constraints.
        std::unordered_map<std::string, std::vector<std::tuple<std::string, std::string, std::string>>> foreignKeys;

        // Create table.
        auto& table = tables.try_emplace(name, std::make_unique<Table>(this, name)).first->second;

        // Read table definition from database and collect foreign keys.
        table->readFromDb(foreignKeys);

        // Resolve foreign keys.
        table->resolveForeignKeys(foreignKeys, tables);

        return *table;
    }

    void Database::dropTable(const std::string& name)
    {
        if (!tables.contains(name)) throw std::runtime_error("Table does not exist");

        // Execute drop table statement.
        const auto stmt = createStatement(std::format("DROP TABLE {};", name), true);
        if (!stmt.step()) throw std::runtime_error("Failed to drop table");

        // Erase table object from map.
        tables.erase(name);
    }

    void Database::vacuum()
    {
        const auto stmt = createStatement("VACUUM", true);
        if (!stmt.step()) throw std::runtime_error("Failed to VACUUM database");
    }

    void Database::initializeTables()
    {
        // Map to keep track of foreign key constraints. These are resolved after all tables and their columns have been created.
        std::unordered_map<std::string, std::vector<std::tuple<std::string, std::string, std::string>>> foreignKeys;

        const auto selectSchema = createStatement("SELECT name, sql FROM sqlite_master WHERE type='table';", true);
        auto       res          = selectSchema.step();
        while (res.code == SQLITE_ROW)
        {
            // Get table name.
            const auto name = selectSchema.column<std::string>(0);

            // Skip sqlite tables.
            if (name.starts_with("sqlite_"))
            {
                res = selectSchema.step();
                continue;
            }

            // Create table.
            auto& table = tables.try_emplace(name, std::make_unique<Table>(this, name)).first->second;

            // Read table definition from database and collect foreign keys.
            table->readFromDb(foreignKeys);

            res = selectSchema.step();
        }

        // Resolve foreign keys.
        for (auto& [name, table] : tables) { table->resolveForeignKeys(foreignKeys, tables); }
    }
}  // namespace sql