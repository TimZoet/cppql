#include "cppql_test/select/select_custom_return_type.h"

#include "cppql-typed/typed_table.h"
#include "cppql-typed/queries/insert.h"
#include "cppql-typed/queries/select.h"

using namespace std::string_literals;

namespace
{
    struct Row
    {
        Row(const int64_t cc0, const float cc1, std::string cc2) : c0(cc0), c1(cc1), c2(std::move(cc2)) {}

        int64_t     c0;
        float       c1;
        std::string c2;
    };
}  // namespace

void SelectCustomReturnType::operator()()
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
    auto insert = table.insert();
    expectNoThrow([&insert] { insert(10, 20.0f, sql::toText("abc")); });
    expectNoThrow([&insert] { insert(20, 40.5f, sql::toText("def")); });
    expectNoThrow([&insert] { insert(30, 80.2f, sql::toText("ghij")); });
    expectNoThrow([&insert] { insert(40, 100.0f, sql::toText("aaaa")); });

    // Create select query with custom return type.
    auto sel = table.select<Row, 0, 1, 2>(table.col<0>() <= 20, sql::BindParameters::All);

    // Select with unbound id.
    const std::vector<Row> vals(sel.begin(), sel.end());
    compareEQ(vals.size(), static_cast<size_t>(2));
    compareEQ(vals[0].c0, 10);
    compareEQ(vals[0].c1, 20.0f);
    compareEQ(vals[0].c2, "abc"s);
    compareEQ(vals[1].c0, 20);
    compareEQ(vals[1].c1, 40.5f);
    compareEQ(vals[1].c2, "def"s);
}
