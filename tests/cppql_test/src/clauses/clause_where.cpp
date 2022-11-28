#include "cppql_test/clauses/clause_where.h"

#include "cppql/include_all.h"

void ClauseWhere::operator()()
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

    compareEQ(sql::Where<std::nullopt_t>::toString(), "");

    auto expr = table.col<0>() == 0 && table.col<1>() >= 10.0f;
    auto where = sql::Where<decltype(expr)>(expr);
    expectNoThrow([&where] {
        int32_t idx = 10;
        where.generateIndices(idx);
        });
    compareEQ(where.toString(), "WHERE (myTable.col1 = ?11 AND myTable.col2 >= ?12)");
}
