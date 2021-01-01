#include "cppql_test/bind_text.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql/database.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql_test/utils.h"

void BindText::operator()()
{
    auto db = createDatabase();

    // Create simple table.
    expectNoThrow([&db]() {
        auto& table = db->createTable("myTable");
        table.createColumn("col1", sql::Column::Type::Text);
        table.createColumn("col2", sql::Column::Type::Text);
        table.createColumn("col3", sql::Column::Type::Text);
        table.commit();
    });

    // Create insert statement.
    auto stmt = db->createStatement("INSERT INTO myTable VALUES (?, ?, ?);", true);

    // Allocate some data.
    const char* data1 = new char[10];
    const char* data2 = new char[10];
    const char* data3 = new char[10];

    // Do some valid and invalid binds.
    compareTrue(stmt.bindText(0, data1, 10, [](void* p) { delete[] static_cast<const char*>(p); }));
    compareTrue(stmt.bindStaticText(1, data2, 10));
    compareTrue(stmt.bindTransientText(2, data3, 10));
    compareFalse(stmt.bindStaticText(3, data2, 10));
    compareFalse(stmt.bindTransientText(3, data2, 10));

    // Delete data whose ownership was not passed to binds.
    delete[] data2;
    delete[] data3;

    removeDatabase();
}
