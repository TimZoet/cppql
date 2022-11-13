#include "cppql_test/join/inner_join.h"

#include "cppql-typed/include_all.h"

using namespace std::string_literals;

void InnerJoin::operator()()
{
    // Create table.
    sql::Table *t0, *t1, *t2;
    expectNoThrow([&] {
        t0 = &db->createTable("Table0");
        t0->createColumn("col1", sql::Column::Type::Int);
        t0->createColumn("col2", sql::Column::Type::Real);
        t0->createColumn("col3", sql::Column::Type::Text);
        t0->commit();

        t1 = &db->createTable("Table1");
        t1->createColumn("col1", sql::Column::Type::Int);
        t1->createColumn("col2", sql::Column::Type::Real);
        t1->commit();

        t2 = &db->createTable("Table2");
        t2->createColumn("col1", sql::Column::Type::Int);
        t2->createColumn("col2", sql::Column::Type::Real);
        t2->createColumn("col3", sql::Column::Type::Int);
        t2->commit();
    });
    sql::TypedTable<int64_t, float, std::string> table0(*t0);
    sql::TypedTable<int64_t, float>              table1(*t1);
    sql::TypedTable<int64_t, float, int32_t>     table2(*t2);


    auto select = table0.join(sql::InnerJoin{}, table1).usings(table0.col<0>(), table0.col<1>());// .join(sql::InnerJoin{}, table2).on(table0.col<1>() == table2.col<1>()).select(table0.col<0>())(sql::BindParameters::All);
    //constexpr bool x = decltype(select)::has_filter_list;
    //static_cast<void>(x);
}
