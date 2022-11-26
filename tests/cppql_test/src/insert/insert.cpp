#include "cppql_test/insert/insert.h"

#include "cppql/include_all.h"

using namespace std::string_literals;

void Insert::operator()()
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
    expectNoThrow([&table] {
        auto insert = table.insert().compile();
        insert(10, 20.0f, sql::toText("abc"));
    });
    expectNoThrow([&table] {
        auto insert = table.insert(table.col<0>(), table.col<1>(), table.col<2>()).compile();
        insert(20, 40.5f, sql::toText("def"));
    });
    expectNoThrow([&table] {
        auto insert = table.insert<0, 1, 2>().compile();
        insert(30, 80.2f, sql::toText("ghij"));
    });
    expectNoThrow([&table] {
        auto insert = table.insert<2, 1, 0>().compile();
        insert(sql::toText("gh\0ij"), 133.3f, 40);
    });
    expectNoThrow([&table] {
        auto insert = table.insert().compile();
        insert(50, 99.9f, sql::toText(""));
    });

    // Create select statement to select all data.
    const auto stmt = db->createStatement("SELECT * FROM myTable;", true);

    // Check rows.

    compareTrue(stmt.step());
    compareEQ(stmt.column<int64_t>(0), 10);
    compareEQ(stmt.column<float>(1), 20.0f);
    compareEQ(stmt.column<std::string>(2), "abc"s);

    compareTrue(stmt.step());
    compareEQ(stmt.column<int64_t>(0), 20);
    compareEQ(stmt.column<float>(1), 40.5f);
    compareEQ(stmt.column<std::string>(2), "def"s);

    compareTrue(stmt.step());
    compareEQ(stmt.column<int64_t>(0), 30);
    compareEQ(stmt.column<float>(1), 80.2f);
    compareEQ(stmt.column<std::string>(2), "ghij"s);

    compareTrue(stmt.step());
    compareEQ(stmt.column<int64_t>(0), 40);
    compareEQ(stmt.column<float>(1), 133.3f);
    compareEQ(stmt.column<std::string>(2), "gh\0ij"s);

    compareTrue(stmt.step());
    compareEQ(stmt.column<int64_t>(0), 50);
    compareEQ(stmt.column<float>(1), 99.9f);
    compareEQ(stmt.column<std::string>(2), ""s);
}
