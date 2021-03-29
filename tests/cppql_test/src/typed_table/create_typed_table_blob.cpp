#include "cppql_test/typed_table/create_typed_table_blob.h"

#include "cppql/ext/typed_table.h"

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
        sql::ext::TypedTable<std::vector<float>> t(table);
        static_cast<void>(t);
    });
    expectNoThrow([&table]() {
        sql::ext::TypedTable<std::vector<int64_t>> t(table);
        static_cast<void>(t);
    });
    expectNoThrow([&table]() {
        sql::ext::TypedTable<std::vector<Foo>> t(table);
        static_cast<void>(t);
    });
    expectNoThrow([&table]() {
        sql::ext::TypedTable<Foo> t(table);
        static_cast<void>(t);
    });

    // Create several invalid tables.
    expectThrow([&table]() {
        sql::ext::TypedTable<float> t(table);
        static_cast<void>(t);
    });
    expectThrow([&table]() {
        sql::ext::TypedTable<std::string> t(table);
        static_cast<void>(t);
    });
    expectThrow([&table]() {
        sql::ext::TypedTable<uint8_t> t(table);
        static_cast<void>(t);
    });
}
