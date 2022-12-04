#include "cppql_test/statements/statement_select_one.h"

#include "cppql/include_all.h"

void StatementSelectOne::operator()()
{
    // Create table.
    sql::Table* t0;
    expectNoThrow([&] {
        t0 = &db->createTable("Table0");
        t0->createColumn("col1", sql::Column::Type::Int);
        t0->createColumn("col2", sql::Column::Type::Real);
        t0->createColumn("col3", sql::Column::Type::Text);
        t0->commit();
    });
    const sql::TypedTable<int64_t, float, std::string> table0(*t0);

    // Insert several rows.
    expectNoThrow([&] {
        auto insert = table0.insert().compile();
        insert(10, 20.0f, sql::toText("abc"));
        insert(20, 40.5f, sql::toText("def"));
        insert(40, 100.0f, sql::toText("aaaa"));
        insert(40, 200.0f, sql::toText("bbbb"));
    });

    int64_t id   = 10;
    auto    stmt = table0.selectAs<float, 1>().where(table0.col<0>() == &id).compileOne();

    compareEQ(20.0f, stmt.bind(sql::BindParameters::All)());
    id = 20;
    compareEQ(40.5f, stmt.bind(sql::BindParameters::All)());
    // Select without result should throw.
    id = 30;
    expectThrow([&] { static_cast<void>(stmt.bind(sql::BindParameters::All)()); });
    // Select with more than one result should throw.
    id = 40;
    expectThrow([&] { static_cast<void>(stmt.bind(sql::BindParameters::All)()); });
}
