#include "cppql_test/database/database_create.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/database.h"

void DatabaseCreate::operator()()
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
    expectNoThrow([dbPath, this]() {
        auto [db, created] = sql::Database::openOrCreate(dbPath);
        compareTrue(created);
    });
    compareTrue(std::filesystem::exists(dbPath));
    expectNoThrow([dbPath, this]() {
        auto [db, created] = sql::Database::openOrCreate(dbPath);
        compareFalse(created);
    });
    compareTrue(std::filesystem::exists(dbPath));
    compareTrue(std::filesystem::remove(dbPath));
}
