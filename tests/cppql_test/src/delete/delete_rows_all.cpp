#include "cppql_test/delete/delete_rows_all.h"

#include "cppql-typed/include_all.h"

using namespace std::string_literals;

void DeleteRowsAll::operator()()
{
    // Create table.
    sql::Table* t;
    expectNoThrow([&t, this] {
        t = &db->createTable("myTable");
        t->createColumn("col1", sql::Column::Type::Int);
        t->createColumn("col2", sql::Column::Type::Real);
        t->createColumn("col3", sql::Column::Type::Text);
        t->commit();
    });
    sql::TypedTable<int64_t, float, std::string> table(*t);

    // Insert several rows.
    auto insert = table.insert()();
    expectNoThrow([&insert] { insert(10, 11.0f, sql::toText("abc")); });
    expectNoThrow([&insert] { insert(20, 12.0f, sql::toText("def")); });
    expectNoThrow([&insert] { insert(30, 13.0f, sql::toText("ghi")); });
    expectNoThrow([&insert] { insert(40, 14.0f, sql::toText("jkl")); });

    // Delete all rows.
    const auto del = table.del()(sql::BindParameters::None);
    del();

    // Check row count.
    const auto count = db->createStatement("SELECT COUNT(*) FROM myTable;", true);
    compareTrue(count.isPrepared());
    compareTrue(count.step());
    compareEQ(count.column<int32_t>(0), 0);
}
