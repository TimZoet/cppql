#include "cppql_test/expressions/expression_column.h"

#include "cppql/include_all.h"

void ExpressionColumn::operator()()
{
    // Create tables.
    sql::Table* t0 = nullptr, * t1 = nullptr;
    expectNoThrow([&] {
        t0 = &db->createTable("myTable");
        t0->createColumn("col1", sql::Column::Type::Int);
        t0->createColumn("col2", sql::Column::Type::Real);
        t0->createColumn("col3", sql::Column::Type::Text);
        t0->commit();

        t1 = &db->createTable("dummyTable");
        t1->createColumn("col1", sql::Column::Type::Int);
        t1->commit();
        });
    const sql::TypedTable<int64_t, float, std::string> table(*t0);

    const auto col0 = table.col<0>();
    const auto col1 = table.col<1>();
    const auto col2 = table.col<2>();

    compareTrue(col0.containsTables(*t0));
    compareFalse(col0.containsTables(*t1));
    compareTrue(col0.containsTables(*t0, *t1));
    compareTrue(col1.containsTables(*t0));
    compareFalse(col1.containsTables(*t1));
    compareTrue(col1.containsTables(*t0, *t1));
    compareTrue(col2.containsTables(*t0));
    compareFalse(col2.containsTables(*t1));
    compareTrue(col2.containsTables(*t0, *t1));

    compareEQ(col0.name(), "col1");
    compareEQ(col0.fullName(), "myTable.col1");
    compareEQ(col0.toString(), "myTable.col1");
    compareEQ(col1.name(), "col2");
    compareEQ(col1.fullName(), "myTable.col2");
    compareEQ(col1.toString(), "myTable.col2");
    compareEQ(col2.name(), "col3");
    compareEQ(col2.fullName(), "myTable.col3");
    compareEQ(col2.toString(), "myTable.col3");
}
