#include "cppql_test/insert/insert_default.h"

#include "cppql-typed/typed_table.h"
#include "cppql-typed/queries/insert.h"
#include "cppql-typed/queries/select_one.h"

using namespace std::string_literals;

void InsertDefault::operator()()
{
    // Create table.
    sql::Table* table;
    expectNoThrow([&table, this]() { table = &db->createTable("myTable"); });

    // Create columns with default values.
    sql::Column *col1, *col2, *col3, *col4;
    expectNoThrow(
      [&table, &col1]() { col1 = &table->createColumn("col1", sql::Column::Type::Int).setDefaultValue(10); });
    expectNoThrow(
      [&table, &col2]() { col2 = &table->createColumn("col2", sql::Column::Type::Real).setDefaultValue(4.5f); });
    expectNoThrow(
      [&table, &col3]() { col3 = &table->createColumn("col3", sql::Column::Type::Text).setDefaultValue("'abc'"); });
    expectNoThrow([&table, &col4]() {
        col4 = &table->createColumn("col4", sql::Column::Type::Blob).setDefaultValue("X'FFAA5500'");
    });

    // Commit table.
    expectNoThrow([&table]() { table->commit(); });
    compareTrue(table->isCommitted());

    sql::TypedTable<int32_t, float, std::string, std::vector<uint8_t>> typedTable(*table);

    // Insert all default values.
    auto insert = typedTable.insert<>();
    insert();

    // Get row.
    auto                       row   = typedTable.selectOne(typedTable.col<0>() > 0, true)(false);
    const std::vector<uint8_t> bytes = {255, 170, 85, 0};

    // Compare.
    compareEQ(static_cast<int32_t>(10), std::get<0>(row));
    compareEQ(4.5f, std::get<1>(row));
    compareEQ(std::string("abc"), std::get<2>(row));
    compareEQ(bytes, std::get<3>(row));
}
