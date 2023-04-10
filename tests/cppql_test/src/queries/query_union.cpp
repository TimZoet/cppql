#include "cppql_test/queries/query_union.h"

#include "cppql/include_all.h"

namespace
{
    struct Row
    {
        int32_t c0;
    };
}  // namespace

void QueryUnion::operator()()
{
    // Create table.
    sql::Table* t0;
    expectNoThrow([&] {
        t0 = &db->createTable("Table0");
        t0->createColumn("col1", sql::Column::Type::Int);
        t0->commit();
    });
    const sql::TypedTable<int64_t> table0(*t0);

    expectNoThrow([&] {
        auto insert = table0.insert().compile();
        for (int32_t i = 0; i < 20; i++) insert(i);
    });

    int32_t p0 = 0, p1 = 0;
    auto    query = table0.selectAs<int32_t>()
                   .where(table0.col<0>() == &p0)
                   .unions(sql::UnionOperator::Union, table0.selectAs<int32_t>().where(table0.col<0>() == &p1))
                   .compile();

    p0 = 10;
    p1 = 12;
    query.bind(sql::BindParameters::All);
    const auto rows = std::vector(query.begin(), query.end());
    compareEQ(std::vector<int32_t>{10, 12}, rows);
}
