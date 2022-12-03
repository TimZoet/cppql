#include "cppql_test/statements/statement_count.h"

#include "cppql/include_all.h"

void StatementCount::operator()()
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

    // Count all.
    auto stmt0 = table0.count().compile();
    expectNoThrow([&] { stmt0.bind(sql::BindParameters::All); });
    compareEQ(4, stmt0());

    // Count with unbound and bound params.
    auto stmt1 = table0.count().where(table0.col<1>() > 12.0f).compile();
    compareEQ(0, stmt1());
    expectNoThrow([&] { stmt1.bind(sql::BindParameters::All); });
    compareEQ(2, stmt1());
}
