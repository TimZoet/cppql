#include "cppql_test/typed_table/create_typed_table.h"

#include "cppql/include_all.h"

namespace
{
    struct Foo
    {
    };
    struct Bar
    {
    };
}  // namespace

void CreateTypedTable::operator()()
{
    // Create complex table.
    auto& table = db->createTable("myTable");
    table.createColumn("col1", sql::Column::Type::Int);
    table.createColumn("col2", sql::Column::Type::Real);
    table.createColumn("col3", sql::Column::Type::Text);
    table.createColumn("col4", sql::Column::Type::Blob);
    table.createColumn("col5", sql::Column::Type::Int);
    table.createColumn("col6", sql::Column::Type::Real);
    table.createColumn("col7", sql::Column::Type::Text);
    table.createColumn("col8", sql::Column::Type::Blob);
    table.commit();

    // Create several valid tables.
    expectNoThrow([&table] {
        const sql::TypedTable<int64_t, double, std::string, std::vector<float>, uint16_t, float, std::string, Foo> t(
          table);
        static_cast<void>(t);
    });
    expectNoThrow([&table] {
        const sql::TypedTable<int32_t, float, std::string, Bar, int8_t, float, std::string, std::vector<Foo>> t(table);
        static_cast<void>(t);
    });
    expectNoThrow([&table] {
        const sql::TypedTable<int8_t, double, std::string, std::vector<float>, uint64_t, double, std::string, Bar> t(table);
        static_cast<void>(t);
    });
}
