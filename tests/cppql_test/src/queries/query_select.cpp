#include "cppql_test/queries/query_select.h"

#include "cppql/include_all.h"

namespace
{
    struct Row1
    {
        Row1(const int64_t cc0, const float cc1, std::string cc2) : c0(cc0), c1(cc1), c2(std::move(cc2)) {}

        int64_t     c0;
        float       c1;
        std::string c2;
    };

    struct Row2
    {
        Row2(std::string cc2, const float cc1, const int64_t cc0) : c0(cc0), c1(cc1), c2(std::move(cc2)) {}

        int64_t     c0;
        float       c1;
        std::string c2;
    };

    struct Row3
    {
        Row3(const int64_t cc0, std::string cc2) : c0(cc0), c2(std::move(cc2)) {}

        int64_t     c0;
        std::string c2;
    };
}  // namespace

void QuerySelect::operator()()
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
    auto q0 = table0.select();
    compareFalse(decltype(q0)::filter_t::valid);
    compareFalse(decltype(q0)::group_t::valid);
    compareFalse(decltype(q0)::having_t::valid);
    compareFalse(decltype(q0)::order_t::valid);
    compareFalse(decltype(q0)::limit_t::valid);
    compareFalse(decltype(q0)::union_t::valid);
    expectNoThrow([&] {
        int32_t idx = 0;
        q0.generateIndices(idx);
    });
    compareEQ(q0.toString(), "SELECT Table0.col1,Table0.col2,Table0.col3 FROM Table0      ");
    expectNoThrow([&] { static_cast<void>(q0.compile()); });
    compareTrue(std::same_as<decltype(q0)::return_t, std::tuple<int64_t, float, std::string>>);

    // Construct default query.
    auto q1 = table0.selectAs<Row1>();
    expectNoThrow([&] {
        int32_t idx = 0;
        q1.generateIndices(idx);
    });
    compareEQ(q1.toString(), "SELECT Table0.col1,Table0.col2,Table0.col3 FROM Table0      ");
    expectNoThrow([&] { static_cast<void>(q1.compile()); });
    compareTrue(std::same_as<decltype(q1)::return_t, Row1>);

    // Construct query with reordered indices.
    auto q2 = table0.select<2, 1, 0>();
    compareEQ(q2.toString(), "SELECT Table0.col3,Table0.col2,Table0.col1 FROM Table0      ");
    expectNoThrow([&] { static_cast<void>(q2.compile()); });

    // Construct query with reordered indices.
    auto q3 = table0.selectAs<Row2, 2, 1, 0>();
    compareEQ(q3.toString(), "SELECT Table0.col3,Table0.col2,Table0.col1 FROM Table0      ");
    expectNoThrow([&] { static_cast<void>(q3.compile()); });

    // Construct query with columns.
    auto q4 = table0.select(table0.col<0>(), table0.col<2>());
    compareEQ(q4.toString(), "SELECT Table0.col1,Table0.col3 FROM Table0      ");
    expectNoThrow([&] { static_cast<void>(q4.compile()); });

    // Construct query with columns.
    auto q5 = table0.selectAs<Row3>(table0.col<0>(), table0.col<2>());
    compareEQ(q5.toString(), "SELECT Table0.col1,Table0.col3 FROM Table0      ");
    expectNoThrow([&] { static_cast<void>(q5.compile()); });

    // Construct query with wrong table.
    expectThrow([&] { static_cast<void>(table0.select(table1.col<0>())); });

    // Add where clause.
    auto q6 = q0.where(table0.col<0>() > 0);
    compareTrue(decltype(q6)::filter_t::valid);
    expectNoThrow([&] {
        int32_t idx = 0;
        q6.generateIndices(idx);
    });
    compareEQ(q6.toString(), "SELECT Table0.col1,Table0.col2,Table0.col3 FROM Table0 WHERE Table0.col1 > ?1     ");
    expectNoThrow([&] { static_cast<void>(q6.compile()); });

    // Add where clause with wrong table.
    expectThrow([&] { static_cast<void>(q0.where(table1.col<0>() > 0)); });

    // Add group clause.
    auto q7 = q0.groupBy(table0.col<1>(), table0.col<2>());
    compareTrue(decltype(q7)::group_t::valid);
    expectNoThrow([&] {
        int32_t idx = 0;
        q7.generateIndices(idx);
    });
    compareEQ(q7.toString(),
              "SELECT Table0.col1,Table0.col2,Table0.col3 FROM Table0  GROUP BY Table0.col2,Table0.col3    ");
    expectNoThrow([&] { static_cast<void>(q7.compile()); });

    // Add group clause with wrong table.
    expectThrow([&] { static_cast<void>(q0.groupBy(table0.col<2>(), table1.col<1>())); });

    // Add having clause.
    auto q8 = q7.having(avg(table0.col<1>()) < 10.0f);
    compareTrue(decltype(q8)::having_t::valid);
    expectNoThrow([&] {
        int32_t idx = 0;
        q8.generateIndices(idx);
    });
    compareEQ(q8.toString(),
              "SELECT Table0.col1,Table0.col2,Table0.col3 FROM Table0  GROUP BY Table0.col2,Table0.col3 HAVING "
              "AVG(Table0.col2) < ?1   ");
    expectNoThrow([&] { static_cast<void>(q8.compile()); });

    // Add having clause with wrong table.
    expectThrow([&] { static_cast<void>(q7.having(table1.col<1>() == 10.0f)); });

    // Add order by clause.
    auto q9 = q0.orderBy(ascending(table0.col<1>()));
    compareTrue(decltype(q9)::order_t::valid);
    expectNoThrow([&] {
        int32_t idx = 0;
        q9.generateIndices(idx);
    });
    compareEQ(q9.toString(), "SELECT Table0.col1,Table0.col2,Table0.col3 FROM Table0     ORDER BY Table0.col2 ASC  ");
    expectNoThrow([&] { static_cast<void>(q9.compile()); });

    // Add order by clause with wrong table.
    expectThrow([&] { static_cast<void>(q0.orderBy(ascending(table1.col<1>()))); });

    // Add limit clause.
    auto q10 = q0.limitOffset(10, 20);
    compareTrue(decltype(q10)::limit_t::valid);
    expectNoThrow([&] {
        int32_t idx = 0;
        q10.generateIndices(idx);
    });
    compareEQ(q10.toString(), "SELECT Table0.col1,Table0.col2,Table0.col3 FROM Table0      LIMIT 10 OFFSET 20");
    expectNoThrow([&] { static_cast<void>(q10.compile()); });

    // Create unions.
    auto q11 = table0.select<0, 1>()
                 .where(table0.col<0>() == 0)
                 .unions(sql::UnionOperator::Intersect, table0.select<0, 1>().where(table0.col<0>() == 1))
                 .unions(sql::UnionOperator::UnionAll, table1.select<0, 1>().where(table1.col<1>() <= 10.0f));
    compareTrue(decltype(q11)::union_t::valid);
    expectNoThrow([&] {
        int32_t idx = 0;
        q11.generateIndices(idx);
    });
    compareEQ(q11.toString(),
              "SELECT Table0.col1,Table0.col2 FROM Table0 WHERE Table0.col1 = ?1   INTERSECT SELECT "
              "Table0.col1,Table0.col2 FROM Table0 WHERE Table0.col1 = ?2     UNION ALL SELECT Table1.col1,Table1.col2 "
              "FROM Table1 WHERE Table1.col2 <= ?3       ");
    expectNoThrow([&] { static_cast<void>(q11.compile()); });

    // Add all clauses.
    auto q12 = q0.where(table0.col<0>() > 0)
                 .groupBy(table0.col<1>())
                 .having(table0.col<0>() != 0)
                 .unions(sql::UnionOperator::Intersect, q0)
                 .orderBy(ascending(table0.col<1>()))
                 .limitOffset(10, 20);
    compareTrue(decltype(q12)::filter_t::valid);
    compareTrue(decltype(q12)::group_t::valid);
    compareTrue(decltype(q12)::having_t::valid);
    compareTrue(decltype(q12)::order_t::valid);
    compareTrue(decltype(q12)::limit_t::valid);
    compareTrue(decltype(q12)::union_t::valid);
    expectNoThrow([&] {
        int32_t idx = 0;
        q12.generateIndices(idx);
    });
    compareEQ(q12.toString(),
              "SELECT Table0.col1,Table0.col2,Table0.col3 FROM Table0 WHERE Table0.col1 > ?1 GROUP BY Table0.col2 "
              "HAVING Table0.col1 != ?2 INTERSECT SELECT Table0.col1,Table0.col2,Table0.col3 FROM Table0       ORDER "
              "BY Table0.col2 ASC  LIMIT 10 OFFSET 20");
    expectNoThrow([&] { static_cast<void>(q12.compile()); });
}
