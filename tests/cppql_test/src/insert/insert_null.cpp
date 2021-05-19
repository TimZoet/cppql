#include "cppql_test/insert/insert_null.h"

#include "cppql/ext/typed_table.h"
#include "cppql/ext/queries/insert.h"

using namespace std::string_literals;

void InsertNull::operator()()
{
    // Create table.
    sql::Table* t;
    expectNoThrow([&t, this]() {
        t = &db->createTable("myTable");
        t->createColumn("col1", sql::Column::Type::Int).setAutoIncrement(true).setPrimaryKey(true).setNotNull(true);
        t->createColumn("col2", sql::Column::Type::Real);
        t->createColumn("col3", sql::Column::Type::Text);
        t->createColumn("col4", sql::Column::Type::Int);
        t->commit();
    });
    sql::ext::TypedTable<int64_t, float, std::string, int32_t> table(*t);

    // Insert several rows.
    expectNoThrow([&]() { table.insert()(nullptr, 11.0f, "abc"s, nullptr); });
    expectNoThrow([&]() { table.insert()(nullptr, 12.0f, nullptr, 10); });
    expectNoThrow([&]() { table.insert()(nullptr, nullptr, "def"s, 20); });
    expectNoThrow([&]() { table.insert<1, 2, 3>()(13.0f, "ghi"s, 30); });
    expectNoThrow([&]() { table.insert<>()(); });
    expectNoThrow([&]() { table.insert<3, 2, 1>()(40, "jkl"s, 14.0f); });
    expectNoThrow([&]() { table.insert<1, 3>()(15.0f, 50); });
    expectNoThrow([&]() {
        const std::optional<float>       f;
        const std::optional<std::string> s;
        const std::optional<int32_t>     i;
        table.insert()(nullptr, f, i, s);
    });

    // Create select statement to select all data.
    const auto stmt = db->createStatement("SELECT * FROM myTable;", true);

    // Check rows.

    compareTrue(stmt.step());
    compareEQ(stmt.column<int64_t>(0), 1);
    compareEQ(stmt.column<float>(1), 11.0f);
    compareEQ(stmt.column<std::string>(2), "abc"s);
    compareEQ(stmt.column<int32_t>(3), 0);
    compareEQ(stmt.columnType(3), sql::Column::Type::Null);

    compareTrue(stmt.step());
    compareEQ(stmt.column<int64_t>(0), 2);
    compareEQ(stmt.column<float>(1), 12.0f);
    compareEQ(stmt.column<std::string>(2), ""s);
    compareEQ(stmt.column<int32_t>(3), 10);
    compareEQ(stmt.columnType(2), sql::Column::Type::Null);

    compareTrue(stmt.step());
    compareEQ(stmt.column<int64_t>(0), 3);
    compareEQ(stmt.column<float>(1), 0.0f);
    compareEQ(stmt.column<std::string>(2), "def"s);
    compareEQ(stmt.column<int32_t>(3), 20);
    compareEQ(stmt.columnType(1), sql::Column::Type::Null);

    compareTrue(stmt.step());
    compareEQ(stmt.column<int64_t>(0), 4);
    compareEQ(stmt.column<float>(1), 13.0f);
    compareEQ(stmt.column<std::string>(2), "ghi"s);
    compareEQ(stmt.column<int32_t>(3), 30);

    compareTrue(stmt.step());
    compareEQ(stmt.column<int64_t>(0), 5);
    compareEQ(stmt.column<float>(1), 0.0f);
    compareEQ(stmt.column<std::string>(2), ""s);
    compareEQ(stmt.column<int32_t>(3), 0);
    compareEQ(stmt.columnType(1), sql::Column::Type::Null);
    compareEQ(stmt.columnType(2), sql::Column::Type::Null);
    compareEQ(stmt.columnType(3), sql::Column::Type::Null);

    compareTrue(stmt.step());
    compareEQ(stmt.column<int64_t>(0), 6);
    compareEQ(stmt.column<float>(1), 14.0f);
    compareEQ(stmt.column<std::string>(2), "jkl"s);
    compareEQ(stmt.column<int32_t>(3), 40);

    compareTrue(stmt.step());
    compareEQ(stmt.column<int64_t>(0), 7);
    compareEQ(stmt.column<float>(1), 15.0f);
    compareEQ(stmt.column<std::string>(2), ""s);
    compareEQ(stmt.column<int32_t>(3), 50);
    compareEQ(stmt.columnType(2), sql::Column::Type::Null);

    compareTrue(stmt.step());
    compareEQ(stmt.column<int64_t>(0), 8);
    compareEQ(stmt.column<float>(1), 0.0f);
    compareEQ(stmt.column<std::string>(2), ""s);
    compareEQ(stmt.column<int32_t>(3), 0);
    compareEQ(stmt.columnType(1), sql::Column::Type::Null);
    compareEQ(stmt.columnType(2), sql::Column::Type::Null);
    compareEQ(stmt.columnType(3), sql::Column::Type::Null);
}
