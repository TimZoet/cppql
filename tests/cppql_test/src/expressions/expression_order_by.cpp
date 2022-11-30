#include "cppql_test/expressions/expression_order_by.h"

#include "cppql/include_all.h"

void ExpressionOrderBy::operator()()
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

    const auto order = ascending(table0.col<0>()) + descending(table0.col<1>()) +
                       ascending(table1.col<1>(), sql::Nulls::First) + descending(table1.col<0>(), sql::Nulls::Last);

    compareEQ(order.toString(),
              "peepee.col1 ASC , peepee.col2 DESC , poopoo.col2 ASC NULLS FIRST, poopoo.col1 DESC NULLS LAST");
    compareTrue(order.containsTables(*t0));
    compareTrue(order.containsTables(*t1));
    compareTrue(order.containsTables(*t0, *t1));
    compareTrue(order.containsTables(*t0, *t1, *t2));
    compareFalse(order.containsTables(*t2));
}
