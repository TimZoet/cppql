#include "cppql_test/create_column_real.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql/database.h"

namespace test
{
    struct Foo
    {
    };

    void CreateColumnReal::operator()()
    {
        // Create database and table(s).
        create();
        // Open database and verify contents.
        verify();
    }

    void CreateColumnReal::create()
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
        expectNoThrow([&table, &col1]() { col1 = &table->createColumn("col1", sql::Column::Type::Real); });
        expectNoThrow([&table, &col2]() { col2 = &table->createColumn<float>("col2"); });
        expectNoThrow([&table, &col3]() { col3 = &table->createColumn<double>("col3"); });

        // Check column types.
        compareEQ(col1->getType(), sql::Column::Type::Real);
        compareEQ(col2->getType(), sql::Column::Type::Real);
        compareEQ(col3->getType(), sql::Column::Type::Real);

        // Commit table.
        expectNoThrow([&table]() { table->commit(); });
        compareTrue(table->isCommitted());
    }

    void CreateColumnReal::verify()
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
        compareEQ(cols.find("col1")->second->getType(), sql::Column::Type::Real);
        compareEQ(cols.find("col2")->second->getType(), sql::Column::Type::Real);
        compareEQ(cols.find("col3")->second->getType(), sql::Column::Type::Real);

        // Remove database.
        std::filesystem::remove(dbPath);
    }
}  // namespace test
