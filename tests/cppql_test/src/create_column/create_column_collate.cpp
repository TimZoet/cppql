#include "cppql_test/create_column/create_column_collate.h"

void CreateColumnCollate::operator()()
{
    // Create database and table(s).
    create();
    // Open database and verify contents.
    reopen();
    verify();
}

void CreateColumnCollate::create()
{
    // Create table.
    sql::Table* table;
    expectNoThrow([&] { table = &db->createTable("myTable"); });
    expectNoThrow([&] { table->createColumn("col0", sql::Column::Type::Text).collate("NOCASE"); });
    expectNoThrow([&] { table->createColumn("col1", sql::Column::Type::Text); });
    expectNoThrow([&] { table->commit(); });
}

void CreateColumnCollate::verify()
{
    // Try to get table.
    sql::Table* table;
    expectNoThrow([&] { table = &db->getTable("myTable"); });

    compareTrue(table->getColumn("col0").hasCollate())
      .info("Column constraints are not yet retrieved on database load.");
    compareFalse(table->getColumn("col1").hasCollate());
    compareEQ("NOCASE", table->getColumn("col0").getCollate())
      .info("Column constraints are not yet retrieved on database load.");
    compareEQ("", table->getColumn("col1").getCollate());
}
