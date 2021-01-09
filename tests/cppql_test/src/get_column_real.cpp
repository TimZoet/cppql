#include "cppql_test/get_column_real.h"

void GetColumnReal::operator()()
{
    // Create simple table.
    expectNoThrow([this]() {
        auto& table = db->createTable("myTable");
        table.createColumn("col1", sql::Column::Type::Real);
        table.createColumn("col2", sql::Column::Type::Real);
        table.commit();

        compareTrue(db->createStatement("INSERT INTO myTable VALUES (10, -10);", true).step());
        compareTrue(db->createStatement("INSERT INTO myTable VALUES (20, -20);", true).step());
        compareTrue(db->createStatement("INSERT INTO myTable VALUES (30, -30);", true).step());
    });

    // Create select statement.
    const auto stmt = db->createStatement("SELECT col1, col2 FROM myTable;", true);

    float col1;
    double col2;

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
    compareEQ(stmt.column<float>(0), 30);
    compareEQ(stmt.column<double>(1), -30);
}
