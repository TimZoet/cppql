#include "cppql_test/utils.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <filesystem>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "sqlite3.h"

namespace utils
{
    DatabaseMember::DatabaseMember(const bool inMem) : inMemory(inMem)
    {
        if (inMemory) { db = sql::Database::create("", SQLITE_OPEN_MEMORY | SQLITE_OPEN_NOMUTEX); }
        else
        {
            const auto cwd    = std::filesystem::current_path();
            const auto dbPath = cwd / "db.db";
            if (exists(dbPath)) std::filesystem::remove(dbPath);
            db = sql::Database::create(dbPath);
        }
    }

    DatabaseMember::~DatabaseMember() noexcept
    {
        db->setClose(sql::Database::Close::V2);
        db->setShutdown(sql::Database::Shutdown::Off);
        db.reset();
        if (!inMemory)
        {
            const auto cwd    = std::filesystem::current_path();
            const auto dbPath = cwd / "db.db";
            std::filesystem::remove(dbPath);
        }
    }

    void DatabaseMember::reopen()
    {
        db->setClose(sql::Database::Close::V2);
        db->setShutdown(sql::Database::Shutdown::Off);
        db.reset();
        if (!inMemory)
        {
            const auto cwd    = std::filesystem::current_path();
            const auto dbPath = cwd / "db.db";
            db                = sql::Database::open(dbPath);
        }
        else
            throw std::runtime_error("Cannot reopen in-memory database.");
    }

}  // namespace utils