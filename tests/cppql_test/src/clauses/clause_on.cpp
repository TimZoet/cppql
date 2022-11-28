#include "cppql_test/clauses/clause_on.h"

#include "cppql/include_all.h"

void ClauseOn::operator()()
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

    compareEQ(sql::On<std::nullopt_t>::toString(), "");

    auto expr = table.col<0>() == 0 && table.col<1>() >= 10.0f;
    auto on = sql::On<decltype(expr)>(expr);
    expectNoThrow([&on] {
        int32_t idx = 10;
        on.generateIndices(idx);
        });
    compareEQ(on.toString(), "ON (myTable.col1 = ?11 AND myTable.col2 >= ?12)");
}
