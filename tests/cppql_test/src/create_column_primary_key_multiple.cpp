#include "cppql_test/create_column_primary_key_multiple.h"

void CreateColumnPrimaryKeyMultiple::operator()()
{
    // Create database and table(s).
    create();
    // Open database and verify contents.
    reopen();
    verify();
}

void CreateColumnPrimaryKeyMultiple::create()
{
    // Create table.
    sql::Table* table;
    expectNoThrow([&table, this] { table = &db->createTable("myTable"); });

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
    // Try to get table.
    sql::Table* table;
    expectNoThrow([&table, this]() { table = &db->getTable("myTable"); });

    // Check column types.
    const auto& idCol1 = table->getColumn("id1");
    compareEQ(idCol1.getType(), sql::Column::Type::Int);
    compareTrue(idCol1.isPrimaryKey());
    compareTrue(idCol1.isNotNull());
    const auto& idCol2 = table->getColumn("id2");
    compareEQ(idCol2.getType(), sql::Column::Type::Int);
    compareTrue(idCol2.isPrimaryKey());
    compareTrue(idCol2.isNotNull());
}