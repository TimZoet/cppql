#include "cppql_test/queries/query_delete.h"

#include "cppql/include_all.h"

void QueryDelete::operator()()
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
    sql::TypedTable<int64_t, float, std::string> table0(*t0);
    sql::TypedTable<int64_t, float, std::string> table1(*t1);

    // Construct default query.
    auto q0 = table0.del();
    expectNoThrow([&] { q0.generateIndices(); });
    compareEQ(q0.toString(), "DELETE FROM Table0   ;");
    expectNoThrow([&] { static_cast<void>(q0.compile()); });

    // Add where clause.
    auto q1 = q0.where(table0.col<0>() > 0);
    compareTrue(decltype(q1)::filter_t::valid);
    expectNoThrow([&] { q1.generateIndices(); });
    compareEQ(q1.toString(), "DELETE FROM Table0 WHERE Table0.col1 > ?1  ;");
    expectNoThrow([&] { static_cast<void>(q1.compile()); });

    // Add where clause with wrong table.
    expectThrow([&] { static_cast<void>(q0.where(table1.col<0>() > 0)); });

    // Add order by clause.
    auto q2 = q0.orderBy(ascending(table0.col<1>()));
    compareTrue(decltype(q2)::order_t::valid);
    expectNoThrow([&] { q2.generateIndices(); });
    compareEQ(q2.toString(), "DELETE FROM Table0  ORDER BY Table0.col2 ASC  ;");
    expectNoThrow([&] { static_cast<void>(q2.compile()); });

    // Add order by clause with wrong table.
    expectThrow([&] { static_cast<void>(q0.orderBy(ascending(table1.col<1>()))); });

    // Add limit clause.
    auto q3 = q0.limitOffset(10, 20);
    compareTrue(decltype(q3)::limit_t::valid);
    expectNoThrow([&] { q3.generateIndices(); });
    compareEQ(q3.toString(), "DELETE FROM Table0   LIMIT 10 OFFSET 20;");
    expectNoThrow([&] { static_cast<void>(q3.compile()); });

    // Add all clauses.
    auto q4 = q0.where(table0.col<0>() > 0).orderBy(ascending(table0.col<1>())).limitOffset(10, 20);
    compareTrue(decltype(q4)::filter_t::valid);
    compareTrue(decltype(q4)::order_t::valid);
    compareTrue(decltype(q4)::limit_t::valid);
    expectNoThrow([&] { q4.generateIndices(); });
    compareEQ(q4.toString(), "DELETE FROM Table0 WHERE Table0.col1 > ?1 ORDER BY Table0.col2 ASC  LIMIT 10 OFFSET 20;");
    expectNoThrow([&] { static_cast<void>(q4.compile()); });
}
