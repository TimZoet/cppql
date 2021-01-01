#include "cppql_test/create_column_primary_key.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql/database.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql_test/utils.h"

void CreateColumnPrimaryKey::operator()()
{
    // Create database and table(s).
    create();
    // Open database and verify contents.
    verify();
}

void CreateColumnPrimaryKey::create()
{
    auto db = createDatabase();

    // Create table.
    sql::Table* table;
    expectNoThrow([&db, &table] { table = &db->createTable("myTable"); });

    // Create column.
    sql::Column* idCol;
    expectNoThrow([&table, &idCol]() { idCol = &table->createColumn("id", sql::Column::Type::Int); });
    expectNoThrow([&idCol]() { idCol->setAutoIncrement(true).setPrimaryKey(true).setNotNull(true); });

    // Check column types.
    compareEQ(idCol->getType(), sql::Column::Type::Int);
    compareTrue(idCol->isAutoIncrement());
    compareTrue(idCol->isPrimaryKey());
    compareTrue(idCol->isNotNull());

    // Commit table.
    expectNoThrow([&table]() { table->commit(); });
    compareTrue(table->isCommitted());
}

void CreateColumnPrimaryKey::verify()
{
    auto db = openDatabase();

    // Try to get table.
    sql::Table* table;
    expectNoThrow([&db, &table]() { table = &db->getTable("myTable"); });

    // Check column types.
    const auto& idCol = table->getColumns().find("id")->second;
    compareEQ(idCol->getType(), sql::Column::Type::Int);
    compareTrue(idCol->isAutoIncrement());
    compareTrue(idCol->isPrimaryKey());
    compareTrue(idCol->isNotNull());

    removeDatabase();
}