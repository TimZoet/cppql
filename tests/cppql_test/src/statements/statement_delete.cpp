#include "cppql_test/statements/statement_delete.h"

#include "cppql/include_all.h"

void StatementDelete::operator()()
{
    // Create table.
    sql::Table* t0;
    expectNoThrow([&] {
        t0 = &db->createTable("Table0");
        t0->createColumn("col1", sql::Column::Type::Int);
        t0->createColumn("col2", sql::Column::Type::Real);
        t0->createColumn("col3", sql::Column::Type::Text);
        t0->commit();
    });
    sql::TypedTable<int64_t, float, std::string> table0(*t0);

    // Insert several rows.
    expectNoThrow([&] {
        auto insert = table0.insert().compile();
        insert(10, 11.0f, sql::toText("abc"));
        insert(20, 12.0f, sql::toText("def"));
        insert(30, 13.0f, sql::toText("ghi"));
        insert(40, 14.0f, sql::toText("jkl"));
    });

    // Delete all and count.
    auto stmt0 = table0.del().compile();
    expectNoThrow([&] { stmt0.bind(sql::BindParameters::All); });
    expectNoThrow([&] { stmt0(); });
    compareEQ(0, table0.count().compile()());

    // Insert several rows.
    expectNoThrow([&] {
        auto insert = table0.insert().compile();
        insert(10, 11.0f, sql::toText("abc"));
        insert(20, 12.0f, sql::toText("def"));
        insert(30, 13.0f, sql::toText("ghi"));
        insert(40, 14.0f, sql::toText("jkl"));
    });

    // Delete some with bound and unbound params.
    auto stmt1 = table0.del().where(table0.col<1>() > 12.0f).compile();
    expectNoThrow([&] { stmt1(); });
    compareEQ(4, table0.count().compile()());
    expectNoThrow([&] { stmt1.bind(sql::BindParameters::All); });
    expectNoThrow([&] { stmt1(); });
    compareEQ(2, table0.count().compile()());
}
