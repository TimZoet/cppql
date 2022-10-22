#include "cppql_test/get_column/get_column_type.h"

void GetColumnType::operator()()
{
    // Create simple table.
    expectNoThrow([this] {
        auto& table = db->createTable("myTable");
        table.createColumn("col1", sql::Column::Type::Text);
        table.createColumn("col2", sql::Column::Type::Real);
        table.createColumn("col3", sql::Column::Type::Blob);
        table.createColumn("col4", sql::Column::Type::Int);
        table.createColumn("col5", sql::Column::Type::Null);
        table.commit();

        constexpr int32_t x    = 0;
        const auto        stmt = db->createStatement("INSERT INTO myTable VALUES (?, ?, ?, ?, ?);", true);
        compareTrue(stmt.bindTransientText(0, "abc"));
        compareTrue(stmt.bindFloat(1, 1.0f));
        compareTrue(stmt.bindTransientBlob(2, &x, sizeof(int32_t)));
        compareTrue(stmt.bindInt(3, x));
        compareTrue(stmt.bindNull(4));
        compareTrue(stmt.step());
    });

    // Create select statement.
    const auto stmt = db->createStatement("SELECT col1, col2, col3, col4, col5 FROM myTable;", true);

    compareTrue(stmt.step());
    compareEQ(sql::Column::Type::Text, stmt.columnType(0));
    compareEQ(sql::Column::Type::Real, stmt.columnType(1));
    compareEQ(sql::Column::Type::Blob, stmt.columnType(2));
    compareEQ(sql::Column::Type::Int, stmt.columnType(3));
    compareEQ(sql::Column::Type::Null, stmt.columnType(4));
}
