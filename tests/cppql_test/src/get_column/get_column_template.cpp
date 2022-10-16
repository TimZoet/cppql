#include "cppql_test/get_column/get_column_template.h"

using namespace std::string_literals;

namespace
{
    struct Foo
    {
        int32_t x;
        float   y;
    };
}  // namespace

void GetColumnTemplate::operator()()
{
    // Create table with all column types.
    expectNoThrow([this] {
        auto& table = db->createTable("myTable");

        table.createColumn("col1", sql::Column::Type::Int);
        table.createColumn("col2", sql::Column::Type::Real);
        table.createColumn("col3", sql::Column::Type::Text);
        table.createColumn("col4", sql::Column::Type::Blob);

        table.createColumn("col5", sql::Column::Type::Int);
        table.createColumn("col6", sql::Column::Type::Real);
        table.createColumn("col7", sql::Column::Type::Text);
        table.createColumn("col8", sql::Column::Type::Blob);

        table.commit();

        const auto insert = db->createStatement("INSERT INTO myTable VALUES (?, ?, ?, ?, ?, ?, ?, ?);", true);

        compareTrue(insert.bindInt(0, 10));
        compareTrue(insert.bindFloat(1, 4.0f));
        compareTrue(insert.bindTransientText(2, std::string("abc")));
        const Foo foo{.x = 20, .y = 8.0f};
        compareTrue(insert.bindStaticBlob(3, &foo, sizeof(Foo)));

        compareTrue(insert.bindInt(4, -10));
        compareTrue(insert.bindFloat(5, -4.0f));
        compareTrue(insert.bindTransientText(6, std::string("def")));
        const std::vector<uint8_t> vec{0, 1, 2, 3, 4, 5, 6, 7};
        compareTrue(insert.bindStaticBlob(7, vec.data(), sizeof(uint8_t) * vec.size()));

        // Insert row.
        compareTrue(insert.step());
    });

    // Create select statement.
    const auto stmt = db->createStatement("SELECT col1, col2, col3, col4, col5, col6, col7, col8 FROM myTable;", true);

    compareTrue(stmt.step());
    compareEQ(stmt.column<int32_t>(0), 10);
    compareEQ(stmt.column<float>(1), 4.0f);
    compareEQ(stmt.column<std::string>(2), "abc"s);
    compareEQ(stmt.column<Foo>(3).x, 20);
    compareEQ(stmt.column<Foo>(3).y, 8.0f);

    compareEQ(stmt.column<int32_t>(4), -10);
    compareEQ(stmt.column<float>(5), -4.0f);
    compareEQ(stmt.column<std::string>(6), "def"s);
    const auto v = stmt.column<std::vector<uint8_t>>(7);
    compareEQ(v.size(), static_cast<size_t>(8));
    for (size_t i = 0; i < v.size(); i++) compareEQ(v[i], static_cast<uint8_t>(i));
}
