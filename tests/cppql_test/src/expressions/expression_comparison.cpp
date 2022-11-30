#include "cppql_test/expressions/expression_comparison.h"

#include "cppql/include_all.h"

void ExpressionComparison::operator()()
{
    // Create tables.
    sql::Table *t0 = nullptr, *t1 = nullptr;
    expectNoThrow([&] {
        t0 = &db->createTable("peepee");
        t0->createColumn("col1", sql::Column::Type::Int);
        t0->createColumn("col2", sql::Column::Type::Real);
        t0->commit();

        t1 = &db->createTable("poopoo");
        t1->createColumn("col1", sql::Column::Type::Int);
        t1->createColumn("col2", sql::Column::Type::Real);
        t1->commit();
    });
    const sql::TypedTable<int64_t, float> table0(*t0);

    float val = 0.0f;

    auto eq0 = table0.col<1>() == 0.0f;
    auto eq1 = 0.0f == table0.col<1>();
    auto eq2 = table0.col<1>() == &val;
    auto eq3 = &val == table0.col<1>();
    auto eq4 = table0.col<1>() == nullptr;
    auto eq5 = nullptr == table0.col<1>();
    expectNoThrow([&] {
        int32_t idx = 0;
        eq0.generateIndices(idx);
        eq1.generateIndices(idx);
        eq2.generateIndices(idx);
        eq3.generateIndices(idx);
        eq4.generateIndices(idx);
        eq5.generateIndices(idx);
    });
    compareEQ(eq0.toString(), "peepee.col2 = ?1");
    compareEQ(eq1.toString(), "?2 = peepee.col2");
    compareEQ(eq2.toString(), "peepee.col2 = ?3");
    compareEQ(eq3.toString(), "?4 = peepee.col2");
    compareEQ(eq4.toString(), "peepee.col2 = ?5");
    compareEQ(eq5.toString(), "?6 = peepee.col2");
    compareTrue(eq0.containsTables(*t0));
    compareTrue(eq0.containsTables(*t0, *t1));
    compareFalse(eq0.containsTables(*t1));

    auto ne0 = table0.col<1>() != 0.0f;
    auto ne1 = 0.0f != table0.col<1>();
    auto ne2 = table0.col<1>() != &val;
    auto ne3 = &val != table0.col<1>();
    auto ne4 = table0.col<1>() != nullptr;
    auto ne5 = nullptr != table0.col<1>();
    expectNoThrow([&] {
        int32_t idx = 0;
        ne0.generateIndices(idx);
        ne1.generateIndices(idx);
        ne2.generateIndices(idx);
        ne3.generateIndices(idx);
        ne4.generateIndices(idx);
        ne5.generateIndices(idx);
    });
    compareEQ(ne0.toString(), "peepee.col2 != ?1");
    compareEQ(ne1.toString(), "?2 != peepee.col2");
    compareEQ(ne2.toString(), "peepee.col2 != ?3");
    compareEQ(ne3.toString(), "?4 != peepee.col2");
    compareEQ(ne4.toString(), "peepee.col2 != ?5");
    compareEQ(ne5.toString(), "?6 != peepee.col2");

    auto lt0 = table0.col<1>() < 0.0f;
    auto lt1 = 0.0f < table0.col<1>();
    auto lt2 = table0.col<1>() < &val;
    auto lt3 = &val < table0.col<1>();
    expectNoThrow([&] {
        int32_t idx = 0;
        lt0.generateIndices(idx);
        lt1.generateIndices(idx);
        lt2.generateIndices(idx);
        lt3.generateIndices(idx);
    });
    compareEQ(lt0.toString(), "peepee.col2 < ?1");
    compareEQ(lt1.toString(), "?2 < peepee.col2");
    compareEQ(lt2.toString(), "peepee.col2 < ?3");
    compareEQ(lt3.toString(), "?4 < peepee.col2");

    auto gt0 = table0.col<1>() > 0.0f;
    auto gt1 = 0.0f > table0.col<1>();
    auto gt2 = table0.col<1>() > &val;
    auto gt3 = &val > table0.col<1>();
    expectNoThrow([&] {
        int32_t idx = 0;
        gt0.generateIndices(idx);
        gt1.generateIndices(idx);
        gt2.generateIndices(idx);
        gt3.generateIndices(idx);
    });
    compareEQ(gt0.toString(), "peepee.col2 > ?1");
    compareEQ(gt1.toString(), "?2 > peepee.col2");
    compareEQ(gt2.toString(), "peepee.col2 > ?3");
    compareEQ(gt3.toString(), "?4 > peepee.col2");

    auto le0 = table0.col<1>() <= 0.0f;
    auto le1 = 0.0f <= table0.col<1>();
    auto le2 = table0.col<1>() <= &val;
    auto le3 = &val <= table0.col<1>();
    expectNoThrow([&] {
        int32_t idx = 0;
        le0.generateIndices(idx);
        le1.generateIndices(idx);
        le2.generateIndices(idx);
        le3.generateIndices(idx);
    });
    compareEQ(le0.toString(), "peepee.col2 <= ?1");
    compareEQ(le1.toString(), "?2 <= peepee.col2");
    compareEQ(le2.toString(), "peepee.col2 <= ?3");
    compareEQ(le3.toString(), "?4 <= peepee.col2");

    auto ge0 = table0.col<1>() >= 0.0f;
    auto ge1 = 0.0f >= table0.col<1>();
    auto ge2 = table0.col<1>() >= &val;
    auto ge3 = &val >= table0.col<1>();
    expectNoThrow([&] {
        int32_t idx = 0;
        ge0.generateIndices(idx);
        ge1.generateIndices(idx);
        ge2.generateIndices(idx);
        ge3.generateIndices(idx);
    });
    compareEQ(ge0.toString(), "peepee.col2 >= ?1");
    compareEQ(ge1.toString(), "?2 >= peepee.col2");
    compareEQ(ge2.toString(), "peepee.col2 >= ?3");
    compareEQ(ge3.toString(), "?4 >= peepee.col2");
}
