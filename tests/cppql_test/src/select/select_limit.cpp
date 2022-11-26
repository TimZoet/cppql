#include "cppql_test/select/select_limit.h"

#include "cppql/include_all.h"

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
    auto insert = table.insert().compile();
    expectNoThrow([&insert] {
        for (int64_t i = 0; i < 100; i++) insert(i);
    });

    // Try a bunch of different combinations of limit and offset for select with filter.

    {
        auto sel = table.selectAs<int64_t>()
                     .where(table.col<0>() >= static_cast<int64_t>(0))
                     .limitOffset(10, 0)
                     .compile()
                     .bind(sql::BindParameters::All);
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(10));
    }

    {
        auto sel = table.selectAs<int64_t>()
                     .where(table.col<0>() >= static_cast<int64_t>(0))
                     .limitOffset(33, 0)
                     .compile()
                     .bind(sql::BindParameters::All);
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(33));
    }

    {
        auto sel = table.selectAs<int64_t>()
                     .where(table.col<0>() >= static_cast<int64_t>(0))
                     .limitOffset(10, 10)
                     .compile()
                     .bind(sql::BindParameters::All);
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(10));
    }

    {
        auto sel = table.selectAs<int64_t>()
                     .where(table.col<0>() >= static_cast<int64_t>(0))
                     .limitOffset(33, 20)
                     .compile()
                     .bind(sql::BindParameters::All);
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(33));
    }

    {
        auto sel = table.selectAs<int64_t>()
                     .where(table.col<0>() >= static_cast<int64_t>(50))
                     .limitOffset(50, 0)
                     .compile()
                     .bind(sql::BindParameters::All);
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(50));
    }

    {
        auto sel = table.selectAs<int64_t>()
                     .where(table.col<0>() >= static_cast<int64_t>(50))
                     .limitOffset(60, 0)
                     .compile()
                     .bind(sql::BindParameters::All);
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(50));
    }

    {
        auto sel = table.selectAs<int64_t>()
                     .where(table.col<0>() >= static_cast<int64_t>(50))
                     .limitOffset(50, 10)
                     .compile()
                     .bind(sql::BindParameters::All);
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(40));
    }

    {
        auto sel = table.selectAs<int64_t>()
                     .where(table.col<0>() >= static_cast<int64_t>(50))
                     .limitOffset(50, 50)
                     .compile()
                     .bind(sql::BindParameters::All);
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(0));
    }

    // Try a bunch of different combinations of limit and offset for select without filter.

    {
        auto                       sel = table.selectAs<int64_t>().limitOffset(10, 0).compile();
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(10));
    }

    {
        auto                       sel = table.selectAs<int64_t>().limitOffset(33, 0).compile();
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(33));
    }

    {
        auto                       sel = table.selectAs<int64_t>().limitOffset(10, 10).compile();
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(10));
    }

    {
        auto                       sel = table.selectAs<int64_t>().limitOffset(33, 20).compile();
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(33));
    }

    {
        auto                       sel = table.selectAs<int64_t>().limitOffset(50, 0).compile();
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(50));
    }

    {
        auto                       sel = table.selectAs<int64_t>().limitOffset(50, 10).compile();
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(50));
    }

    {
        auto                       sel = table.selectAs<int64_t>().limitOffset(50, 60).compile();
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(40));
    }

    {
        auto                       sel = table.selectAs<int64_t>().limitOffset(-1, 60).compile();
        const std::vector<int64_t> rows(sel.begin(), sel.end());
        compareEQ(rows.size(), static_cast<size_t>(40));
    }
}
