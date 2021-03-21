#include "cppql_test/typed_table/create_typed_table_text.h"

#include "cppql/ext/typed_table.h"

struct Foo{};

void CreateTypedTableText::operator()()
{
    // Create table with a text column.
    auto& table = db->createTable("myTable");
    table.createColumn("col1", sql::Column::Type::Text);
    table.commit();

    // Create several valid tables.
    expectNoThrow([&table]() {
        sql::ext::TypedTable<std::string> t(table);
        static_cast<void>(t);
    });

    // Create several invalid tables.
    expectThrow([&table]() {
        sql::ext::TypedTable<int32_t> t(table);
        static_cast<void>(t);
    });
    expectThrow([&table]() {
        sql::ext::TypedTable<float> t(table);
        static_cast<void>(t);
    });
    expectThrow([&table]() {
        sql::ext::TypedTable<Foo> t(table);
        static_cast<void>(t);
    });
    expectThrow([&table]() {
        sql::ext::TypedTable<std::vector<std::string>> t(table);
        static_cast<void>(t);
    });
}
