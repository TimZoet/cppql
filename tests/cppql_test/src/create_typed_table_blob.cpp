#include "cppql_test/create_typed_table_blob.h"

#include "cppql/ext/typed_table.h"

struct Foo{};

void CreateTypedTableBlob::operator()()
{
    // Create table with an integer column.
    auto& table = db->createTable("myTable");
    table.createColumn("col1", sql::Column::Type::Blob);
    table.commit();


    // Create various valid and invalid typed tables.
    expectNoThrow([&table]() {
        sql::ext::TypedTable<void*> t(table);
        static_cast<void>(t);
    });

    expectNoThrow([&table]() {
        sql::ext::TypedTable<float*> t(table);
        static_cast<void>(t);
    });

    expectNoThrow([&table]() {
        sql::ext::TypedTable<uint8_t*> t(table);
        static_cast<void>(t);
    });

    expectNoThrow([&table]() {
        sql::ext::TypedTable<Foo*> t(table);
        static_cast<void>(t);
    });

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
