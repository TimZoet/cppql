#include "cppql_test/create_column_primary_key_multiple.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql/database.h"

namespace test
{
    void CreateColumnPrimaryKeyMultiple::operator()()
    {
        // Create database and table(s).
        create();
        // Open database and verify contents.
        verify();
    }

    void CreateColumnPrimaryKeyMultiple::create()
    {
        // Create database.
        const auto cwd    = std::filesystem::current_path();
        const auto dbPath = cwd / "db.db";
        std::filesystem::remove(dbPath);
        auto db = sql::Database::create(dbPath);

        // Create table.
        sql::Table* table;
        expectNoThrow([&db, &table] { table = &db->createTable("myTable"); });

        // Create columns.
        sql::Column *idCol1, *idCol2;
        expectNoThrow([&table, &idCol1]() { idCol1 = &table->createColumn("id1", sql::Column::Type::Int); });
        expectNoThrow([&idCol1]() { idCol1->setPrimaryKey(true).setNotNull(true); });
        expectNoThrow([&table, &idCol2]() { idCol2 = &table->createColumn("id2", sql::Column::Type::Int); });
        expectNoThrow([&idCol2]() { idCol2->setPrimaryKey(true).setNotNull(true); });

        // Check column types.
        compareEQ(idCol1->getType(), sql::Column::Type::Int);
        compareTrue(idCol1->isPrimaryKey());
        compareTrue(idCol1->isNotNull());
        compareEQ(idCol2->getType(), sql::Column::Type::Int);
        compareTrue(idCol2->isPrimaryKey());
        compareTrue(idCol2->isNotNull());
        
        // Commit table.
        expectNoThrow([&table]() { table->commit(); });
        compareTrue(table->isCommitted());
    }

    void CreateColumnPrimaryKeyMultiple::verify()
    {
        return;
        // Open database.
        const auto cwd    = std::filesystem::current_path();
        const auto dbPath = cwd / "db.db";
        auto       db     = sql::Database::open(dbPath);

        // Try to get table.
        sql::Table* table;
        expectNoThrow([&db, &table]() { table = &db->getTable("myTable"); });

        // Check column types.
        const auto& idCol1 = table->getColumns().find("id1")->second;
        compareEQ(idCol1->getType(), sql::Column::Type::Int);
        compareTrue(idCol1->isPrimaryKey());
        compareTrue(idCol1->isNotNull());
        const auto& idCol2 = table->getColumns().find("id2")->second;
        compareEQ(idCol2->getType(), sql::Column::Type::Int);
        compareTrue(idCol2->isPrimaryKey());
        compareTrue(idCol2->isNotNull());

        // Remove database.
        std::filesystem::remove(dbPath);
    }
}  // namespace test
