#include "cppql_test/clauses/clause_union.h"

#include "cppql/include_all.h"

void ClauseUnion::operator()()
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
    sql::TypedTable<int64_t, float, std::string> table(*t);

    auto q0     = table.select();
    auto q1     = table.select();
    auto q2     = table.select();
    auto union0 = sql::Union<decltype(q0)>(sql::UnionOperator::Intersect, q0);
    auto union1 = sql::Union<decltype(q0), decltype(q1)>(sql::UnionOperator::Union, union0.query, q1);
    auto union2 = sql::Union<decltype(q0), decltype(q1), decltype(q2)>(sql::UnionOperator::Except, union1.query, q2);

    compareEQ(sql::Union<std::nullopt_t>::toString(), "");
    compareEQ(union0.toString(), "INTERSECT SELECT myTable.col1,myTable.col2,myTable.col3 FROM myTable      ");
    compareEQ(union1.toString(),
              "INTERSECT SELECT myTable.col1,myTable.col2,myTable.col3 FROM myTable      UNION SELECT "
              "myTable.col1,myTable.col2,myTable.col3 FROM myTable      ");
    compareEQ(union2.toString(),
              "INTERSECT SELECT myTable.col1,myTable.col2,myTable.col3 FROM myTable      UNION SELECT "
              "myTable.col1,myTable.col2,myTable.col3 FROM myTable      EXCEPT SELECT "
              "myTable.col1,myTable.col2,myTable.col3 FROM myTable      ");
}
