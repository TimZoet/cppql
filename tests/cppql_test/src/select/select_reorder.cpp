#include "cppql_test/select/select_reorder.h"

#include "cppql-typed/typed_table.h"
#include "cppql-typed/queries/insert.h"
#include "cppql-typed/queries/select.h"

using namespace std::string_literals;

void SelectReorder::operator()()
{
    // Create table.
    sql::Table* t;
    expectNoThrow([&t, this] {
        t = &db->createTable("myTable");
        t->createColumn("col1", sql::Column::Type::Int);
        t->createColumn("col2", sql::Column::Type::Real);
        t->createColumn("col3", sql::Column::Type::Text);
        t->commit();
    });
    sql::TypedTable<int64_t, float, std::string> table(*t);

    // Insert single row.
    auto insert = table.insert();
    expectNoThrow([&insert] { insert(10, 20.0f, sql::toText("abc")); });

    // Select columns in different orders.
    auto sel0 = table.select<0, 1, 2>(table.col<0>() == 10, std::nullopt, std::nullopt, sql::BindParameters::All);
    auto sel1 = table.select<2, 1, 0>(table.col<0>() == 10, std::nullopt, std::nullopt, sql::BindParameters::All);
    auto sel2 = table.select<0, 0, 0>(table.col<0>() == 10, std::nullopt, std::nullopt, sql::BindParameters::All);
    auto sel3 = table.select<1, 1, 1>(table.col<0>() == 10, std::nullopt, std::nullopt, sql::BindParameters::All);
    auto sel4 = table.select<2, 2, 2>(table.col<0>() == 10, std::nullopt, std::nullopt, sql::BindParameters::All);
    auto sel5 = table.select<0, 1, 0, 1>(table.col<0>() == 10, std::nullopt, std::nullopt, sql::BindParameters::All);
    auto sel6 =
      table.select<2, 1, 0, 0, 1, 2>(table.col<0>() == 10, std::nullopt, std::nullopt, sql::BindParameters::All);
    auto sel7 = table.select<0>(table.col<0>() == 10, std::nullopt, std::nullopt, sql::BindParameters::All);
    auto sel8 = table.select<1>(table.col<0>() == 10, std::nullopt, std::nullopt, sql::BindParameters::All);
    auto sel9 = table.select<2>(table.col<0>() == 10, std::nullopt, std::nullopt, sql::BindParameters::All);

    // Get values.
    auto val0 = *sel0.begin();
    auto val1 = *sel1.begin();
    auto val2 = *sel2.begin();
    auto val3 = *sel3.begin();
    auto val4 = *sel4.begin();
    auto val5 = *sel5.begin();
    auto val6 = *sel6.begin();
    auto val7 = *sel7.begin();
    auto val8 = *sel8.begin();
    auto val9 = *sel9.begin();

    // Check all values.

    compareEQ(std::get<0>(val0), 10);
    compareEQ(std::get<1>(val0), 20.0f);
    compareEQ(std::get<2>(val0), "abc"s);

    compareEQ(std::get<0>(val1), "abc"s);
    compareEQ(std::get<1>(val1), 20.0f);
    compareEQ(std::get<2>(val1), 10);

    compareEQ(std::get<0>(val2), 10);
    compareEQ(std::get<1>(val2), 10);
    compareEQ(std::get<2>(val2), 10);

    compareEQ(std::get<0>(val3), 20.0f);
    compareEQ(std::get<1>(val3), 20.0f);
    compareEQ(std::get<2>(val3), 20.0f);

    compareEQ(std::get<0>(val4), "abc"s);
    compareEQ(std::get<1>(val4), "abc"s);
    compareEQ(std::get<2>(val4), "abc"s);

    compareEQ(std::get<0>(val5), 10);
    compareEQ(std::get<1>(val5), 20.0f);
    compareEQ(std::get<2>(val5), 10);
    compareEQ(std::get<3>(val5), 20.0f);

    compareEQ(std::get<0>(val6), "abc"s);
    compareEQ(std::get<1>(val6), 20.0f);
    compareEQ(std::get<2>(val6), 10);
    compareEQ(std::get<3>(val6), 10);
    compareEQ(std::get<4>(val6), 20.0f);
    compareEQ(std::get<5>(val6), "abc"s);

    compareEQ(std::get<0>(val7), 10);

    compareEQ(std::get<0>(val8), 20.0f);

    compareEQ(std::get<0>(val9), "abc"s);
}
