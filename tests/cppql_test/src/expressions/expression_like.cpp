#include "cppql_test/expressions/expression_like.h"

#include "cppql/include_all.h"

void ExpressionLike::operator()()
{
    // Create tables.
    sql::Table* t0 = nullptr, * t1 = nullptr;
    expectNoThrow([&] {
        t0 = &db->createTable("peepee");
        t0->createColumn("col1", sql::Column::Type::Int);
        t0->createColumn("col2", sql::Column::Type::Text);
        t0->commit();

        t1 = &db->createTable("poopoo");
        t1->createColumn("col1", sql::Column::Type::Int);
        t1->createColumn("col2", sql::Column::Type::Text);
        t1->commit();
        });
    const sql::TypedTable<int64_t, std::string> table0(*t0);

    std::string val;

    auto eq0 = like(table0.col<1>(), "");
    auto eq1 = like(table0.col<1>(), &val);
    expectNoThrow([&] {
        int32_t idx = 0;
        eq0.generateIndices(idx);
        eq1.generateIndices(idx);
        });
    compareEQ(eq0.toString(), "peepee.col2 LIKE ?1");
    compareEQ(eq1.toString(), "peepee.col2 LIKE ?2");
    compareTrue(eq0.containsTables(*t0));
    compareTrue(eq0.containsTables(*t0, *t1));
    compareFalse(eq0.containsTables(*t1));
}
