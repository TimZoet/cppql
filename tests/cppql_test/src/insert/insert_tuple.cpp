#include "cppql_test/insert/insert_tuple.h"

#include "cppql-typed/typed_table.h"
#include "cppql-typed/queries/insert.h"

using namespace std::string_literals;

void InsertTuple::operator()()
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
    sql::TypedTable<int64_t, float, std::string> table(*t);

    // Insert several rows.
    auto insert = table.insert();
    expectNoThrow([&insert]() { insert(std::make_tuple<int64_t, float, sql::Text>(10, 20.0f, sql::toText("abc"))); });
    expectNoThrow([&insert]() { insert(std::make_tuple<int64_t, float, sql::Text>(20, 40.5f, sql::toText("def"))); });
    expectNoThrow([&insert]() { insert(std::make_tuple<int64_t, float, sql::Text>(30, 80.2f, sql::toText("ghij"))); });
    expectNoThrow(
      [&insert]() { insert(std::make_tuple<int64_t, float, sql::Text>(40, 133.3f, sql::toText("gh\0ij"))); });

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
}
