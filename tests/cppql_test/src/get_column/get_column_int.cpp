#include "cppql_test/get_column/get_column_int.h"

void GetColumnInt::operator()()
{
    // Create simple table.
    expectNoThrow([this] {
        auto& table = db->createTable("myTable");
        table.createColumn("col1", sql::Column::Type::Int);
        table.createColumn("col2", sql::Column::Type::Int);
        table.commit();

        compareTrue(db->createStatement("INSERT INTO myTable VALUES (10, -10);", true).step());
        compareTrue(db->createStatement("INSERT INTO myTable VALUES (20, -20);", true).step());
        compareTrue(db->createStatement("INSERT INTO myTable VALUES (30, -30);", true).step());
        compareTrue(db->createStatement("INSERT INTO myTable VALUES (40, -40);", true).step());
        compareTrue(db->createStatement("INSERT INTO myTable VALUES (50, -50);", true).step());
    });

    // Create select statement.
    const auto stmt = db->createStatement("SELECT col1, col2 FROM myTable;", true);

    int32_t col1;
    int64_t col2;

    // Get row.
    compareTrue(stmt.step());
    stmt.column(0, col1);
    stmt.column(1, col2);
    compareEQ(col1, 10);
    compareEQ(col2, -10);

    // Get row.
    compareTrue(stmt.step());
    stmt.column(0, col1);
    stmt.column(1, col2);
    compareEQ(col1, 20);
    compareEQ(col2, -20);

    // Get row.
    compareTrue(stmt.step());
    compareEQ(stmt.column<int32_t>(0), 30);
    compareEQ(stmt.column<int64_t>(1), -30);

    // Get row.
    compareTrue(stmt.step());
    compareEQ(stmt.column<int8_t>(0), 40);
    compareEQ(stmt.column<int16_t>(1), -40);

    // Get row.
    compareTrue(stmt.step());
    compareEQ(stmt.column<uint32_t>(0), static_cast<uint32_t>(50));
    compareEQ(stmt.column<uint64_t>(1), static_cast<uint64_t>(-50));
}
