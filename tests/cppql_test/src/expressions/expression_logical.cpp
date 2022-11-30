#include "cppql_test/expressions/expression_logical.h"

#include "cppql/include_all.h"

void ExpressionLogical::operator()()
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

    auto or0 = table0.col<0>() == 10 || table0.col<1>() != table1.col<1>();
    auto or1 = table0.col<0>() >= table1.col<1>() || table0.col<1>() <= 0.0f;
    expectNoThrow([&] {
        int32_t idx = 0;
        or0.generateIndices(idx);
        or1.generateIndices(idx);
    });
    compareEQ(or0.toString(), "(peepee.col1 = ?1 OR peepee.col2 != poopoo.col2)");
    compareEQ(or1.toString(), "(peepee.col1 >= poopoo.col2 OR peepee.col2 <= ?2)");
    compareTrue(or0.containsTables(*t0));
    compareTrue(or0.containsTables(*t0, *t1));
    compareTrue(or0.containsTables(*t0, *t1, *t2));
    compareFalse(or0.containsTables(*t2));

    auto expr = or1 && or0;
    expectNoThrow([&] {
        int32_t idx = 0;
        expr.generateIndices(idx);
    });
    compareEQ(expr.toString(),
              "((peepee.col1 >= poopoo.col2 OR peepee.col2 <= ?1) AND (peepee.col1 = ?2 OR peepee.col2 != poopoo.col2))");
    compareTrue(expr.containsTables(*t0));
    compareTrue(expr.containsTables(*t0, *t1));
    compareTrue(expr.containsTables(*t0, *t1, *t2));
    compareFalse(expr.containsTables(*t2));
}
