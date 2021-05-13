#include "cppql_test/select/select_to_vector.h"

#include "cppql/ext/typed_table.h"
#include "cppql/ext/queries/insert.h"
#include "cppql/ext/queries/select.h"

using namespace std::string_literals;

namespace
{
    struct Foo
    {
        int64_t     c1;
        float       c2;
        std::string c3;
    };
}  // namespace

void SelectToVector::operator()()
{
    // Create table.
    sql::Table* t;
    expectNoThrow([&t, this]() {
        t = &db->createTable("myTable");
        t->createColumn("col1", sql::Column::Type::Int);
        t->createColumn("col2", sql::Column::Type::Real);
        t->createColumn("col3", sql::Column::Type::Text);
        t->commit();
    });
    sql::ext::TypedTable<int64_t, float, std::string> table(*t);

    // Insert several rows.
    auto insert = table.insert();
    expectNoThrow([&insert]() { insert(10, 20.0f, "abc"s); });
    expectNoThrow([&insert]() { insert(20, 40.5f, "def"s); });
    expectNoThrow([&insert]() { insert(30, 80.2f, "ghij"s); });
    expectNoThrow([&insert]() { insert(40, 100.0f, "aaaa"s); });

    // Create select queries.
    auto sel0 = table.select<0, 1, 2>(table.col<0>() >= 20, true);
    auto sel1 = table.select<Foo, 0, 1, 2>(table.col<0>() >= 20, true);

    // Select to vectors.
    const std::vector<std::tuple<int64_t, float, std::string>> vals0(sel0.begin(), sel0.end());
    const std::vector<Foo>                                     vals1(sel1.begin(), sel1.end());
    // TODO: Use vector comparison methods.
    // Check result length.
    compareEQ(vals0.size(), 3);
    compareEQ(vals1.size(), 3);

    // Check vector of tuples.
    compareEQ(std::get<0>(vals0[0]), 20);
    compareEQ(std::get<1>(vals0[0]), 40.5f);
    compareEQ(std::get<2>(vals0[0]), "def");
    compareEQ(std::get<0>(vals0[1]), 30);
    compareEQ(std::get<1>(vals0[1]), 80.2f);
    compareEQ(std::get<2>(vals0[1]), "ghij");
    compareEQ(std::get<0>(vals0[2]), 40);
    compareEQ(std::get<1>(vals0[2]), 100.0f);
    compareEQ(std::get<2>(vals0[2]), "aaaa");

    // Check vector of structs.
    compareEQ(vals1[0].c1, 20);
    compareEQ(vals1[0].c2, 40.5f);
    compareEQ(vals1[0].c3, "def");
    compareEQ(vals1[1].c1, 30);
    compareEQ(vals1[1].c2, 80.2f);
    compareEQ(vals1[1].c3, "ghij");
    compareEQ(vals1[2].c1, 40);
    compareEQ(vals1[2].c2, 100.0f);
    compareEQ(vals1[2].c3, "aaaa");
}
