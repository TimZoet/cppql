#include "cppql_test/typed_table/create_typed_table_int.h"

#include "cppql-typed/typed_table.h"

namespace
{
    struct Foo
    {
    };
}  // namespace

void CreateTypedTableInt::operator()()
{
    // Create table with an integer column.
    auto& table = db->createTable("myTable");
    table.createColumn("col1", sql::Column::Type::Int);
    table.commit();

    // Create several valid tables.
    expectNoThrow([&table] {
        const sql::TypedTable<int32_t> t(table);
        static_cast<void>(t);
    });
    expectNoThrow([&table] {
        const sql::TypedTable<size_t> t(table);
        static_cast<void>(t);
    });
    expectNoThrow([&table] {
        const sql::TypedTable<uint8_t> t(table);
        static_cast<void>(t);
    });
    expectNoThrow([&table] {
        const sql::TypedTable<bool> t(table);
        static_cast<void>(t);
    });

    // Create several invalid tables.
    expectThrow([&table] {
        const sql::TypedTable<float> t(table);
        static_cast<void>(t);
    });
    expectThrow([&table] {
        const sql::TypedTable<std::string> t(table);
        static_cast<void>(t);
    });
    expectThrow([&table] {
        const sql::TypedTable<Foo*> t(table);
        static_cast<void>(t);
    });
}
