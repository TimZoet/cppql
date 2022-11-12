#include "cppql_test/select/select_blob.h"

#include "cppql-typed/include_all.h"

using namespace std::string_literals;

namespace
{
    struct Foo
    {
        int64_t a;
        float   b;
    };

    [[nodiscard]] bool operator==(const Foo& lhs, const Foo& rhs) noexcept { return lhs.a == rhs.a && lhs.b == rhs.b; }

}  // namespace

void SelectBlob::operator()()
{
    // Create table.
    sql::Table* t;
    expectNoThrow([&t, this] {
        t = &db->createTable("myTable");
        t->createColumn("col1", sql::Column::Type::Int);
        t->createColumn("col2", sql::Column::Type::Blob);
        t->createColumn("col3", sql::Column::Type::Blob);
        t->createColumn("col4", sql::Column::Type::Blob);
        t->commit();
    });
    sql::TypedTable<int64_t, std::vector<int32_t>, Foo, std::vector<Foo>> table(*t);

    // Insert several rows.
    auto insert = table.insert();
    expectNoThrow([&insert] {
        const std::vector      a = {0, 1, 2, 3};
        const Foo              b{.a = 10, .b = 5};
        const std::vector<Foo> c = {{.a = 20, .b = 30}, {.a = 40, .b = 50}};
        insert(1, sql::toStaticBlob(a), sql::toStaticBlob(b), sql::toStaticBlob(c));
    });
    expectNoThrow([&insert] {
        const std::vector      a = {-10, -11, -12, -13};
        const Foo              b{.a = -1000, .b = 0.5f};
        const std::vector<Foo> c = {{.a = 1000000, .b = 4.2f}, {.a = -100, .b = -1.0f}};
        insert(2, sql::toStaticBlob(a), sql::toStaticBlob(b), sql::toStaticBlob(c));
    });

    // Create select.
    auto sel = table.select<1, 2, 3>().where(table.col<0>() > 0)(sql::BindParameters::All);
    const std::vector<std::tuple<std::vector<int32_t>, Foo, std::vector<Foo>>> rows(sel.begin(), sel.end());

    // Check first row.
    compareEQ(std::get<0>(rows[0]), std::vector{0, 1, 2, 3});
    compareEQ(std::get<1>(rows[0]), Foo{.a = 10, .b = 5});
    compareEQ(std::get<2>(rows[0]), std::vector{Foo{.a = 20, .b = 30}, Foo{.a = 40, .b = 50}});

    // Check second row.
    compareEQ(std::get<0>(rows[1]), std::vector{-10, -11, -12, -13});
    compareEQ(std::get<1>(rows[1]), Foo{.a = -1000, .b = 0.5f});
    compareEQ(std::get<2>(rows[1]), std::vector{Foo{.a = 1000000, .b = 4.2f}, Foo{.a = -100, .b = -1.0f}});
}
