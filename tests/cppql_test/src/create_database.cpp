#include "cppql_test/create_database.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql/database.h"

namespace test
{
    void CreateDatabase::operator()()
    {
        const auto cwd    = std::filesystem::current_path();
        const auto dbPath = cwd / ".db";
        std::filesystem::remove(dbPath);

        compareFalse(std::filesystem::exists(dbPath));
        expectThrow([dbPath]() { auto db = sql::Database::open(dbPath); });
        compareFalse(std::filesystem::exists(dbPath));
        expectNoThrow([dbPath]() { auto db = sql::Database::create(dbPath); });
        compareTrue(std::filesystem::exists(dbPath));
        expectNoThrow([dbPath]() { auto db = sql::Database::open(dbPath); });
        compareTrue(std::filesystem::exists(dbPath));
        expectThrow([dbPath]() { auto db = sql::Database::create(dbPath); });
        compareTrue(std::filesystem::exists(dbPath));
        compareTrue(std::filesystem::remove(dbPath));
        compareFalse(std::filesystem::exists(dbPath));
        expectNoThrow([dbPath]() { auto db = sql::Database::openOrCreate(dbPath); });
        compareTrue(std::filesystem::exists(dbPath));
        expectNoThrow([dbPath]() { auto db = sql::Database::openOrCreate(dbPath); });
        compareTrue(std::filesystem::exists(dbPath));
        compareTrue(std::filesystem::remove(dbPath));
    }
}  // namespace test
