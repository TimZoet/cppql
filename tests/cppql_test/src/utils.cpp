#include "cppql_test/utils.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <filesystem>

namespace utils
{
    DatabaseMember::DatabaseMember()
    {
        const auto cwd    = std::filesystem::current_path();
        const auto dbPath = cwd / "db.db";
        std::filesystem::remove(dbPath);
        db = sql::Database::create(dbPath);
    }

    DatabaseMember::~DatabaseMember() noexcept
    {
        try
        {
            db.reset();
            const auto cwd    = std::filesystem::current_path();
            const auto dbPath = cwd / "db.db";
            std::filesystem::remove(dbPath);
        }
        catch (...)
        {
        }
    }

    void DatabaseMember::reopen()
    {
        db.reset();
        const auto cwd    = std::filesystem::current_path();
        const auto dbPath = cwd / "db.db";
        db                = sql::Database::open(dbPath);
    }

}  // namespace utils