#include "cppql_test/select/select_limit.h"

#include "cppql-typed/typed_table.h"
#include "cppql-typed/queries/insert.h"
#include "cppql-typed/queries/select.h"

using namespace std::string_literals;

void SelectLimit::operator()()
{
    // Create table.
    sql::Table* t;
    expectNoThrow([&t, this] {
        t = &db->createTable("myTable");
        t->createColumn("col1", sql::Column::Type::Int);
        t->commit();
    });
    sql::TypedTable<int64_t> table(*t);

    // Insert 100 rows.
    auto insert = table.insert();
    expectNoThrow([&insert] {
        for (int64_t i = 0; i < 100; i++) insert(i);
    });

    // Try a bunch of different combinations of limit and offset for select with filter.

    {
        auto                       sel = table.select<int64_t>(table.col<0>() >= static_cast<int64_t>(0),
                                         sql::LimitExpression{.limit = 10, .offset = 0},
                                         sql::BindParameters::All);
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(10));
    }

    {
        auto                       sel = table.select<int64_t>(table.col<0>() >= static_cast<int64_t>(0),
                                         sql::LimitExpression{.limit = 33, .offset = 0},
                                         sql::BindParameters::All);
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(33));
    }

    {
        auto                       sel = table.select<int64_t>(table.col<0>() >= static_cast<int64_t>(0),
                                         sql::LimitExpression{.limit = 10, .offset = 10},
                                         sql::BindParameters::All);
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(10));
    }

    {
        auto                       sel = table.select<int64_t>(table.col<0>() >= static_cast<int64_t>(0),
                                         sql::LimitExpression{.limit = 33, .offset = 20},
                                         sql::BindParameters::All);
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(33));
    }

    {
        auto                       sel = table.select<int64_t>(table.col<0>() >= static_cast<int64_t>(50),
                                         sql::LimitExpression{.limit = 50, .offset = 0},
                                         sql::BindParameters::All);
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(50));
    }

    {
        auto                       sel = table.select<int64_t>(table.col<0>() >= static_cast<int64_t>(50),
                                         sql::LimitExpression{.limit = 60, .offset = 0},
                                         sql::BindParameters::All);
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(50));
    }

    {
        auto                       sel = table.select<int64_t>(table.col<0>() >= static_cast<int64_t>(50),
                                         sql::LimitExpression{.limit = 50, .offset = 10},
                                         sql::BindParameters::All);
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(40));
    }

    {
        auto                       sel = table.select<int64_t>(table.col<0>() >= static_cast<int64_t>(50),
                                         sql::LimitExpression{.limit = 50, .offset = 50},
                                         sql::BindParameters::All);
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(0));
    }

    // Try a bunch of different combinations of limit and offset for select without filter.

    {
        auto                       sel = table.selectAll<int64_t>(sql::LimitExpression{.limit = 10, .offset = 0});
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(10));
    }

    {
        auto                       sel = table.selectAll<int64_t>(sql::LimitExpression{.limit = 33, .offset = 0});
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(33));
    }

    {
        auto                       sel = table.selectAll<int64_t>(sql::LimitExpression{.limit = 10, .offset = 10});
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(10));
    }

    {
        auto                       sel = table.selectAll<int64_t>(sql::LimitExpression{.limit = 33, .offset = 20});
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(33));
    }

    {
        auto                       sel = table.selectAll<int64_t>(sql::LimitExpression{.limit = 50, .offset = 0});
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(50));
    }

    {
        auto                       sel = table.selectAll<int64_t>(sql::LimitExpression{.limit = 50, .offset = 10});
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(50));
    }

    {
        auto                       sel = table.selectAll<int64_t>(sql::LimitExpression{.limit = 50, .offset = 60});
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(40));
    }

    {
        auto                       sel = table.selectAll<int64_t>(sql::LimitExpression{.limit = -1, .offset = 60});
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(40));
    }
}
