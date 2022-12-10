#include "cppql_test/create_column/create_column_unique.h"

void CreateColumnUnique::operator()()
{
    // Create database and table(s).
    create();
    // Open database and verify contents.
    reopen();
    verify();
}

void CreateColumnUnique::create()
{
    // Create table.
    sql::Table* table;
    expectNoThrow([&] { table = &db->createTable("myTable"); });
    expectNoThrow([&] { table->createColumn("col0", sql::Column::Type::Int).unique(sql::ConflictClause::Rollback); });
    expectNoThrow([&] { table->createColumn("col1", sql::Column::Type::Int); });
    expectNoThrow([&] { table->commit(); });
}

void CreateColumnUnique::verify()
{
    // Try to get table.
    sql::Table* table;
    expectNoThrow([&] { table = &db->getTable("myTable"); });

    compareTrue(table->getColumn("col0").isUnique()).info("Column constraints are not yet retrieved on database load.");
    compareFalse(table->getColumn("col1").isUnique());
    compareEQ(sql::ConflictClause::Rollback, table->getColumn("col0").getUniqueConflictClause())
      .info("Column constraints are not yet retrieved on database load.");
    compareEQ(sql::ConflictClause::Abort, table->getColumn("col1").getUniqueConflictClause());
}
