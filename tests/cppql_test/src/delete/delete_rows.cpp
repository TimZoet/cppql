#include "cppql_test/delete/delete_rows.h"

#include "cppql-typed/typed_table.h"
#include "cppql-typed/queries/delete.h"
#include "cppql-typed/queries/insert.h"

using namespace std::string_literals;

void DeleteRows::operator()()
{
    // Create table.
    sql::Table* t;
    expectNoThrow([&t, this]() {
        t = &db->createTable("myTable");
        t->createColumn("col1", sql::Column::Type::Int);
        t->createColumn("col2", sql::Column::Type::Real);
        t->createColumn("col3", sql::Column::Type::Text);
        t->commit();
    });
    sql::TypedTable<int64_t, float, std::string> table(*t);

    // Insert several rows.
    auto insert = table.insert();
    expectNoThrow([&insert]() { insert(10, 11.0f, sql::toText("abc")); });
    expectNoThrow([&insert]() { insert(20, 12.0f, sql::toText("def")); });
    expectNoThrow([&insert]() { insert(30, 13.0f, sql::toText("ghi")); });
    expectNoThrow([&insert]() { insert(40, 14.0f, sql::toText("jkl")); });

    // Create delete and select count queries.
    int32_t    id  = 0;
    const auto del = table.del(table.col<0>() == &id, false);
    const auto count = db->createStatement("SELECT COUNT(*) FROM myTable;", true);
    compareTrue(count.isPrepared());

    // Delete with unbound id.
    del();
    compareTrue(count.step());
    compareEQ(count.column<int32_t>(0), 4);
    compareTrue(count.reset());

    // Delete with bound id 0.
    del(true);
    compareTrue(count.step());
    compareEQ(count.column<int32_t>(0), 4);
    compareTrue(count.reset());

    // Delete with bound id 10.
    id = 10;
    del(true);
    compareTrue(count.step());
    compareEQ(count.column<int32_t>(0), 3);
    compareTrue(count.reset());

    // Delete with bound id 10.
    id = 40;
    del(false);
    compareTrue(count.step());
    compareEQ(count.column<int32_t>(0), 3);
    compareTrue(count.reset());

    // Delete with bound id 40.
    id = 40;
    del(true);
    compareTrue(count.step());
    compareEQ(count.column<int32_t>(0), 2);
    compareTrue(count.reset());

    // Delete with bound id 30.
    const auto del2 = table.del(table.col<0>() == 30, true);
    del2(false);
    compareTrue(count.step());
    compareEQ(count.column<int32_t>(0), 1);
    compareTrue(count.reset());
}
