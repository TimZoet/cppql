#include "cppql_test/queries/query_join.h"

#include "cppql/include_all.h"

namespace
{
    struct Row
    {
        Row(const int64_t cc0, const float cc1, std::string cc2, std::string cc3) :
            c0(cc0), c1(cc1), c2(std::move(cc2)), c3(std::move(cc3))
        {
        }

        int64_t     c0;
        float       c1;
        std::string c2;
        std::string c3;
    };
}  // namespace

void QueryJoin::operator()()
{
    // Create table.
    sql::Table *t0, *t1, *t2;
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

        t2 = &db->createTable("Table2");
        t2->createColumn("col1", sql::Column::Type::Int);
        t2->createColumn("col2", sql::Column::Type::Real);
        t2->createColumn("col3", sql::Column::Type::Text);
        t2->commit();
    });
    const sql::TypedTable<int64_t, float, std::string> table0(*t0);
    const sql::TypedTable<int64_t, float, std::string> table1(*t1);
    const sql::TypedTable<int64_t, float, std::string> table2(*t2);

    expectNoThrow([&] { static_cast<void>(table0.join(sql::CrossJoin, table1)); });
    expectNoThrow([&] { static_cast<void>(table0.join(sql::LeftJoin, table1)); });
    expectNoThrow([&] { static_cast<void>(table0.join(sql::RightJoin, table1)); });
    expectNoThrow([&] { static_cast<void>(table0.join(sql::FullJoin, table1)); });
    expectNoThrow([&] { static_cast<void>(table0.join(sql::InnerJoin, table1)); });
    expectNoThrow([&] { static_cast<void>(table0.join(sql::NaturalLeftJoin, table1)); });
    expectNoThrow([&] { static_cast<void>(table0.join(sql::NaturalRightJoin, table1)); });
    expectNoThrow([&] { static_cast<void>(table0.join(sql::NaturalFullJoin, table1)); });
    expectNoThrow([&] { static_cast<void>(table0.join(sql::NaturalInnerJoin, table1)); });

    // Construct default join.
    auto q0 = table0.join(sql::InnerJoin, table1);
    compareFalse(decltype(q0)::filter_t::valid);
    compareFalse(decltype(q0)::using_t::valid);
    compareFalse(decltype(q0)::join_t::natural);
    expectNoThrow([&] {
        int32_t idx = 0;
        q0.generateIndices(idx);
    });
    compareEQ(q0.toString(), "Table0 INNER JOIN Table1 ");

    // Add on clause.
    auto q1 = q0.on(table0.col<0>() != 0 && table1.col<1>() < 1.0f);
    compareTrue(decltype(q1)::filter_t::valid);
    compareEQ(q1.toString(), "Table0 INNER JOIN Table1 ON (Table0.col1 != ?0 AND Table1.col2 < ?0)");

    // Add on clause with wrong table.
    expectThrow([&] { q0.on(table2.col<0>() != 0); });

    // Add using clause.
    auto q2 = q0.usings(table0.col<0>(), table0.col<1>());
    compareTrue(decltype(q2)::using_t::valid);
    compareEQ(q2.toString(), "Table0 INNER JOIN Table1 USING(col1,col2)");

    // Add using clause with wrong table.
    expectThrow([&] { q0.usings(table2.col<0>()); });

    // Construct select query.
    auto q3 =
      table0.join(sql::InnerJoin, table1).select(table0.col<0>(), table0.col<1>(), table0.col<2>(), table1.col<2>());
    compareTrue(std::same_as<decltype(q3)::return_t, std::tuple<int64_t, float, std::string, std::string>>);
    compareEQ(q3.toString(),
              "SELECT Table0.col1,Table0.col2,Table0.col3,Table1.col3 FROM Table0 INNER JOIN Table1       ");
    expectNoThrow([&] { static_cast<void>(q3.compile()); });

    // Construct select query.
    auto q4 = table0.join(sql::InnerJoin, table1)
                .selectAs<Row>(table0.col<0>(), table0.col<1>(), table0.col<2>(), table1.col<2>());
    compareTrue(std::same_as<decltype(q4)::return_t, Row>);
    compareEQ(q4.toString(),
              "SELECT Table0.col1,Table0.col2,Table0.col3,Table1.col3 FROM Table0 INNER JOIN Table1       ");
    expectNoThrow([&] { static_cast<void>(q4.compile()); });
}
