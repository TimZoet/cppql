#include "cppql_test/create_column_int.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql/database.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql_test/utils.h"

struct Foo
{
};

void CreateColumnInt::operator()()
{
    // Create database and table(s).
    create();
    // Open database and verify contents.
    verify();
}

void CreateColumnInt::create()
{
    auto db = createDatabase();

    // Create table.
    sql::Table* table;
    expectNoThrow([&db, &table]() { table = &db->createTable("myTable"); });

    // Create columns.
    sql::Column *col1, *col2, *col3, *col4;
    expectNoThrow([&table, &col1]() { col1 = &table->createColumn("col1", sql::Column::Type::Int); });
    expectNoThrow([&table, &col2]() { col2 = &table->createColumn<int32_t>("col2"); });
    expectNoThrow([&table, &col3]() { col3 = &table->createColumn<uint8_t>("col3"); });
    expectNoThrow([&table, &col4]() { col4 = &table->createColumn<size_t>("col4"); });

    // Check column types.
    compareEQ(col1->getType(), sql::Column::Type::Int);
    compareEQ(col2->getType(), sql::Column::Type::Int);
    compareEQ(col3->getType(), sql::Column::Type::Int);
    compareEQ(col4->getType(), sql::Column::Type::Int);

    // Commit table.
    expectNoThrow([&table]() { table->commit(); });
    compareTrue(table->isCommitted());
}

void CreateColumnInt::verify()
{
    auto db = openDatabase();

    // Try to get table.
    sql::Table* table;
    expectNoThrow([&db, &table]() { table = &db->getTable("myTable"); });

    // Check column types.
    const auto& cols = table->getColumns();
    compareEQ(cols.find("col1")->second->getType(), sql::Column::Type::Int);
    compareEQ(cols.find("col2")->second->getType(), sql::Column::Type::Int);
    compareEQ(cols.find("col3")->second->getType(), sql::Column::Type::Int);
    compareEQ(cols.find("col4")->second->getType(), sql::Column::Type::Int);

    removeDatabase();
}
