#include "cppql_test/binding/bind_template.h"

void BindTemplate::operator()()
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

    // Allocate some data.
    const void*       data1 = new uint8_t[10];
    const void*       data2 = new uint8_t[10];
    const void*       data3 = new uint8_t[10];
    const char*       str1  = new char[6];
    const std::string str2("abcde");
    const std::string str3("abcde");

    // Single binds.
    compareTrue(stmt.bind<int32_t>(0, 10));
    compareTrue(stmt.bind<size_t>(0, 10));
    compareTrue(stmt.bind<uint8_t>(0, 10));
    compareTrue(stmt.bind<float>(0, 10.0f));
    compareTrue(stmt.bind<double>(0, 10.0));
    compareTrue(stmt.bind<std::nullptr_t>(0, nullptr));
    compareTrue(stmt.bind(0, sql::Text{str1, 6, [](void* p) { delete[] static_cast<char*>(p); }}));
    compareTrue(stmt.bind(0, sql::StaticText{str2.c_str(), 6}));
    compareTrue(stmt.bind(0, sql::TransientText{str3.c_str(), 6}));
    compareTrue(stmt.bind(0, sql::Blob{data1, 6, [](void* p) { delete[] static_cast<uint8_t*>(p); }}));
    compareTrue(stmt.bind(0, sql::StaticBlob{data2, 6}));
    compareTrue(stmt.bind(0, sql::TransientBlob{data3, 6}));

    // Multiple binds.
    compareTrue(stmt.bind(0, 10, 20.0f, sql::TransientText{str3.c_str(), 6}));

    // Offset binds.
    compareTrue(stmt.bind<int32_t>(1, 10));
    compareTrue(stmt.bind<int32_t>(2, 10));
    compareFalse(stmt.bind<int32_t>(3, 10));
    compareFalse(stmt.bind<int32_t>(4, 10));
    compareFalse(stmt.bind<int32_t>(-1, 10));
    compareFalse(stmt.bind(1, 10, 20.0f, sql::TransientText{str3.c_str(), 6}));

    // Optional binds.
    compareTrue(stmt.bind<std::optional<int32_t>>(0, 10));
    compareTrue(stmt.bind<std::optional<int32_t>>(0, {}));
    compareTrue(stmt.bind<std::optional<int32_t>, std::optional<float>>(0, 10, 20.f));
    compareTrue(stmt.bind<std::optional<int32_t>, std::optional<float>>(0, 10, {}));
    compareTrue(stmt.bind<std::optional<int32_t>, std::optional<float>>(0, {}, 20.0f));
    compareTrue(stmt.bind<std::optional<int32_t>, std::optional<float>>(0, {}, {}));

    // Delete data whose ownership was not passed to binds.
    delete[] static_cast<const uint8_t*>(data2);
    delete[] static_cast<const uint8_t*>(data3);
}
