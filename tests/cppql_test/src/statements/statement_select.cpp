#include "cppql_test/statements/statement_select.h"

#include "cppql/include_all.h"

namespace
{
    struct Foo
    {
        int64_t a;
        float   b;
    };

    [[nodiscard]] bool operator==(const Foo& lhs, const Foo& rhs) noexcept { return lhs.a == rhs.a && lhs.b == rhs.b; }

}  // namespace

void StatementSelect::operator()()
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
        t1->createColumn("col2", sql::Column::Type::Blob);
        t1->createColumn("col3", sql::Column::Type::Blob);
        t1->createColumn("col4", sql::Column::Type::Blob);
        t1->commit();
    });
    const sql::TypedTable<int64_t, float, std::string>                          table0(*t0);
    const sql::TypedTable<int64_t, std::vector<int32_t>, Foo, std::vector<Foo>> table1(*t1);

    // Insert several rows.
    expectNoThrow([&] {
        auto insert = table0.insert().compile();
        insert(10, 20.0f, sql::toText("abc"));
        insert(20, 40.5f, sql::toText("def"));
        insert(30, 80.2f, sql::toText("ghij"));
        insert(40, 100.0f, sql::toText("aaaa"));
        insert(40, 200.0f, sql::toText("bbbb"));
    });

    // Select simple rows.
    {
        // Create select query.
        int64_t id  = 20;
        auto    sel = table0.select<0, 1, 2>().where(table0.col<0>() == &id).compile();

        // Select with unbound id.
        std::vector<std::tuple<int64_t, float, std::string>> vals(sel.begin(), sel.end());
        compareEQ(vals.size(), static_cast<size_t>(0));

        // Select with bound id 20.
        sel.bind(sql::BindParameters::All);
        vals.assign(sel.begin(), sel.end());
        compareEQ(vals.size(), static_cast<size_t>(1));
        compareEQ(vals[0], std::make_tuple<int64_t, float, std::string>(20, 40.5f, "def"));

        // Select with bound id 20.
        id = 30;
        vals.assign(sel.begin(), sel.end());
        compareEQ(vals.size(), static_cast<size_t>(1));
        compareEQ(vals[0], std::make_tuple<int64_t, float, std::string>(20, 40.5f, "def"));

        // Select with bound id 40.
        id = 40;
        sel.bind(sql::BindParameters::All);
        vals.assign(sel.begin(), sel.end());
        compareEQ(vals.size(), static_cast<size_t>(2));
        compareEQ(vals[0], std::make_tuple<int64_t, float, std::string>(40, 100.0f, "aaaa"));
        compareEQ(vals[1], std::make_tuple<int64_t, float, std::string>(40, 200.0f, "bbbb"));

        // Create select query and immediately bind.
        auto sel2 = table0.select<0, 1, 2>().where(table0.col<0>() == 30).compile().bind(sql::BindParameters::All);
        vals.assign(sel2.begin(), sel2.end());
        compareEQ(vals.size(), static_cast<size_t>(1));
        compareEQ(vals[0], std::make_tuple<int64_t, float, std::string>(30, 80.2f, "ghij"));
    }

    // Insert several rows.
    expectNoThrow([&] {
        auto insert = table1.insert().compile();
        {
            const std::vector      a = {0, 1, 2, 3};
            const Foo              b{.a = 10, .b = 5};
            const std::vector<Foo> c = {{.a = 20, .b = 30}, {.a = 40, .b = 50}};
            insert(1, sql::toStaticBlob(a), sql::toStaticBlob(b), sql::toStaticBlob(c));
        }
        {
            const std::vector      a = {-10, -11, -12, -13};
            const Foo              b{.a = -1000, .b = 0.5f};
            const std::vector<Foo> c = {{.a = 1000000, .b = 4.2f}, {.a = -100, .b = -1.0f}};
            insert(2, sql::toStaticBlob(a), sql::toStaticBlob(b), sql::toStaticBlob(c));
        }
    });

    // Select blobs.
    {
        // Create select.
        auto sel = table1.select<1, 2, 3>().where(table1.col<0>() > 0).compile().bind(sql::BindParameters::All);
        const std::vector<std::tuple<std::vector<int32_t>, Foo, std::vector<Foo>>> rows(sel.begin(), sel.end());

        // Check first row.
        compareEQ(std::get<0>(rows[0]), std::vector{0, 1, 2, 3});
        compareEQ(std::get<1>(rows[0]), Foo{.a = 10, .b = 5});
        compareEQ(std::get<2>(rows[0]), std::vector{Foo{.a = 20, .b = 30}, Foo{.a = 40, .b = 50}});

        // Check second row.
        compareEQ(std::get<0>(rows[1]), std::vector{-10, -11, -12, -13});
        compareEQ(std::get<1>(rows[1]), Foo{.a = -1000, .b = 0.5f});
        compareEQ(std::get<2>(rows[1]), std::vector{Foo{.a = 1000000, .b = 4.2f}, Foo{.a = -100, .b = -1.0f}});
    }

    // Select with incorrect reset.
    {
        // Create select query.
        int64_t val = 0;
        auto    sel = table0.selectAs<int64_t, 0>()
                     .where(table0.col<0>() > &val)
                     .orderBy(ascending(table0.col<0>()))
                     .compile()
                     .bind(sql::BindParameters::All);

        // Get a single row and discard iterator before all rows are returned...
        expectNoThrow([&sel] {
            auto it = sel.begin();
            static_cast<void>(++it);
        });

        // ...after which a bind should throw, because the statement was not reset.
        val = 25;
        expectThrow([&sel] { sel.bind(sql::BindParameters::All); });

        // Explicit reset should work again.
        expectNoThrow([&sel] { sel.reset().bind(sql::BindParameters::All); });
        const std::vector<int64_t> res(sel.begin(), sel.end());
        compareEQ(std::vector<int64_t>{30, 40, 40}, res);
    }
}
