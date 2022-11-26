#include "cppql_test/typed_table/create_typed_table_blob.h"

#include "cppql/include_all.h"

namespace
{
    struct Foo
    {
    };
}  // namespace

void CreateTypedTableBlob::operator()()
{
    // Create table with a blob column.
    auto& table = db->createTable("myTable");
    table.createColumn("col1", sql::Column::Type::Blob);
    table.commit();

    // Create several valid tables.
    expectNoThrow([&table] {
        const sql::TypedTable<std::vector<float>> t(table);
        static_cast<void>(t);
    });
    expectNoThrow([&table] {
        const sql::TypedTable<std::vector<int64_t>> t(table);
        static_cast<void>(t);
    });
    expectNoThrow([&table] {
        const sql::TypedTable<std::vector<Foo>> t(table);
        static_cast<void>(t);
    });
    expectNoThrow([&table] {
        const sql::TypedTable<Foo> t(table);
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
        const sql::TypedTable<uint8_t> t(table);
        static_cast<void>(t);
    });
}
