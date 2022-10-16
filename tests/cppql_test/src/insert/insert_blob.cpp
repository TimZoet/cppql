#include "cppql_test/insert/insert_blob.h"

#include "cppql-typed/typed_table.h"
#include "cppql-typed/queries/insert.h"

namespace
{
    struct Foo
    {
        float   x;
        int64_t y;
    };
}  // namespace

void InsertBlob::operator()()
{
    // Create table.
    sql::Table* t;
    expectNoThrow([&t, this] {
        t = &db->createTable("myTable");
        t->createColumn("col1", sql::Column::Type::Int).setAutoIncrement(true).setPrimaryKey(true);
        t->createColumn("col2", sql::Column::Type::Blob);
        t->commit();
    });
    sql::TypedTable<int64_t, uint32_t*> table(*t);

    // Generate some data.
    std::vector<uint32_t> blob1{1, 2, 3, 4, 5};
    std::vector<uint32_t> blob2{11, 22, 33, 44, 55};
    std::vector<uint32_t> blob3{1'000, 10'000, 100'000, 1'000'000, 10'000'000};
    Foo                   blob4{.x = 4.5f, .y = -10};

    // Insert several rows.
    auto insert = table.insert();
    expectNoThrow([&insert, &blob1] { insert(nullptr, sql::toStaticBlob(blob1)); });
    expectNoThrow([&insert, &blob2] { insert(nullptr, sql::toStaticBlob(blob2)); });
    expectNoThrow([&insert, &blob3] { insert(nullptr, sql::toStaticBlob(blob3)); });
    expectNoThrow([&insert, &blob4] { insert(nullptr, sql::toStaticBlob(blob4)); });

    // Create select statement to select all data.
    const auto stmt = db->createStatement("SELECT * FROM myTable;", true);

    // Check rows.
    // TODO: Once BetterTest supports directly comparing vectors, use those methods instead of iterating here.

    compareTrue(stmt.step());
    compareEQ(stmt.column<int64_t>(0), 1);
    std::vector<uint32_t> res1;
    stmt.column(1, res1);
    compareEQ(blob1.size(), res1.size());
    for (size_t i = 0; i < blob1.size(); i++) compareEQ(blob1[i], res1[i]);

    compareTrue(stmt.step());
    compareEQ(stmt.column<int64_t>(0), 2);
    std::vector<uint32_t> res2;
    stmt.column(1, res2);
    compareEQ(blob2.size(), res2.size());
    for (size_t i = 0; i < blob2.size(); i++) compareEQ(blob2[i], res2[i]);

    compareTrue(stmt.step());
    compareEQ(stmt.column<int64_t>(0), 3);
    std::vector<uint32_t> res3;
    stmt.column(1, res3);
    compareEQ(blob3.size(), res3.size());
    for (size_t i = 0; i < blob3.size(); i++) compareEQ(blob3[i], res3[i]);

    compareTrue(stmt.step());
    compareEQ(stmt.column<int64_t>(0), 4);
    Foo res4{};
    stmt.column(1, res4);
    compareEQ(res4.x, blob4.x);
    compareEQ(res4.y, blob4.y);
}
