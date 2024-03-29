#include "cppql_test/create_column/create_column_primary_key.h"

void CreateColumnPrimaryKey::operator()()
{
    // Create database and table(s).
    create();
    // Open database and verify contents.
    reopen();
    verify();
}

void CreateColumnPrimaryKey::create()
{
    // Create table.
    sql::Table* table;
    expectNoThrow([&table, this] { table = &db->createTable("myTable"); });

    // Create column.
    sql::Column* idCol;
    expectNoThrow([&table, &idCol] { idCol = &table->createColumn("id", sql::Column::Type::Int); });
    expectNoThrow([&idCol] { idCol->primaryKey(true).notNull(); });

    // Check column types.
    compareEQ(idCol->getType(), sql::Column::Type::Int);
    compareTrue(idCol->isAutoIncrement());
    compareTrue(idCol->isPrimaryKey());
    compareTrue(idCol->isNotNull());

    // Commit table.
    expectNoThrow([&table] { table->commit(); });
    compareTrue(table->isCommitted());
}

void CreateColumnPrimaryKey::verify()
{
    // Try to get table.
    sql::Table* table;
    expectNoThrow([&table, this] { table = &db->getTable("myTable"); });

    // Check column types.
    const auto& idCol = table->getColumn("id");
    compareEQ(idCol.getType(), sql::Column::Type::Int);
    compareTrue(idCol.isAutoIncrement());
    compareTrue(idCol.isPrimaryKey());
    compareTrue(idCol.isNotNull());
}