#include "cppql_test/bind_blob.h"

void BindBlob::operator()()
{
    // Create simple table.
    expectNoThrow([this]() {
        auto& table = db->createTable("myTable");
        table.createColumn("col1", sql::Column::Type::Blob);
        table.createColumn("col2", sql::Column::Type::Blob);
        table.createColumn("col3", sql::Column::Type::Blob);
        table.commit();
    });

    // Create insert statement.
    const auto stmt = db->createStatement("INSERT INTO myTable VALUES (?, ?, ?);", true);
    
    // Allocate some data.
    const void* data1 = new uint8_t[10];
    const void* data2 = new uint8_t[10];
    const void* data3 = new uint8_t[10];

    // Do some valid and invalid binds.
    compareTrue(stmt.bindBlob(0, data1, 10, [](void* p){ delete[] static_cast<const uint8_t*>(p); }));
    compareTrue(stmt.bindStaticBlob(1, data2, 10));
    compareTrue(stmt.bindTransientBlob(2, data3, 10));
    compareFalse(stmt.bindStaticBlob(3, data2, 10));
    compareFalse(stmt.bindTransientBlob(3, data2, 10));

    // Delete data whose ownership was not passed to binds.
    delete[] static_cast<const uint8_t*>(data2);
    delete[] static_cast<const uint8_t*>(data3);
}
