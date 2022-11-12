#include "cppql_test/select/select_to_vector.h"

#include "cppql-typed/include_all.h"

using namespace std::string_literals;

namespace
{
    struct Foo
    {
        int64_t     c1;
        float       c2;
        std::string c3;
    };

    [[nodiscard]] bool operator==(const Foo& lhs, const Foo& rhs) noexcept
    {
        return lhs.c1 == rhs.c1 && lhs.c2 == rhs.c2 && lhs.c3 == rhs.c3;
    }

    [[nodiscard]] bool operator==(const Foo& lhs, const std::tuple<int64_t, float, std::string>& rhs) noexcept
    {
        return lhs.c1 == std::get<0>(rhs) && lhs.c2 == std::get<1>(rhs) && lhs.c3 == std::get<2>(rhs);
    }
}  // namespace

void SelectToVector::operator()()
{
    // Create table.
    sql::Table* t;
    expectNoThrow([&t, this] {
        t = &db->createTable("myTable");
        t->createColumn("col1", sql::Column::Type::Int);
        t->createColumn("col2", sql::Column::Type::Real);
        t->createColumn("col3", sql::Column::Type::Text);
        t->commit();
    });
    sql::TypedTable<int64_t, float, std::string> table(*t);

    // Insert several rows.
    std::vector<Foo> original;
    auto             insert = table.insert();
    expectNoThrow([&insert] { insert(10, 20.0f, sql::toText("abc")); });
    expectNoThrow([&insert, &original] {
        insert(20, 40.5f, sql::toText("def"));
        original.emplace_back(20, 40.5f, "def");
    });
    expectNoThrow([&insert, &original] {
        insert(30, 80.2f, sql::toText("ghij"));
        original.emplace_back(30, 80.2f, "ghij");
    });
    expectNoThrow([&insert, &original] {
        insert(40, 100.0f, sql::toText("aaaa"));
        original.emplace_back(40, 100.0f, "aaaa");
    });

    // Create select queries.
    auto sel0 = table.select<0, 1, 2>().where(table.col<0>() >= 20)(sql::BindParameters::All);
    auto sel1 = table.select<Foo, 0, 1, 2>().where(table.col<0>() >= 20)(sql::BindParameters::All);

    // Select to vectors.
    const std::vector<std::tuple<int64_t, float, std::string>> vals0(sel0.begin(), sel0.end());
    const std::vector<Foo>                                     vals1(sel1.begin(), sel1.end());

    compareEQ(original, vals0);
    compareEQ(original, vals1);
}
