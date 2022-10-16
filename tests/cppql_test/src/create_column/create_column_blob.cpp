#include "cppql_test/create_column/create_column_blob.h"

namespace
{
    struct Foo
    {
    };
}  // namespace

void CreateColumnBlob::operator()()
{
    // Create database and table(s).
    create();
    // Open database and verify contents.
    reopen();
    verify();
}

void CreateColumnBlob::create()
{
    // Create table.
    sql::Table* table;
    expectNoThrow([&table, this] { table = &db->createTable("myTable"); });

    // Create columns.
    sql::Column *col1, *col2, *col3, *col4;
    expectNoThrow([&table, &col1] { col1 = &table->createColumn("col1", sql::Column::Type::Blob); });
    expectNoThrow([&table, &col2] { col2 = &table->createColumn<float*>("col2"); });
    expectNoThrow([&table, &col3] { col3 = &table->createColumn<void*>("col3"); });
    expectNoThrow([&table, &col4] { col4 = &table->createColumn<Foo*>("col4"); });

    // Check column types.
    compareEQ(col1->getType(), sql::Column::Type::Blob);
    compareEQ(col2->getType(), sql::Column::Type::Blob);
    compareEQ(col3->getType(), sql::Column::Type::Blob);
    compareEQ(col4->getType(), sql::Column::Type::Blob);

    // Commit table.
    expectNoThrow([&table] { table->commit(); });
    compareTrue(table->isCommitted());
}

void CreateColumnBlob::verify()
{
    // Try to get table.
    sql::Table* table;
    expectNoThrow([&table, this] { table = &db->getTable("myTable"); });

    // Check column types.
    compareEQ(table->getColumn("col1").getType(), sql::Column::Type::Blob);
    compareEQ(table->getColumn("col2").getType(), sql::Column::Type::Blob);
    compareEQ(table->getColumn("col3").getType(), sql::Column::Type::Blob);
    compareEQ(table->getColumn("col4").getType(), sql::Column::Type::Blob);
}
