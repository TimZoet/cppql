#include "cppql_test/update/update_limit.h"

#include "cppql-typed/include_all.h"

using namespace std::string_literals;

void UpdateLimit::operator()()
{
    // This test cannot run because SQLITE_ENABLE_UPDATE_DELETE_LIMIT
    // is not enabled by default. This option is unfortunately not available
    // as an option for the Conan package.
    // Refer to https://www.sqlite.org/compile.html#enable_update_delete_limit.
#if 0
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

    // Insert several rows.
    auto insert = table.insert().compile();
    expectNoThrow([&insert] { insert(10, 20.0f, sql::toText("abc")); });
    expectNoThrow([&insert] { insert(20, 40.5f, sql::toText("def")); });
    expectNoThrow([&insert] { insert(30, 80.2f, sql::toText("ghij")); });

    {
        auto update = table.update<1>().orderBy(ascending(table.col<0>())).limitOffset(2, 0).compile();
        auto select = table.select<float, 1>().compile();

        expectNoThrow([&] { update(5.0f); });
        const std::vector<float> rows(select.begin(), select.end());
        compareEQ(rows.size(), static_cast<size_t>(3)).fatal("");
        compareEQ(5.0f, rows[0]);
        compareEQ(5.0f, rows[1]);
        compareEQ(80.2f, rows[2]);
    }

    {
        auto update = table.update<1>()
                        .where(table.col<0>() <= 20)
                        .orderBy(ascending(table.col<0>()))
                        .limitOffset(1, 1)
                        .compile()
                        .bind(sql::BindParameters::All);
        auto select = table.select<float, 1>().compile();

        expectNoThrow([&] { update(11.0f); });
        const std::vector<float> rows(select.begin(), select.end());
        compareEQ(rows.size(), static_cast<size_t>(3)).fatal("");
        compareEQ(5.0f, rows[0]);
        compareEQ(11.0f, rows[1]);
        compareEQ(80.2f, rows[2]);
    }

    {
        auto update = table.update<1>().orderBy(descending(table.col<0>())).limitOffset(2, 0).compile();
        auto select = table.select<float, 1>().compile();

        expectNoThrow([&] { update(13.0f); });
        const std::vector<float> rows(select.begin(), select.end());
        compareEQ(rows.size(), static_cast<size_t>(3)).fatal("");
        compareEQ(5.0f, rows[0]);
        compareEQ(13.0f, rows[1]);
        compareEQ(13.0f, rows[2]);
    }
#endif
}
