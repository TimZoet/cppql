#include "cppql_test/typed_table/create_typed_table_blob.h"

#include "cppql-typed/typed_table.h"

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
    expectNoThrow([&table]() {
        sql::TypedTable<std::vector<float>> t(table);
        static_cast<void>(t);
    });
    expectNoThrow([&table]() {
        sql::TypedTable<std::vector<int64_t>> t(table);
        static_cast<void>(t);
    });
    expectNoThrow([&table]() {
        sql::TypedTable<std::vector<Foo>> t(table);
        static_cast<void>(t);
    });
    expectNoThrow([&table]() {
        sql::TypedTable<Foo> t(table);
        static_cast<void>(t);
    });

    // Create several invalid tables.
    expectThrow([&table]() {
        sql::TypedTable<float> t(table);
        static_cast<void>(t);
    });
    expectThrow([&table]() {
        sql::TypedTable<std::string> t(table);
        static_cast<void>(t);
    });
    expectThrow([&table]() {
        sql::TypedTable<uint8_t> t(table);
        static_cast<void>(t);
    });
}
