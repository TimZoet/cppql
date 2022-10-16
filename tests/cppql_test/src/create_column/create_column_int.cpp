#include "cppql_test/create_column/create_column_int.h"

void CreateColumnInt::operator()()
{
    // Create database and table(s).
    create();
    // Open database and verify contents.
    reopen();
    verify();
}

void CreateColumnInt::create()
{
    // Create table.
    sql::Table* table;
    expectNoThrow([&table, this] { table = &db->createTable("myTable"); });

    // Create columns.
    sql::Column *col1, *col2, *col3, *col4;
    expectNoThrow([&table, &col1] { col1 = &table->createColumn("col1", sql::Column::Type::Int); });
    expectNoThrow([&table, &col2] { col2 = &table->createColumn<int32_t>("col2"); });
    expectNoThrow([&table, &col3] { col3 = &table->createColumn<uint8_t>("col3"); });
    expectNoThrow([&table, &col4] { col4 = &table->createColumn<size_t>("col4"); });

    // Check column types.
    compareEQ(col1->getType(), sql::Column::Type::Int);
    compareEQ(col2->getType(), sql::Column::Type::Int);
    compareEQ(col3->getType(), sql::Column::Type::Int);
    compareEQ(col4->getType(), sql::Column::Type::Int);

    // Commit table.
    expectNoThrow([&table] { table->commit(); });
    compareTrue(table->isCommitted());
}

void CreateColumnInt::verify()
{
    // Try to get table.
    sql::Table* table;
    expectNoThrow([&table, this] { table = &db->getTable("myTable"); });

    // Check column types.
    compareEQ(table->getColumn("col1").getType(), sql::Column::Type::Int);
    compareEQ(table->getColumn("col2").getType(), sql::Column::Type::Int);
    compareEQ(table->getColumn("col3").getType(), sql::Column::Type::Int);
    compareEQ(table->getColumn("col4").getType(), sql::Column::Type::Int);
}
