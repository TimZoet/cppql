#include "cppql_test/get_column_blob.h"

struct Foo
{
    int64_t a;
    float   b;
    int8_t  c;
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

        std::vector<float> values2{40.5f, 50.5f, 60.5f};
        compareTrue(stmt.bindTransientBlob(0, values2.data(), values2.size() * sizeof(float)));
        compareTrue(stmt.step());
        compareTrue(stmt.reset());

        std::vector<Foo> values3{{.a = 100, .b = 42, .c = 3}, {.a = 200, .b = 666, .c = -4}};
        compareTrue(stmt.bindTransientBlob(0, values3.data(), values3.size() * sizeof(Foo)));
        compareTrue(stmt.step());
        compareTrue(stmt.reset());
    });

    // Create select statement.
    const auto stmt = db->createStatement("SELECT col1 FROM myTable;", true);

    std::vector<float> values;

    // Get row.
    compareTrue(stmt.step());
    stmt.column(0, values);
    compareEQ(values.size(), 3);
    compareEQ(values[0], 10.5f);
    compareEQ(values[1], 20.5f);
    compareEQ(values[2], 30.5f);

    // Get row.
    compareTrue(stmt.step());
    values = stmt.column<float*>(0);
    compareEQ(values.size(), 3);
    compareEQ(values[0], 40.5f);
    compareEQ(values[1], 50.5f);
    compareEQ(values[2], 60.5f);

    // Get row.
    compareTrue(stmt.step());
    auto values2 = stmt.column<Foo*>(0);
    compareEQ(values2.size(), 2);
    compareEQ(values2[0].a, 100);
    compareEQ(values2[0].b, 42);
    compareEQ(values2[0].c, 3);
    compareEQ(values2[1].a, 200);
    compareEQ(values2[1].b, 666);
    compareEQ(values2[1].c, -4);
}
