#include "cppql_test/expressions/expression_aggregate.h"

#include "cppql/include_all.h"

void ExpressionAggregate::operator()()
{
    // Create tables.
    sql::Table *t0 = nullptr, *t1 = nullptr;
    expectNoThrow([&] {
        t0 = &db->createTable("myTable");
        t0->createColumn("col1", sql::Column::Type::Int);
        t0->createColumn("col2", sql::Column::Type::Real);
        t0->createColumn("col3", sql::Column::Type::Text);
        t0->commit();

        t1 = &db->createTable("dummyTable");
        t1->createColumn("col1", sql::Column::Type::Int);
        t1->commit();
    });
    const sql::TypedTable<int64_t, float, std::string> table(*t0);

    const auto avg = sql::avg(table.col<1>());
    compareTrue(avg.containsTables(*t0));
    compareFalse(avg.containsTables(*t1));
    compareTrue(avg.containsTables(*t0, *t1));
    compareEQ(avg.toString(), "AVG(myTable.col2)");

    expectNoThrow([&] {
        static_cast<void>(sql::count(table.col<1>()));
        static_cast<void>(sql::max(table.col<1>()));
        static_cast<void>(sql::min(table.col<1>()));
        static_cast<void>(sql::sum(table.col<1>()));
        static_cast<void>(sql::total(table.col<1>()));
    });
}
