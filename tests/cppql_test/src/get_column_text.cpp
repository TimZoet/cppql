#include "cppql_test/get_column_text.h"

void GetColumnText::operator()()
{
    // Create simple table.
    expectNoThrow([this]() {
        auto& table = db->createTable("myTable");
        table.createColumn("col1", sql::Column::Type::Text);
        table.createColumn("col2", sql::Column::Type::Text);
        table.commit();

        compareTrue(db->createStatement("INSERT INTO myTable VALUES ('abc', 'def');", true).step());
        compareTrue(db->createStatement("INSERT INTO myTable VALUES ('0123', '4567');", true).step());
        compareTrue(db->createStatement("INSERT INTO myTable VALUES ('!@#$', '%&*');", true).step());
    });

    // Create select statement.
    const auto stmt = db->createStatement("SELECT col1, col2 FROM myTable;", true);

    std::string col1;
    std::string col2;

    // Get row.
    compareTrue(stmt.step());
    stmt.column(0, col1);
    stmt.column(1, col2);
    compareEQ(col1, "abc");
    compareEQ(col2, "def");

    // Get row.
    compareTrue(stmt.step());
    stmt.column(0, col1);
    stmt.column(1, col2);
    compareEQ(col1, "0123");
    compareEQ(col2, "4567");

    // Get row.
    compareTrue(stmt.step());
    compareEQ(stmt.column<std::string>(0), "!@#$");
    compareEQ(stmt.column<std::string>(1), "%&*");
}
