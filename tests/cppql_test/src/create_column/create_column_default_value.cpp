#include "cppql_test/create_column/create_column_default_value.h"

#include "cppql/include_all.h"

void CreateColumnDefaultValue::operator()()
{
    // Create table.
    sql::Table* table;
    expectNoThrow([&table, this] { table = &db->createTable("myTable"); });

    // Create columns with default values.
    sql::Column *col1, *col2, *col3, *col4;
    expectNoThrow([&table, &col1] { col1 = &table->createColumn("col1", sql::Column::Type::Int).defaultValue(10); });
    expectNoThrow([&table, &col2] { col2 = &table->createColumn("col2", sql::Column::Type::Real).defaultValue(4.5f); });
    expectNoThrow(
      [&table, &col3] { col3 = &table->createColumn("col3", sql::Column::Type::Text).defaultValue("'abc'"); });
    expectNoThrow(
      [&table, &col4] { col4 = &table->createColumn("col4", sql::Column::Type::Blob).defaultValue("X'FFAA5500'"); });

    // Commit table.
    expectNoThrow([&table] { table->commit(); });
    compareTrue(table->isCommitted());
}