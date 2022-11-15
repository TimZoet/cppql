#include "cppql_test/select/select_order_by_null.h"

#include "cppql-typed/include_all.h"

using namespace std::string_literals;

void SelectOrderByNull::operator()()
{
    // Create table.
    sql::Table* t;
    expectNoThrow([&t, this] {
        t = &db->createTable("myTable");
        t->createColumn("col0", sql::Column::Type::Int);
        t->createColumn("col1", sql::Column::Type::Int);
        t->commit();
    });
    sql::TypedTable<int64_t, int64_t> table(*t);

    // Generate and insert a bunch of vals.
    const std::vector<std::tuple<int64_t, int64_t>> vals   = {{1, 2}, {1, 1}, {2, 2}, {2, 1}, {3, 0}, {0, 3}};
    auto                                            insert = table.insert().compile();
    expectNoThrow([&] {
        insert(1, 1);
        insert(1, 2);
        insert(2, 1);
        insert(2, 2);
        insert(nullptr, 3);
        insert(3, nullptr);
    });

    // col0 ASC NULLS LAST, col1 DESC NULLS FIRST
    auto select =
      table.select()
        .orderBy(ascending(table.col<0>(), sql::Nulls::Last) + descending(table.col<1>(), sql::Nulls::First))
        .compile()
        .bind(sql::BindParameters::None);// TODO: Discard all bind(None)?
    const std::vector<std::tuple<int64_t, int64_t>> rows(select.begin(), select.end());
    compareEQ(rows.size(), static_cast<size_t>(6)).fatal("");
    compareEQ(vals, rows);
}
