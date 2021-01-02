#include "cppql_test/bind_null.h"

void BindNull::operator()()
{
    // Create simple table.
    expectNoThrow([this]() {
        auto& table = db->createTable("myTable");
        table.createColumn("col1", sql::Column::Type::Int);
        table.createColumn("col2", sql::Column::Type::Int);
        table.createColumn("col3", sql::Column::Type::Int);
        table.commit();
    });

    // Create insert statement.
    const auto stmt = db->createStatement("INSERT INTO myTable VALUES (?, ?, ?);", true);
    
    // Do some valid and invalid binds.
    compareTrue(stmt.bindNull(0));
    compareTrue(stmt.bindNull(1));
    compareTrue(stmt.bindNull(2));
    compareFalse(stmt.bindNull(3));
    compareFalse(stmt.bindNull(3));
}
