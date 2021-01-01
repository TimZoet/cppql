#include "cppql_test/bind_null.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql/database.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql_test/utils.h"

void BindNull::operator()()
{
    auto db = createDatabase();

    // Create simple table.
    expectNoThrow([&db]() {
        auto& table = db->createTable("myTable");
        table.createColumn("col1", sql::Column::Type::Int);
        table.createColumn("col2", sql::Column::Type::Int);
        table.createColumn("col3", sql::Column::Type::Int);
        table.commit();
    });

    // Create insert statement.
    auto stmt = db->createStatement("INSERT INTO myTable VALUES (?, ?, ?);", true);
    
    // Do some valid and invalid binds.
    compareTrue(stmt.bindNull(0));
    compareTrue(stmt.bindNull(1));
    compareTrue(stmt.bindNull(2));
    compareFalse(stmt.bindNull(3));
    compareFalse(stmt.bindNull(3));

    removeDatabase();
}
