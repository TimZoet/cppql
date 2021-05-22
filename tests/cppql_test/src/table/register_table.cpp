#include "cppql_test/table/register_table.h"

void RegisterTable::operator()()
{
    // Create two tables through raw statements. Second table has FK to first.
    const auto stmt1 = db->createStatement("CREATE TABLE table1(col1 INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, col2 TEXT);", true);
    const auto stmt2 =
      db->createStatement("CREATE TABLE table2(ref INTEGER, FOREIGN KEY (ref) REFERENCES table1(col1));", true);
    compareTrue(stmt1.step());
    compareTrue(stmt2.step());

    // First register should work, second should throw.
    expectNoThrow([this]() { db->registerTable("table1"); });
    expectNoThrow([this]() { db->registerTable("table2"); });
    expectThrow([this]() { db->registerTable("table1"); });
    expectThrow([this]() { db->registerTable("table2"); });

    // Get registered tables.
    auto& table1 = db->getTable("table1");
    auto& table2 = db->getTable("table2");

    // Verify integrity of first table.
    compareEQ(table1.getName(), "table1");
    compareEQ(table1.getColumnCount(), static_cast<size_t>(2));
    auto& col1 = table1.getColumn("col1");
    auto& col2 = table1.getColumn("col2");
    compareEQ(col1.isAutoIncrement(), true);
    compareEQ(col1.isForeignKey(), false);
    compareEQ(col1.isNotNull(), true);
    compareEQ(col1.isPrimaryKey(), true);
    compareEQ(col2.isAutoIncrement(), false);
    compareEQ(col2.isForeignKey(), false);
    compareEQ(col2.isNotNull(), false);
    compareEQ(col2.isPrimaryKey(), false);

     // Verify integrity of second table.
    compareEQ(table2.getName(), "table2");
    compareEQ(table2.getColumnCount(), static_cast<size_t>(1));
    auto& refCol = table2.getColumn("ref");
    compareEQ(refCol.isAutoIncrement(), false);
    compareEQ(refCol.isForeignKey(), true);
    compareEQ(refCol.isNotNull(), false);
    compareEQ(refCol.isPrimaryKey(), false);
    compareEQ(refCol.getForeignKey(), &col1);
}
