#include "cppql_test/select/select_one.h"

#include "cppql/ext/insert.h"
#include "cppql/ext/select_one.h"
#include "cppql/ext/typed_table.h"

using namespace std::string_literals;

void SelectOne::operator()()
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
    expectNoThrow([&insert]() { insert(40, 200.0f, "bbbb"s); });

    // Create select one.
    int64_t id  = 0;
    auto sel = table.selectOne<0, 1, 2>(table.col<0>() == &id, false);

    // Select by several IDs that should result in exactly one row.
    expectNoThrow([&id, &sel]() {
        id = 10;
        sel(true);
    });
    expectNoThrow([&id, &sel]() {
        id = 20;
        sel(true);
    });
    expectNoThrow([&id, &sel]() {
        id = 30;
        sel(true);
    });

    // Select by IDs that should result in 0 or more than 1 results.
    expectThrow([&id, &sel]() {
        id = 0;
        sel(true);
    });
    expectThrow([&id, &sel]() {
        id = 40;
        sel(true);
    });
}