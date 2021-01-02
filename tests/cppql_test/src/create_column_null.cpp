#include "cppql_test/create_column_null.h"

struct Foo
{
};

void CreateColumnNull::operator()()
{
    // Create database and table(s).
    create();
    // Open database and verify contents.
    reopen();
    verify();
}

void CreateColumnNull::create()
{
    // Create table.
    sql::Table* table;
    expectNoThrow([&table, this]() { table = &db->createTable("myTable"); });

    // Create columns.
    sql::Column *col1, *col2, *col3, *col4;
    expectNoThrow([&table, &col1]() { col1 = &table->createColumn("col1", sql::Column::Type::Null); });
    expectNoThrow([&table, &col2]() { col2 = &table->createColumn<std::nullptr_t>("col2"); });

    // Check column types.
    compareEQ(col1->getType(), sql::Column::Type::Null);
    compareEQ(col2->getType(), sql::Column::Type::Null);

    // Commit table.
    expectNoThrow([&table]() { table->commit(); });
    compareTrue(table->isCommitted());
}

void CreateColumnNull::verify()
{
    // Try to get table.
    sql::Table* table;
    expectNoThrow([&table, this]() { table = &db->getTable("myTable"); });

    // Check column types.
    const auto& cols = table->getColumns();
    compareEQ(cols.find("col1")->second->getType(), sql::Column::Type::Null);
    compareEQ(cols.find("col2")->second->getType(), sql::Column::Type::Null);
}
