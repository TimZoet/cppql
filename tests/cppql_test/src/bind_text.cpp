#include "cppql_test/bind_text.h"

void BindText::operator()()
{
    // Create simple table.
    expectNoThrow([this]() {
        auto& table = db->createTable("myTable");
        table.createColumn("col1", sql::Column::Type::Text);
        table.createColumn("col2", sql::Column::Type::Text);
        table.createColumn("col3", sql::Column::Type::Text);
        table.commit();
    });

    // Create insert statement.
    const auto stmt = db->createStatement("INSERT INTO myTable VALUES (?, ?, ?);", true);

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
}
