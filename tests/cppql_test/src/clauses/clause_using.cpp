#include "cppql_test/clauses/clause_using.h"

#include "cppql/include_all.h"

void ClauseUsing::operator()()
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

    compareEQ(sql::Using<>::toString(), "");

    const auto using0 = sql::Using<col0_t>(table.col<0>());
    compareEQ(using0.toString(), "USING(col1)");

    const auto using1 = sql::Using<col2_t, col0_t, col1_t>(table.col<2>(), table.col<0>(), table.col<1>());
    compareEQ(using1.toString(), "USING(col3,col1,col2)");
}
