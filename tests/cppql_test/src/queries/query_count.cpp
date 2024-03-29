#include "cppql_test/queries/query_count.h"

#include "cppql/include_all.h"

void QueryCount::operator()()
{
    // Create table.
    sql::Table *t0, *t1;
    expectNoThrow([&] {
        t0 = &db->createTable("Table0");
        t0->createColumn("col1", sql::Column::Type::Int);
        t0->createColumn("col2", sql::Column::Type::Real);
        t0->createColumn("col3", sql::Column::Type::Text);
        t0->commit();

        t1 = &db->createTable("Table1");
        t1->createColumn("col1", sql::Column::Type::Int);
        t1->createColumn("col2", sql::Column::Type::Real);
        t1->createColumn("col3", sql::Column::Type::Text);
        t1->commit();
    });
    const sql::TypedTable<int64_t, float, std::string> table0(*t0);
    const sql::TypedTable<int64_t, float, std::string> table1(*t1);

    // Construct default query.
    auto q0 = table0.count();
    expectNoThrow([&] { q0.generateIndices(); });
    compareEQ(q0.toString(), "SELECT COUNT(*) FROM Table0 ;");
    expectNoThrow([&] { static_cast<void>(q0.compile()); });

    // Add where clause.
    auto q1 = q0.where(table0.col<0>() > 0);
    compareTrue(decltype(q1)::filter_t::valid);
    expectNoThrow([&] { q1.generateIndices(); });
    compareEQ(q1.toString(), "SELECT COUNT(*) FROM Table0 WHERE Table0.col1 > ?1;");
    expectNoThrow([&] { static_cast<void>(q1.compile()); });

    // Add where clause with wrong table.
    expectThrow([&] { static_cast<void>(q0.where(table1.col<0>() > 0)); });
}
