#include "cppql_test/clauses/clause_group_by.h"

#include "cppql/include_all.h"

void ClauseGroupBy::operator()()
{
    // Create table.
    sql::Table* t = nullptr;
    expectNoThrow([&t, this] {
        t = &db->createTable("myTable");
        t->createColumn("col1", sql::Column::Type::Int);
        t->createColumn("col2", sql::Column::Type::Real);
        t->createColumn("col3", sql::Column::Type::Text);
        t->commit();
    });
    const sql::TypedTable<int64_t, float, std::string> table(*t);
    using col0_t = decltype(table.col<0>());
    using col1_t = decltype(table.col<1>());
    using col2_t = decltype(table.col<2>());

    compareEQ(sql::GroupBy<>::toString(), "");

    const auto groups1 = sql::GroupBy<col0_t>(table.col<0>());
    compareEQ(groups1.toString(), "GROUP BY myTable.col1");

    const auto groups2 = sql::GroupBy<col2_t, col0_t, col1_t>(table.col<2>(), table.col<0>(), table.col<1>());
    compareEQ(groups2.toString(), "GROUP BY myTable.col3,myTable.col1,myTable.col2");
}
