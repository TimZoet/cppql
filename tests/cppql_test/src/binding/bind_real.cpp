#include "cppql_test/binding/bind_real.h"

void BindReal::operator()()
{
    // Create simple table.
    expectNoThrow([this] {
        auto& table = db->createTable("myTable");
        table.createColumn("col1", sql::Column::Type::Int);
        table.createColumn("col2", sql::Column::Type::Int);
        table.createColumn("col3", sql::Column::Type::Int);
        table.commit();
    });

    // Create insert statement.
    const auto stmt = db->createStatement("INSERT INTO myTable VALUES (?, ?, ?);", true);
    
    // Do some valid and invalid binds.
    compareTrue(stmt.bindFloat(0, 10));
    compareTrue(stmt.bindFloat(1, 20));
    compareTrue(stmt.bindDouble(2, 30));
    compareFalse(stmt.bindFloat(3, 30));
    compareFalse(stmt.bindDouble(3, 30));
}
