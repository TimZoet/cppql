#include "cppql_test/select/select_blob.h"

#include "cppql/ext/typed_table.h"
#include "cppql/ext/queries/insert.h"
#include "cppql/ext/queries/select_one.h"

using namespace std::string_literals;

struct Foo
{
    int64_t a;
    float   b;
};

void SelectBlob::operator()()
{
    // Create table.
    sql::Table* t;
    expectNoThrow([&t, this]() {
        t = &db->createTable("myTable");
        t->createColumn("col1", sql::Column::Type::Int);
        t->createColumn("col2", sql::Column::Type::Blob);
        t->createColumn("col3", sql::Column::Type::Blob);
        t->createColumn("col4", sql::Column::Type::Blob);
        t->commit();
    });
    sql::ext::TypedTable<int64_t, std::vector<int32_t>, Foo, std::vector<Foo>> table(*t);

    // Insert several rows.
    auto insert = table.insert();
    expectNoThrow([&insert]() {
        const std::vector<int32_t> a = {0, 1, 2, 3};
        const Foo                  b{.a = 10, .b = 5};
        const std::vector<Foo>     c = {{.a = 20, .b = 30}, {.a = 40, .b = 50}};
        insert(1, sql::toStaticBlob(a), sql::toStaticBlob(b), sql::toStaticBlob(c));
    });
    expectNoThrow([&insert]() {
        const std::vector<int32_t> a = {-10, -11, -12, -13};
        const Foo                  b{.a = -1000, .b = 0.5f};
        const std::vector<Foo>     c = {{.a = 1000000, .b = 4.2f}, {.a = -100, .b = -1.0f}};
        insert(2, sql::toStaticBlob(a), sql::toStaticBlob(b), sql::toStaticBlob(c));
    });

    // Create select.
    auto sel = table.select<1, 2, 3>(table.col<0>() > 0, true);
    std::vector<std::tuple<std::vector<int32_t>, Foo, std::vector<Foo>>> rows(sel.begin(), sel.end());

    // TODO: Use vector comparison methods.
    // Check first row.
    compareEQ(std::get<0>(rows[0])[0], 0);
    compareEQ(std::get<0>(rows[0])[1], 1);
    compareEQ(std::get<0>(rows[0])[2], 2);
    compareEQ(std::get<0>(rows[0])[3], 3);
    compareEQ(std::get<1>(rows[0]).a, 10);
    compareEQ(std::get<1>(rows[0]).b, 5.0f);
    compareEQ(std::get<2>(rows[0])[0].a, 20);
    compareEQ(std::get<2>(rows[0])[0].b, 30.0f);
    compareEQ(std::get<2>(rows[0])[1].a, 40);
    compareEQ(std::get<2>(rows[0])[1].b, 50.0f);

    // Check second row.
    compareEQ(std::get<0>(rows[1])[0], -10);
    compareEQ(std::get<0>(rows[1])[1], -11);
    compareEQ(std::get<0>(rows[1])[2], -12);
    compareEQ(std::get<0>(rows[1])[3], -13);
    compareEQ(std::get<1>(rows[1]).a, -1000);
    compareEQ(std::get<1>(rows[1]).b, 0.5f);
    compareEQ(std::get<2>(rows[1])[0].a, 1000000);
    compareEQ(std::get<2>(rows[1])[0].b, 4.2f);
    compareEQ(std::get<2>(rows[1])[1].a, -100);
    compareEQ(std::get<2>(rows[1])[1].b, -1.0f);
}
