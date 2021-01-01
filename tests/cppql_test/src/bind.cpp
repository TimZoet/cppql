#include "cppql_test/bind.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql/database.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql_test/utils.h"

void Bind::operator()()
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

    // Create unprepared insert statement.
    auto stmt = db->createStatement("INSERT INTO myTable VALUES (?, ?, ?);", false);

    // Bind before prepare should fail.
    compareFalse(stmt.bindInt(0, 10));

    // Prepare.
    compareTrue(stmt.prepare());

    // Do some valid and invalid binds.
    compareTrue(stmt.bindInt(0, 10));
    compareTrue(stmt.bindInt(1, 20));
    compareTrue(stmt.bindInt(2, 30));
    compareFalse(stmt.bindInt(3, 40));

    // Clear bindings.
    compareTrue(stmt.clearBindings());

    // Try binding again.
    compareTrue(stmt.bindInt(0, 10));
    compareTrue(stmt.bindInt(1, 20));
    compareTrue(stmt.bindInt(2, 30));
    compareFalse(stmt.bindInt(3, 40));

    removeDatabase();
}
