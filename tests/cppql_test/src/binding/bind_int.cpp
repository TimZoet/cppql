#include "cppql_test/binding/bind_int.h"

void BindInt::operator()()
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
    compareTrue(stmt.bindInt(0, 10));
    compareTrue(stmt.bindInt(1, 20));
    compareTrue(stmt.bindInt64(2, 30));
    compareFalse(stmt.bindInt(3, 30));
    compareFalse(stmt.bindInt64(3, 30));
}
