#include "cppql/core/database.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "sqlite3.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql/core/assert.h"
#include "cppql/error/sqlite_error.h"

namespace sql
{
    Database::Database(sqlite3* database) : db(database) { initializeTables(); }

    Database::~Database()
    {
        int32_t res = SQLITE_OK;

        if (db)
        {

            switch (close)
            {
            case Close::Off: break;
            case Close::V1: res = sqlite3_close(db); break;
            case Close::V2: res = sqlite3_close_v2(db); break;
            }

            CPPQL_ASSERT(res == SQLITE_OK);
        }

        if (shutdown == Shutdown::On)
        {
            res = sqlite3_shutdown();
            CPPQL_ASSERT(res == SQLITE_OK);
        }

        static_cast<void>(res);
    }

    ////////////////////////////////////////////////////////////////
    // Static open/create methods.
    ////////////////////////////////////////////////////////////////

    DatabasePtr Database::create(const std::filesystem::path& file, const int32_t flags)
    {
        if (!(flags & SQLITE_OPEN_MEMORY) && exists(file))
            throw CppqlError(
              std::format("Failed to create database at {}. Database file already exists", file.string()));

        // Try to create a new database.
        sqlite3* db = nullptr;
        if (const auto res =
              sqlite3_open_v2(file.string().c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | flags, nullptr);
            res != SQLITE_OK)
            throw SqliteError(std::format("Failed to create database at {}.", file.string()), res);

        return std::make_unique<Database>(db);
    }

    DatabasePtr Database::open(const std::filesystem::path& file, const int32_t flags)
    {
        if (!(flags & SQLITE_OPEN_MEMORY) && !exists(file))
            throw CppqlError(std::format("Failed to open database at {}. File does not exist", file.string()));

        // Try to open database.
        sqlite3* db = nullptr;
        if (const auto res = sqlite3_open_v2(file.string().c_str(), &db, SQLITE_OPEN_READWRITE | flags, nullptr);
            res != SQLITE_OK)
            throw SqliteError(std::format("Failed to open database at {}.", file.string()), res);

        return std::make_unique<Database>(db);
    }

    std::pair<DatabasePtr, bool> Database::openOrCreate(const std::filesystem::path& file, const int32_t flags)
    {
        const bool created = !exists(file);

        // Try to open or create database.
        sqlite3* db = nullptr;
        if (const auto res =
              sqlite3_open_v2(file.string().c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | flags, nullptr);
            res != SQLITE_OK)
            throw SqliteError(std::format("Failed to open database at {}.", file.string()), res);

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
        if (it == tables.end()) throw CppqlError(std::format("A table with the name {} does not exist", name));
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
        if (!created)
            throw CppqlError(std::format("Could not create table {}. A table with this name already exists.", name));
        return *it->second;
    }

    Table& Database::registerTable(const std::string& name)
    {
        if (tables.contains(name))
            throw CppqlError(
              std::format("Could not register table {}. A table with this name was already registered.", name));

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
        if (!tables.contains(name))
            throw CppqlError(std::format("Could not drop table {}. A table with this name does not exist.", name));

        // Execute drop table statement.
        const auto stmt = createStatement(std::format("DROP TABLE {};", name), true);
        if (const auto res = stmt.step(); !res) throw SqliteError(std::format("Failed to drop table."), res.code);

        // Erase table object from map.
        tables.erase(name);
    }

    Transaction Database::beginTransaction(const Transaction::Type type) { return Transaction(*this, type); }

    void Database::vacuum()
    {
        const auto stmt = createStatement("VACUUM", true);
        if (const auto res = stmt.step(); !res) throw SqliteError(std::format("Failed to VACUUM database."), res.code);
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