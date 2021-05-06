#include "cppql_test/select/select_order_by.h"

#include "cppql/ext/insert.h"
#include "cppql/ext/select.h"
#include "cppql/ext/typed_table.h"

using namespace std::string_literals;

void SelectOrderBy::operator()()
{
    // Create table.
    sql::Table* t;
    expectNoThrow([&t, this]() {
        t = &db->createTable("myTable");
        t->createColumn("col0", sql::Column::Type::Int);
        t->createColumn("col1", sql::Column::Type::Int);
        t->createColumn("col2", sql::Column::Type::Int);
        t->commit();
    });
    sql::ext::TypedTable<int64_t, int64_t, int64_t> table(*t);

    // Generate and insert a bunch of vals.
    std::vector<std::tuple<int64_t, int64_t, int64_t>> vals;
    for (int64_t i = 0; i < 100; i++) vals.emplace_back(i, -i, i % 10);
    auto insert = table.insert();
    expectNoThrow([&]() {
        for (const auto& r : vals) insert(r);
    });

    // col0 ASC
    {
        auto sel = table.selectAll(+table.col<0>());

        const std::vector<std::tuple<int64_t, int64_t, int64_t>> rows(sel.begin(), sel.end());
        std::sort(vals.begin(), vals.end(), [](const auto& lhs, const auto& rhs) {
            return std::get<0>(lhs) < std::get<0>(rhs);
        });
        compareEQ(vals, rows);
    }

    // col0 DESC
    {
        auto                                                     sel = table.selectAll(-table.col<0>());
        const std::vector<std::tuple<int64_t, int64_t, int64_t>> rows(sel.begin(), sel.end());
        std::sort(vals.begin(), vals.end(), [](const auto& lhs, const auto& rhs) {
            return std::get<0>(lhs) > std::get<0>(rhs);
        });
        compareEQ(vals, rows);
    }

    // col0 ASC, col1 ASC
    {
        auto sel = table.selectAll(+table.col<0>() + table.col<1>());
        const std::vector<std::tuple<int64_t, int64_t, int64_t>> rows(sel.begin(), sel.end());
        std::sort(vals.begin(), vals.end(), [](const auto& lhs, const auto& rhs) {
            return std::get<0>(lhs) < std::get<0>(rhs) ||
                   (std::get<0>(lhs) == std::get<0>(rhs) && std::get<1>(lhs) < std::get<1>(rhs));
        });
        compareEQ(vals, rows);
    }

    // col2 DESC, col1 DESC
    {
        auto sel = table.selectAll(-table.col<2>() - table.col<1>());
        const std::vector<std::tuple<int64_t, int64_t, int64_t>> rows(sel.begin(), sel.end());
        std::sort(vals.begin(), vals.end(), [](const auto& lhs, const auto& rhs) {
            return std::get<2>(lhs) > std::get<2>(rhs) ||
                   (std::get<2>(lhs) == std::get<2>(rhs) && std::get<1>(lhs) > std::get<1>(rhs));
        });
        compareEQ(vals, rows);
    }

    // col0 ASC, col1 DESC, col2 ASC
    {
        auto sel = table.selectAll(+table.col<2>() + (+table.col<0>() - table.col<1>()));
        const std::vector<std::tuple<int64_t, int64_t, int64_t>> rows(sel.begin(), sel.end());
        std::sort(vals.begin(), vals.end(), [](const auto& lhs, const auto& rhs) {
            if (std::get<0>(lhs) < std::get<0>(rhs)) return true;
            if (std::get<0>(lhs) > std::get<0>(rhs)) return false;
            if (std::get<1>(lhs) > std::get<1>(rhs)) return false;
            if (std::get<1>(lhs) < std::get<1>(rhs)) return true;
            if (std::get<2>(lhs) < std::get<2>(rhs)) return true;
            if (std::get<2>(lhs) > std::get<2>(rhs)) return false;
            return false;
        });
        compareEQ(vals, rows);
    }
}
