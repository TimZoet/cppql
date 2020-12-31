#include "cppql_test/create_column_text.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql/database.h"

namespace test
{
    struct Foo
    {
    };

    void CreateColumnText::operator()()
    {
        // Create database and table(s).
        create();
        // Open database and verify contents.
        verify();
    }

    void CreateColumnText::create()
    {
        // Create database.
        const auto cwd    = std::filesystem::current_path();
        const auto dbPath = cwd / "db.db";
        std::filesystem::remove(dbPath);
        auto db = sql::Database::create(dbPath);

        // Create table.
        sql::Table* table;
        expectNoThrow([&db, &table]() { table = &db->createTable("myTable"); });

        // Create columns.
        sql::Column *col1, *col2, *col3, *col4;
        expectNoThrow([&table, &col1]() { col1 = &table->createColumn("col1", sql::Column::Type::Text); });
        expectNoThrow([&table, &col2]() { col2 = &table->createColumn<std::string>("col2"); });

        // Check column types.
        compareEQ(col1->getType(), sql::Column::Type::Text);
        compareEQ(col2->getType(), sql::Column::Type::Text);

        // Commit table.
        expectNoThrow([&table]() { table->commit(); });
        compareTrue(table->isCommitted());
    }

    void CreateColumnText::verify()
    {
        // Open database.
        const auto cwd    = std::filesystem::current_path();
        const auto dbPath = cwd / "db.db";
        auto       db     = sql::Database::open(dbPath);

        // Try to get table.
        sql::Table* table;
        expectNoThrow([&db, &table]() { table = &db->getTable("myTable"); });

        // Check column types.
        const auto& cols = table->getColumns();
        compareEQ(cols.find("col1")->second->getType(), sql::Column::Type::Text);
        compareEQ(cols.find("col2")->second->getType(), sql::Column::Type::Text);

        // Remove database.
        std::filesystem::remove(dbPath);
    }
}  // namespace test
