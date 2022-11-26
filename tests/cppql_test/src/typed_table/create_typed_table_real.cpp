#include "cppql_test/typed_table/create_typed_table_real.h"

#include "cppql/include_all.h"

namespace
{
    struct Foo
    {
    };
}  // namespace

void CreateTypedTableReal::operator()()
{
    // Create table with a float column.
    auto& table = db->createTable("myTable");
    table.createColumn("col1", sql::Column::Type::Real);
    table.commit();

    // Create several valid tables.
    expectNoThrow([&table] {
        const sql::TypedTable<float> t(table);
        static_cast<void>(t);
    });
    expectNoThrow([&table] {
        const sql::TypedTable<double> t(table);
        static_cast<void>(t);
    });

    // Create several invalid tables.
    expectThrow([&table] {
        const sql::TypedTable<int32_t> t(table);
        static_cast<void>(t);
    });
    expectThrow([&table] {
        const sql::TypedTable<std::string> t(table);
        static_cast<void>(t);
    });
    expectThrow([&table] {
        const sql::TypedTable<Foo> t(table);
        static_cast<void>(t);
    });
    expectThrow([&table] {
        const sql::TypedTable<std::vector<float>> t(table);
        static_cast<void>(t);
    });
}
