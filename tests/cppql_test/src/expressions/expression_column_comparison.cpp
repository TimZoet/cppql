#include "cppql_test/expressions/expression_column_comparison.h"

#include "cppql/include_all.h"

void ExpressionColumnComparison::operator()()
{
    // Create tables.
    sql::Table *t0 = nullptr, *t1 = nullptr, *t2 = nullptr;
    expectNoThrow([&] {
        t0 = &db->createTable("peepee");
        t0->createColumn("col1", sql::Column::Type::Int);
        t0->createColumn("col2", sql::Column::Type::Real);
        t0->commit();

        t1 = &db->createTable("poopoo");
        t1->createColumn("col1", sql::Column::Type::Int);
        t1->createColumn("col2", sql::Column::Type::Real);
        t1->commit();

        t2 = &db->createTable("dummy");
        t2->createColumn("col1", sql::Column::Type::Int);
        t2->commit();
    });
    const sql::TypedTable<int64_t, float> table0(*t0);
    const sql::TypedTable<int64_t, float> table1(*t1);

    const auto eq = table0.col<0>() == table1.col<0>();
    const auto ne = table0.col<1>() != table1.col<1>();
    const auto lt = table0.col<0>() < table1.col<1>();
    const auto gt = table0.col<1>() > table1.col<0>();
    const auto le = table0.col<0>() <= max(table0.col<1>());
    const auto ge = table0.col<1>() >= avg<float, true>(table0.col<0>());

    compareEQ(eq.toString(), "peepee.col1 = poopoo.col1");
    compareEQ(ne.toString(), "peepee.col2 != poopoo.col2");
    compareEQ(lt.toString(), "peepee.col1 < poopoo.col2");
    compareEQ(gt.toString(), "peepee.col2 > poopoo.col1");
    compareEQ(le.toString(), "peepee.col1 <= MAX(peepee.col2)");
    compareEQ(ge.toString(), "peepee.col2 >= AVG(DISTINCT peepee.col1)");

    compareTrue(eq.containsTables(*t0));
    compareTrue(eq.containsTables(*t1));
    compareTrue(eq.containsTables(*t0, *t1));
    compareTrue(eq.containsTables(*t0, *t1, *t2));
    compareTrue(eq.containsTables(*t0, *t2));
    compareFalse(eq.containsTables(*t2));

    expectThrow([&] { static_cast<void>(table0.col<0>() == table0.col<0>()); });
    expectThrow([&] { static_cast<void>(table0.col<0>() != table0.col<0>()); });
    expectThrow([&] { static_cast<void>(table0.col<0>() < table0.col<0>()); });
    expectThrow([&] { static_cast<void>(table0.col<0>() > table0.col<0>()); });
    expectThrow([&] { static_cast<void>(table0.col<0>() <= table0.col<0>()); });
    expectThrow([&] { static_cast<void>(table0.col<0>() >= table0.col<0>()); });
}
