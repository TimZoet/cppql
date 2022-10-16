#include "cppql_test/table/create_table.h"

void CreateTable::operator()()
{
    // Create database and table(s).
    create();
    // Open database and verify contents.
    reopen();
    verify();
}

void CreateTable::create()
{
    // Create tables. Last create should throw due to duplicate name.
    sql::Table *table1, *table2, *table3, *table4;
    expectNoThrow([this, &table1] { table1 = &db->createTable("table1"); });
    expectNoThrow([this, &table2] { table2 = &db->createTable("table2"); });
    expectNoThrow([this, &table3] { table3 = &db->createTable("table3"); });
    expectNoThrow([this, &table4] { table4 = &db->createTable("table4"); });
    expectThrow([this] { static_cast<void>(db->createTable("table1")); });

    // Verify names and uncommitted.
    compareEQ(table1->getName(), "table1");
    compareEQ(table2->getName(), "table2");
    compareEQ(table3->getName(), "table3");
    compareFalse(table1->isCommitted());
    compareFalse(table2->isCommitted());
    compareFalse(table3->isCommitted());

    // Create column in all but last table.
    expectNoThrow([&table1] { table1->createColumn("col", sql::Column::Type::Int); });
    expectNoThrow([&table2] { table2->createColumn("col", sql::Column::Type::Int); });
    expectNoThrow([&table3] { table3->createColumn("col", sql::Column::Type::Int); });

    // Creating a column with a duplicate name should throw.
    expectThrow([&table1] { table1->createColumn("col", sql::Column::Type::Int); });

    // Commit tables. Last commit should throw because there are no columns.
    expectNoThrow([&table1] { table1->commit(); });
    expectNoThrow([&table2] { table2->commit(); });
    expectNoThrow([&table3] { table3->commit(); });
    expectThrow([&table4] { table4->commit(); });
    compareTrue(table1->isCommitted());
    compareTrue(table2->isCommitted());
    compareTrue(table3->isCommitted());
    compareFalse(table4->isCommitted());

    // Another commit should throw.
    expectThrow([&table1] { table1->commit(); });
    expectThrow([&table2] { table2->commit(); });
    expectThrow([&table3] { table3->commit(); });
}

void CreateTable::verify()
{
    // Try to get tables.
    expectNoThrow([this] { static_cast<void>(db->getTable("table1")); });
    expectNoThrow([this] { static_cast<void>(db->getTable("table2")); });
    expectNoThrow([this] { static_cast<void>(db->getTable("table3")); });
    expectThrow([this] { static_cast<void>(db->getTable("table4")); });
}
