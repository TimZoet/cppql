#include "cppql_test/create_table.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql/database.h"

namespace test
{
    struct Foo
    {
    };

    void CreateTable::operator()()
    {
        // Create database and table(s).
        create();
        // Open database and verify contents.
        verify();
    }

    void CreateTable::create()
    {
        // Create database.
        // TODO: Put these 4 lines in a utility function for all tests. Same for open.
        const auto cwd    = std::filesystem::current_path();
        const auto dbPath = cwd / "db.db";
        std::filesystem::remove(dbPath);
        auto db = sql::Database::create(dbPath);

        // Create tables. Last create should throw due to duplicate name.
        sql::Table *table1, *table2, *table3, *table4;
        expectNoThrow([&db, &table1]() { table1 = &db->createTable("table1"); });
        expectNoThrow([&db, &table2]() { table2 = &db->createTable("table2"); });
        expectNoThrow([&db, &table3]() { table3 = &db->createTable("table3"); });
        expectNoThrow([&db, &table4]() { table4 = &db->createTable("table4"); });
        expectThrow([&db]() { auto& x = db->createTable("table1"); });

        // Verify names and uncommitted.
        compareEQ(table1->getName(), "table1");
        compareEQ(table2->getName(), "table2");
        compareEQ(table3->getName(), "table3");
        compareFalse(table1->isCommitted());
        compareFalse(table2->isCommitted());
        compareFalse(table3->isCommitted());

        // Create column in all but last table.
        expectNoThrow([&table1]() { table1->createColumn("col", sql::Column::Type::Int); });
        expectNoThrow([&table2]() { table2->createColumn("col", sql::Column::Type::Int); });
        expectNoThrow([&table3]() { table3->createColumn("col", sql::Column::Type::Int); });

        // Creating a column with a duplicate name should throw.
        expectThrow([&table1]() { table1->createColumn("col", sql::Column::Type::Int); });

        // Commit tables. Last commit should throw because there are no columns.
        expectNoThrow([&table1]() { table1->commit(); });
        expectNoThrow([&table2]() { table2->commit(); });
        expectNoThrow([&table3]() { table3->commit(); });
        expectThrow([&table4]() { table4->commit(); });
        compareTrue(table1->isCommitted());
        compareTrue(table2->isCommitted());
        compareTrue(table3->isCommitted());
        compareFalse(table4->isCommitted());

        // Another commit should throw.
        expectThrow([&table1]() { table1->commit(); });
        expectThrow([&table2]() { table2->commit(); });
        expectThrow([&table3]() { table3->commit(); });
    }

    void CreateTable::verify()
    {
        // Open database.
        const auto cwd    = std::filesystem::current_path();
        const auto dbPath = cwd / "db.db";
        auto       db     = sql::Database::open(dbPath);

        // Try to get tables.
        expectNoThrow([&db]() { auto& t = db->getTable("table1"); });
        expectNoThrow([&db]() { auto& t = db->getTable("table2"); });
        expectNoThrow([&db]() { auto& t = db->getTable("table3"); });
        expectThrow([&db]() { auto& t = db->getTable("table4"); });

        // Remove database.
        std::filesystem::remove(dbPath);
    }
}  // namespace test
