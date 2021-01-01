#include "cppql_test/utils.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <filesystem>

sql::DatabasePtr createDatabase()
{
    const auto cwd    = std::filesystem::current_path();
    const auto dbPath = cwd / "db.db";
    std::filesystem::remove(dbPath);
    return sql::Database::create(dbPath);
}

sql::DatabasePtr openDatabase()
{
    const auto cwd    = std::filesystem::current_path();
    const auto dbPath = cwd / "db.db";
    return sql::Database::open(dbPath);
}

void removeDatabase()
{
    const auto cwd    = std::filesystem::current_path();
    const auto dbPath = cwd / "db.db";
    std::filesystem::remove(dbPath);
}