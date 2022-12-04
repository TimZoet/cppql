#include "cppql_test/queries/query_update.h"

#include "cppql/include_all.h"

void QueryUpdate::operator()()
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
    sql::TypedTable<int64_t, float, std::string>       table0(*t0);
    const sql::TypedTable<int64_t, float, std::string> table1(*t1);

    // Construct default query.
    auto q0 = table0.update();
    compareEQ(q0.toString(), "UPDATE Table0 SET (col1,col2,col3) = (?1,?2,?3)   ;");
    expectNoThrow([&] { static_cast<void>(q0.compile()); });

    // Construct query with reordered indices.
    auto q1 = table0.update<2, 1, 0>();
    compareEQ(q1.toString(), "UPDATE Table0 SET (col3,col2,col1) = (?1,?2,?3)   ;");
    expectNoThrow([&] { static_cast<void>(q1.compile()); });

    // Construct query with columns.
    auto q2 = table0.update(table0.col<0>(), table0.col<2>());
    compareEQ(q2.toString(), "UPDATE Table0 SET (col1,col3) = (?1,?2)   ;");
    expectNoThrow([&] { static_cast<void>(q2.compile()); });

    // Construct query with wrong table.
    expectThrow([&] { static_cast<void>(table0.update(table1.col<0>())); });

    // Add where clause.
    auto q3 = q0.where(table0.col<0>() > 0);
    compareTrue(decltype(q3)::filter_t::valid);
    expectNoThrow([&] { q3.generateIndices(); });
    compareEQ(q3.toString(), "UPDATE Table0 SET (col1,col2,col3) = (?1,?2,?3) WHERE Table0.col1 > ?4  ;");
    expectNoThrow([&] { static_cast<void>(q3.compile()); });

    // Add where clause with wrong table.
    expectThrow([&] { static_cast<void>(q0.where(table1.col<0>() > 0)); });

    // Add order by clause.
    auto q4 = q0.orderBy(ascending(table0.col<1>()));
    compareTrue(decltype(q4)::order_t::valid);
    expectNoThrow([&] { q4.generateIndices(); });
    compareEQ(q4.toString(), "UPDATE Table0 SET (col1,col2,col3) = (?1,?2,?3)  ORDER BY Table0.col2 ASC  ;");
    expectNoThrow([&] { static_cast<void>(q4.compile()); }).info("ORDER BY not supported.");

    // Add order by clause with wrong table.
    expectThrow([&] { static_cast<void>(q0.orderBy(ascending(table1.col<1>()))); });

    // Add limit clause.
    auto q5 = q0.limitOffset(10, 20);
    compareTrue(decltype(q5)::limit_t::valid);
    expectNoThrow([&] { q5.generateIndices(); });
    compareEQ(q5.toString(), "UPDATE Table0 SET (col1,col2,col3) = (?1,?2,?3)   LIMIT 10 OFFSET 20;");
    expectNoThrow([&] { static_cast<void>(q5.compile()); }).info("LIMIT OFFSET not supported.");

    // Add all clauses.
    auto q6 = q0.where(table0.col<0>() > 0).orderBy(ascending(table0.col<1>())).limitOffset(10, 20);
    compareTrue(decltype(q6)::filter_t::valid);
    compareTrue(decltype(q6)::order_t::valid);
    compareTrue(decltype(q6)::limit_t::valid);
    expectNoThrow([&] { q6.generateIndices(); });
    compareEQ(q6.toString(),
              "UPDATE Table0 SET (col1,col2,col3) = (?1,?2,?3) WHERE Table0.col1 > ?4 ORDER BY Table0.col2 ASC  LIMIT "
              "10 OFFSET 20;");
    expectNoThrow([&] { static_cast<void>(q6.compile()); }).info("ORDER BY/LIMIT OFFSET not supported.");
}
