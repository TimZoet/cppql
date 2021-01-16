#include "cppql_test/insert_default.h"

#include "cppql/ext/insert.h"
#include "cppql/ext/typed_table.h"

using namespace std::string_literals;

void InsertDefault::operator()()
{
    // Create table.
    sql::Table* t;
    expectNoThrow([&t, this]() {
        t = &db->createTable("myTable");
        t->createColumn("col1", sql::Column::Type::Int).setAutoIncrement(true).setPrimaryKey(true).setNotNull(true);
        t->createColumn("col2", sql::Column::Type::Real);
        t->createColumn("col3", sql::Column::Type::Text);
        t->commit();
    });
    sql::ext::TypedTable<int64_t, float, std::string> table(*t);

    // Insert several rows.
    auto insert = table.insert();
    expectNoThrow([&insert]() { insert(nullptr, 20.0f, nullptr); });
    expectNoThrow([&insert]() { insert(nullptr, 40.5f, "abc"s); });
    expectNoThrow([&insert]() { insert(nullptr, nullptr, "def"s); });
    expectNoThrow([&insert]() { insert(nullptr, nullptr, "ghij"s); });

    // Create select statement to select all data.
    const auto stmt = db->createStatement("SELECT * FROM myTable;", true);

    // Check rows.

    compareTrue(stmt.step());
    compareEQ(stmt.column<int64_t>(0), 1);
    compareEQ(stmt.column<float>(1), 20.0f);
    compareEQ(stmt.column<std::string>(2), ""s);

    compareTrue(stmt.step());
    compareEQ(stmt.column<int64_t>(0), 2);
    compareEQ(stmt.column<float>(1), 40.5f);
    compareEQ(stmt.column<std::string>(2), "abc"s);

    compareTrue(stmt.step());
    compareEQ(stmt.column<int64_t>(0), 3);
    compareEQ(stmt.column<float>(1), 0.0f);
    compareEQ(stmt.column<std::string>(2), "def"s);

    compareTrue(stmt.step());
    compareEQ(stmt.column<int64_t>(0), 4);
    compareEQ(stmt.column<float>(1), 0.0f);
    compareEQ(stmt.column<std::string>(2), "ghij"s);
}
