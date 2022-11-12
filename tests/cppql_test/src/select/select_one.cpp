#include "cppql_test/select/select_one.h"

#include "cppql-typed/include_all.h"

using namespace std::string_literals;

void SelectOne::operator()()
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
    expectNoThrow([&insert] { insert(40, 200.0f, sql::toText("bbbb")); });

    // Create select one.
    int64_t id  = 0;
    auto    sel = table.select<0, 1, 2>().where(table.col<0>() == &id).one(sql::BindParameters::None);

    // Select by several IDs that should result in exactly one row.
    expectNoThrow([&id, &sel] {
        id = 10;
        sel(sql::BindParameters::All);
    });
    expectNoThrow([&id, &sel] {
        id = 20;
        sel(sql::BindParameters::All);
    });
    expectNoThrow([&id, &sel] {
        id = 30;
        sel(sql::BindParameters::All);
    });

    // Select by IDs that should result in 0 or more than 1 results.
    expectThrow([&id, &sel] {
        id = 0;
        sel(sql::BindParameters::All);
    });
    expectThrow([&id, &sel] {
        id = 40;
        sel(sql::BindParameters::All);
    });
}
