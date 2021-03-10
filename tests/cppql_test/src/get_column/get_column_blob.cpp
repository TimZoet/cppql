#include "cppql_test/get_column/get_column_blob.h"

struct Foo
{
    int64_t a = 0;
    float   b = 0;
    int8_t  c = 0;
};

struct Bar
{
    int32_t a;
};

void GetColumnBlob::operator()()
{
    // Create simple table.
    expectNoThrow([this]() {
        auto& table = db->createTable("myTable");
        table.createColumn("col1", sql::Column::Type::Blob);
        table.commit();

        const auto stmt = db->createStatement("INSERT INTO myTable VALUES (?);", true);

        std::vector<float> values1{10.5f, 20.5f, 30.5f};
        compareTrue(stmt.bindTransientBlob(0, values1.data(), values1.size() * sizeof(float)));
        compareTrue(stmt.step());
        compareTrue(stmt.reset());

        std::vector<int64_t> values2{40, 50, 60};
        compareTrue(stmt.bindTransientBlob(0, values2.data(), values2.size() * sizeof(int64_t)));
        compareTrue(stmt.step());
        compareTrue(stmt.reset());

        Foo value3 = {.a = 100, .b = 42, .c = 3};
        compareTrue(stmt.bindTransientBlob(0, &value3, sizeof(Foo)));
        compareTrue(stmt.step());
        compareTrue(stmt.reset());

        Foo value4 = {.a = 200, .b = 242, .c = 23};
        compareTrue(stmt.bindTransientBlob(0, &value4, sizeof(Foo)));
        compareTrue(stmt.step());
        compareTrue(stmt.reset());

        std::vector<Foo> values5 = {Foo{.a = 10, .b = 4.4f, .c = -4}, Foo{.a = -20, .b = -8.8f, .c = 5}};
        compareTrue(stmt.bindTransientBlob(0, values5.data(), values5.size() * sizeof(Foo)));
        compareTrue(stmt.step());
        compareTrue(stmt.reset());

         Foo value6 = {.a = 200, .b = 242, .c = 23};
        compareTrue(stmt.bindTransientBlob(0, &value6, sizeof(Foo)));
        compareTrue(stmt.step());
        compareTrue(stmt.reset());
    });

    // Create select statement.
    const auto stmt = db->createStatement("SELECT col1 FROM myTable;", true);

    // Get row as vector of floats.
    compareTrue(stmt.step());
    std::vector<float> vals1;
    stmt.column(0, vals1);
    compareEQ(vals1.size(), static_cast<size_t>(3));
    compareEQ(vals1[0], 10.5f);
    compareEQ(vals1[1], 20.5f);
    compareEQ(vals1[2], 30.5f);

    // Get row as vector of ints.
    compareTrue(stmt.step());
    const auto vals2 = stmt.column<std::vector<int64_t>>(0);
    compareEQ(vals2.size(), static_cast<size_t>(3));
    compareEQ(vals2[0], static_cast<int64_t>(40));
    compareEQ(vals2[1], static_cast<int64_t>(50));
    compareEQ(vals2[2], static_cast<int64_t>(60));

    // Get row as struct.
    compareTrue(stmt.step());
    Foo val3;
    stmt.column(0, val3);
    compareEQ(val3.a, 100);
    compareEQ(val3.b, 42);
    compareEQ(val3.c, 3);

    // Get row as struct.
    compareTrue(stmt.step());
    const auto val4 = stmt.column<Foo>(0);
    compareEQ(val4.a, 200);
    compareEQ(val4.b, 242);
    compareEQ(val4.c, 23);

    // Get row as vector of structs.
    compareTrue(stmt.step());
    const auto val5 = stmt.column<std::vector<Foo>>(0);
    compareEQ(vals2.size(), static_cast<size_t>(3));
    compareEQ(val5[0].a, static_cast<int64_t>(10));
    compareEQ(val5[0].b, 4.4f);
    compareEQ(val5[0].c, static_cast<int8_t>(-4));
    compareEQ(val5[1].a, static_cast<int64_t>(-20));
    compareEQ(val5[1].b, -8.8f);
    compareEQ(val5[1].c, static_cast<int8_t>(5));

    // Get row with wrong size.
    compareTrue(stmt.step());
    expectThrow([&stmt, this]() {
        Bar val;
        stmt.column(0, val);
    });
}
