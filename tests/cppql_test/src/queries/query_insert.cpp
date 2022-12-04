#include "cppql_test/queries/query_insert.h"

#include "cppql/include_all.h"

void QueryInsert::operator()()
{
    // Create table.
    sql::Table *t0, *t1;
    expectNoThrow([&] {
        t0 = &db->createTable("Table0");
        t0->createColumn("col1", sql::Column::Type::Int);
        t0->createColumn("col2", sql::Column::Type::Real);
        t0->createColumn("col3", sql::Column::Type::Text);
        t0->commit();

        t1 = &db->createTable("Table1");
        t1->createColumn("col1", sql::Column::Type::Int);
        t1->createColumn("col2", sql::Column::Type::Real);
        t1->createColumn("col3", sql::Column::Type::Text);
        t1->commit();
    });
    const sql::TypedTable<int64_t, float, std::string> table0(*t0);
    const sql::TypedTable<int64_t, float, std::string> table1(*t1);

    // Construct default query.
    auto q0 = table0.insert();
    compareEQ(q0.toString(), "INSERT INTO Table0 (col1,col2,col3) VALUES (?1,?2,?3);");
    expectNoThrow([&] { static_cast<void>(q0.compile()); });

    // Construct query with all default values.
    auto q1 = table0.insert<>();
    compareEQ(q1.toString(), "INSERT INTO Table0 DEFAULT VALUES;");
    expectNoThrow([&] { static_cast<void>(q1.compile()); });

    // Construct query with reordered indices.
    auto q2 = table0.insert<2, 1, 0>();
    compareEQ(q2.toString(), "INSERT INTO Table0 (col3,col2,col1) VALUES (?1,?2,?3);");
    expectNoThrow([&] { static_cast<void>(q2.compile()); });

    // Construct query with columns.
    auto q3 = table0.insert(table0.col<0>(), table0.col<2>());
    compareEQ(q3.toString(), "INSERT INTO Table0 (col1,col3) VALUES (?1,?2);");
    expectNoThrow([&] { static_cast<void>(q3.compile()); });

    // Construct query with wrong table.
    expectThrow([&] { static_cast<void>(table0.insert(table1.col<0>())); });
}
