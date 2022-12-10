#include "cppql_test/create_column/create_column_check.h"

void CreateColumnCheck::operator()()
{
    // Create database and table(s).
    create();
    // Open database and verify contents.
    reopen();
    verify();
}

void CreateColumnCheck::create()
{
    // Create table.
    sql::Table* table;
    expectNoThrow([&] { table = &db->createTable("myTable"); });
    expectNoThrow([&] { table->createColumn("col0", sql::Column::Type::Int).check("col0 + col1 > 10"); });
    expectNoThrow([&] { table->createColumn("col1", sql::Column::Type::Int).check("col0 * col1 > 10"); });
    expectNoThrow([&] { table->createColumn("col2", sql::Column::Type::Int); });
    expectNoThrow([&] { table->commit(); });
}

void CreateColumnCheck::verify()
{
    // Try to get table.
    sql::Table* table;
    expectNoThrow([&] { table = &db->getTable("myTable"); });

    compareTrue(table->getColumn("col0").hasCheck()).info("Column constraints are not yet retrieved on database load.");
    compareTrue(table->getColumn("col1").hasCheck()).info("Column constraints are not yet retrieved on database load.");
    compareFalse(table->getColumn("col2").hasCheck());
    compareEQ("col0 + col1 > 10", table->getColumn("col0").getCheck())
      .info("Column constraints are not yet retrieved on database load.");
    compareEQ("col0 * col1 > 10", table->getColumn("col1").getCheck())
      .info("Column constraints are not yet retrieved on database load.");
    compareEQ("", table->getColumn("col2").getCheck());
}
