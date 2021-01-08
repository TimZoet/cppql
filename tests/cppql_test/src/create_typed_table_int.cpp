#include "cppql_test/create_typed_table_int.h"

#include "cppql/ext/typed_table.h"

struct Foo{};

void CreateTypedTableInt::operator()()
{
    // Create table with an integer column.
    auto& table = db->createTable("myTable");
    table.createColumn("col1", sql::Column::Type::Int);
    table.commit();


    // Create various valid and invalid typed tables.
    expectNoThrow([&table]() {
        sql::ext::TypedTable<int32_t> t(table);
        static_cast<void>(t);
    });

    expectNoThrow([&table]() {
        sql::ext::TypedTable<size_t> t(table);
        static_cast<void>(t);
    });

    expectNoThrow([&table]() {
        sql::ext::TypedTable<uint8_t> t(table);
        static_cast<void>(t);
    });

    expectNoThrow([&table]() {
        sql::ext::TypedTable<bool> t(table);
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
        sql::ext::TypedTable<Foo*> t(table);
        static_cast<void>(t);
    });
}
