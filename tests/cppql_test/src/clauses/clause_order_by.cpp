#include "cppql_test/clauses/clause_order_by.h"

#include "cppql/include_all.h"

void ClauseOrderBy::operator()()
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

    compareEQ(sql::OrderBy<std::nullopt_t>::toString(), "");

    auto expr = ascending(table.col<2>(), sql::Nulls::Last) + ascending(table.col<0>()) +
                descending(table.col<1>(), sql::Nulls::First);
    const auto order = sql::OrderBy<decltype(expr)>(expr);
    compareEQ(order.toString(), "ORDER BY myTable.col3 ASC NULLS LAST, myTable.col1 ASC , myTable.col2 DESC NULLS FIRST");
}
