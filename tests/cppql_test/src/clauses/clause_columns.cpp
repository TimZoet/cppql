#include "cppql_test/clauses/clause_columns.h"

#include "cppql/include_all.h"

void ClauseColumns::operator()()
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

    const auto columns0 = sql::Columns();
    compareEQ(columns0.toString(), "");
    compareEQ(columns0.toStringFull(), "");

    const auto columns1 = sql::Columns(table.col<0>());
    compareEQ(columns1.toString(), "col1");
    compareEQ(columns1.toStringFull(), "myTable.col1");

    const auto columns2 = sql::Columns(table.col<2>(), table.col<0>(), table.col<1>());
    compareEQ(columns2.toString(), "col3,col1,col2");
    compareEQ(columns2.toStringFull(), "myTable.col3,myTable.col1,myTable.col2");
}
