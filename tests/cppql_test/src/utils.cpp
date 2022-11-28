#include "cppql_test/utils.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <filesystem>

namespace utils
{
    DatabaseMember::DatabaseMember()
    {
        // TODO: Is it possible to create an in-memory database?
        // Would allow running tests in parallel without issues.
        const auto cwd    = std::filesystem::current_path();
        const auto dbPath = cwd / "db.db";
        if (exists(dbPath)) std::filesystem::remove(dbPath);
        db = sql::Database::create(dbPath);
    }

    DatabaseMember::~DatabaseMember() noexcept
    {
        db.reset();
        const auto cwd    = std::filesystem::current_path();
        const auto dbPath = cwd / "db.db";
        std::filesystem::remove(dbPath);
    }

    void DatabaseMember::reopen()
    {
        db.reset();
        const auto cwd    = std::filesystem::current_path();
        const auto dbPath = cwd / "db.db";
        db                = sql::Database::open(dbPath);
    }

}  // namespace utils