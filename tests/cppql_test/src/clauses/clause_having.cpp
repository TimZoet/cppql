#include "cppql_test/clauses/clause_having.h"

#include "cppql/include_all.h"

void ClauseHaving::operator()()
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

    compareEQ(sql::Having<std::nullopt_t>::toString(), "");

    auto expr = table.col<0>() == 0 && table.col<1>() >= 10.0f;
    auto having = sql::Having<decltype(expr)>(expr);
    expectNoThrow([&having] {
        int32_t idx = 10;
        having.generateIndices(idx);
        });
    compareEQ(having.toString(), "HAVING (myTable.col1 = ?11 AND myTable.col2 >= ?12)");
}
