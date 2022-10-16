#include "cppql-core/database.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <cassert>
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
        if (db)
        {
            switch (close)
            {
            case Close::Off: break;
            case Close::V1: assert(sqlite3_close(db) == SQLITE_OK); break;
            case Close::V2: assert(sqlite3_close_v2(db) == SQLITE_OK); break;
            }
        }

        if (shutdown == Shutdown::On) assert(sqlite3_shutdown() == SQLITE_OK);
    }

    ////////////////////////////////////////////////////////////////
    // Static open/create methods.
    ////////////////////////////////////////////////////////////////

    DatabasePtr Database::create(const std::filesystem::path& file)
    {
        if (exists(file)) throw std::runtime_error("Database file already exists");

        // Try to create a new database.
        sqlite3* db = nullptr;
        if (const auto res =
              sqlite3_open_v2(file.string().c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
            res != SQLITE_OK)
            throw std::runtime_error("");

        return std::make_unique<Database>(db);
    }

    DatabasePtr Database::open(const std::filesystem::path& file)
    {
        if (!exists(file)) throw std::runtime_error("Database file does not exist");

        // Try to open database.
        sqlite3* db = nullptr;
        if (const auto res = sqlite3_open_v2(file.string().c_str(), &db, SQLITE_OPEN_READWRITE, nullptr);
            res != SQLITE_OK)
            throw std::runtime_error("");

        return std::make_unique<Database>(db);
    }

    std::pair<DatabasePtr, bool> Database::openOrCreate(const std::filesystem::path& file)
    {
        const bool created = !exists(file);

        // Try to open or create database.
        sqlite3* db = nullptr;
        if (const auto res =
              sqlite3_open_v2(file.string().c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
            res != SQLITE_OK)
            throw std::runtime_error("");

        return std::make_pair(std::make_unique<Database>(db), created);
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    struct sqlite3* Database::get() const noexcept { return db; }

    Database::Close Database::getClose() const noexcept { return close; }

    Database::Shutdown Database::getShutdown() const noexcept { return shutdown; }

    Table& Database::getTable(const std::string& name)
    {
        const auto it = tables.find(name);
        if (it == tables.end()) throw std::runtime_error(std::format("A table with the name {} does not exist", name));
        return *it->second;
    }

    int64_t Database::getLastInsertRowId() const noexcept { return sqlite3_last_insert_rowid(db); }

    std::string Database::getErrorMessage() const { return {sqlite3_errmsg(db)}; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void Database::setClose(const Close value) noexcept { close = value; }

    void Database::setShutdown(const Shutdown value) noexcept { shutdown = value; }

    ////////////////////////////////////////////////////////////////
    // ...
    ////////////////////////////////////////////////////////////////

    Statement Database::createStatement(std::string code, const bool prepare)
    {
        return {*this, std::move(code), prepare};
    }

    Table& Database::createTable(const std::string& name)
    {
        // Create new table.
        const auto [it, created] = tables.try_emplace(name, std::make_unique<Table>(this, name));
        if (!created) throw std::runtime_error("");
        return *it->second;
    }

    Table& Database::registerTable(const std::string& name)
    {
        if (tables.contains(name)) throw std::runtime_error("Table was already registered");

        // Map to keep track of foreign key constraints.
        std::unordered_map<std::string, std::vector<std::tuple<std::string, std::string, std::string>>> foreignKeys;

        // Create table.
        const auto& table = tables.try_emplace(name, std::make_unique<Table>(this, name)).first->second;

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
        if (const auto stmt = createStatement(std::format("DROP TABLE {};", name), true); !stmt.step())
            throw std::runtime_error("Failed to drop table");

        // Erase table object from map.
        tables.erase(name);
    }

    void Database::vacuum()
    {
        if (const auto stmt = createStatement("VACUUM", true); !stmt.step())
            throw std::runtime_error("Failed to VACUUM database");
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
            const auto& table = tables.try_emplace(name, std::make_unique<Table>(this, name)).first->second;

            // Read table definition from database and collect foreign keys.
            table->readFromDb(foreignKeys);

            res = selectSchema.step();
        }

        // Resolve foreign keys.
        for (auto& [name, table] : tables) { table->resolveForeignKeys(foreignKeys, tables); }
    }
}  // namespace sql