#include "cppql_test/select/select.h"

#include "cppql-typed/include_all.h"

using namespace std::string_literals;

void Select::operator()()
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
    auto insert = table.insert().compile();
    expectNoThrow([&insert] { insert(10, 20.0f, sql::toText("abc")); });
    expectNoThrow([&insert] { insert(20, 40.5f, sql::toText("def")); });
    expectNoThrow([&insert] { insert(30, 80.2f, sql::toText("ghij")); });
    expectNoThrow([&insert] { insert(40, 100.0f, sql::toText("aaaa")); });
    expectNoThrow([&insert] { insert(40, 200.0f, sql::toText("bbbb")); });

    // Create select query.
    int64_t id  = 20;
    auto sel = table.select<0, 1, 2>().where(table.col<0>() == &id).compile();

    // Select with unbound id.
    std::vector<std::tuple<int64_t, float, std::string>> vals(sel.begin(), sel.end());
    compareEQ(vals.size(), static_cast<size_t>(0));

    // Select with bound id 20.
    sel.bind(sql::BindParameters::All);
    vals.assign(sel.begin(), sel.end());
    compareEQ(vals.size(), static_cast<size_t>(1));
    compareEQ(vals[0], std::make_tuple<int64_t, float, std::string>(20, 40.5f, "def"));

    // Select with bound id 20.
    id = 30;
    vals.assign(sel.begin(), sel.end());
    compareEQ(vals.size(), static_cast<size_t>(1));
    compareEQ(vals[0], std::make_tuple<int64_t, float, std::string>(20, 40.5f, "def"));

    // Select with bound id 40.
    id = 40;
    sel.bind(sql::BindParameters::All);
    vals.assign(sel.begin(), sel.end());
    compareEQ(vals.size(), static_cast<size_t>(2));
    compareEQ(vals[0], std::make_tuple<int64_t, float, std::string>(40, 100.0f, "aaaa"));
    compareEQ(vals[1], std::make_tuple<int64_t, float, std::string>(40, 200.0f, "bbbb"));

    // Create select query and immediately bind.
    auto sel2 = table.select<0, 1, 2>().where(table.col<0>() == 30).compile().bind(sql::BindParameters::All);
    vals.assign(sel2.begin(), sel2.end());
    compareEQ(vals.size(), static_cast<size_t>(1));
    compareEQ(vals[0], std::make_tuple<int64_t, float, std::string>(30, 80.2f, "ghij"));
}
