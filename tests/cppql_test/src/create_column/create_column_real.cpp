#include "cppql_test/create_column/create_column_real.h"

void CreateColumnReal::operator()()
{
    // Create database and table(s).
    create();
    // Open database and verify contents.
    reopen();
    verify();
}

void CreateColumnReal::create()
{
    // Create table.
    sql::Table* table;
    expectNoThrow([&table, this] { table = &db->createTable("myTable"); });

    // Create columns.
    sql::Column *col1, *col2, *col3;
    expectNoThrow([&table, &col1] { col1 = &table->createColumn("col1", sql::Column::Type::Real); });
    expectNoThrow([&table, &col2] { col2 = &table->createColumn<float>("col2"); });
    expectNoThrow([&table, &col3] { col3 = &table->createColumn<double>("col3"); });

    // Check column types.
    compareEQ(col1->getType(), sql::Column::Type::Real);
    compareEQ(col2->getType(), sql::Column::Type::Real);
    compareEQ(col3->getType(), sql::Column::Type::Real);

    // Commit table.
    expectNoThrow([&table] { table->commit(); });
    compareTrue(table->isCommitted());
}

void CreateColumnReal::verify()
{
    // Try to get table.
    sql::Table* table;
    expectNoThrow([&table, this] { table = &db->getTable("myTable"); });

    // Check column types.
    compareEQ(table->getColumn("col1").getType(), sql::Column::Type::Real);
    compareEQ(table->getColumn("col2").getType(), sql::Column::Type::Real);
    compareEQ(table->getColumn("col3").getType(), sql::Column::Type::Real);
}
