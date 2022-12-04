#include "cppql_test/statements/statement_insert.h"

#include "cppql/include_all.h"

using namespace std::string_literals;

namespace
{
    struct Foo
    {
        float   x;
        int64_t y;
    };

    [[nodiscard]] bool operator==(const Foo& lhs, const Foo& rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
}  // namespace

void StatementInsert::operator()()
{
    // Create table.
    sql::Table *t0, *t1, *t3, *t4;
    expectNoThrow([&] {
        t0 = &db->createTable("Table0");
        t0->createColumn("col1", sql::Column::Type::Int);
        t0->createColumn("col2", sql::Column::Type::Real);
        t0->createColumn("col3", sql::Column::Type::Text);
        t0->commit();

        t1 = &db->createTable("Table1");
        t1->createColumn("col1", sql::Column::Type::Int).setPrimaryKey(true).setAutoIncrement(true);
        t1->createColumn("col2", sql::Column::Type::Blob);
        t1->commit();

        t3 = &db->createTable("Table3");
        t3->createColumn("col1", sql::Column::Type::Int).setDefaultValue(10);
        t3->createColumn("col2", sql::Column::Type::Real).setDefaultValue(4.5f);
        t3->createColumn("col3", sql::Column::Type::Text).setDefaultValue("'abc'");
        t3->createColumn("col4", sql::Column::Type::Blob).setDefaultValue("X'FFAA5500'");
        t3->commit();

        t4 = &db->createTable("Table4");
        t4->createColumn("col1", sql::Column::Type::Int).setAutoIncrement(true).setPrimaryKey(true).setNotNull(true);
        t4->createColumn("col2", sql::Column::Type::Real);
        t4->createColumn("col3", sql::Column::Type::Text);
        t4->createColumn("col4", sql::Column::Type::Int);
        t4->commit();
    });
    const sql::TypedTable<int64_t, float, std::string>                       table0(*t0);
    const sql::TypedTable<int64_t, std::vector<uint32_t>>                    table1(*t1);
    const sql::TypedTable<int64_t, Foo>                                      table2(*t1);
    const sql::TypedTable<int32_t, float, std::string, std::vector<uint8_t>> table3(*t3);
    const sql::TypedTable<int64_t, float, std::string, int32_t>              table4(*t4);

    // Insert basic data.
    {
        const std::vector<std::tuple<int64_t, float, std::string>> rows = {
          {10, 20.0f, "abc"}, {20, 40.5f, "def"}, {30, 80.2f, "ghij"}, {40, 133.3f, "gh\0ij"}, {50, 99.9f, ""}};
        expectNoThrow([&] {
            auto insert = table0.insert().compile();
            for (const auto& r : rows) insert(std::get<0>(r), std::get<1>(r), sql::toStaticText(std::get<2>(r)));
        });

        // Retrieve rows.
        auto       sel   = table0.select().orderBy(ascending(table0.col<0>())).compile();
        const auto rows2 = decltype(rows)(sel.begin(), sel.end());
        compareEQ(rows, rows2);
    }

    // Insert blob data.
    {
        const std::vector<std::vector<uint32_t>> blobs = {
          {1, 2, 3, 4, 5}, {11, 22, 33, 44, 55}, {1'000, 10'000, 100'000, 1'000'000, 10'000'000}};
        const Foo foo{.x = 4.5f, .y = -10};
        expectNoThrow([&] {
            auto insert1 = table1.insert().compile();
            auto insert2 = table2.insert().compile();
            insert1(nullptr, sql::toStaticBlob(blobs[0]));
            insert1(nullptr, sql::toStaticBlob(blobs[1]));
            insert1(nullptr, sql::toStaticBlob(blobs[2]));
            insert2(nullptr, sql::toStaticBlob(foo));
        });

        // Retrieve vectors.
        auto sel0 = table1.selectAs<std::vector<uint32_t>>(table1.col<1>())
                      .where(table1.col<0>() <= 3)
                      .orderBy(ascending(table1.col<0>()))
                      .compile()
                      .bind(sql::BindParameters::All);
        const auto rows = std::vector<std::vector<uint32_t>>(sel0.begin(), sel0.end());
        compareEQ(blobs, rows);

        // Retrieve Foo.
        auto sel1 =
          table2.selectAs<Foo>(table2.col<1>()).where(table2.col<0>() == 4).compileOne().bind(sql::BindParameters::All);
        compareEQ(foo, sel1());
    }

    // Insert default values.
    {
        auto insert = table3.insert<>().compile();
        insert();

        auto                       row   = table3.select().compileOne()();
        const std::vector<uint8_t> bytes = {255, 170, 85, 0};
        compareEQ(10, std::get<0>(row));
        compareEQ(4.5f, std::get<1>(row));
        compareEQ(std::string("abc"), std::get<2>(row));
        compareEQ(bytes, std::get<3>(row));
    }

    // Insert nulls.
    {
        expectNoThrow([&] { table4.insert().compile()(nullptr, 11.0f, sql::toText("abc"), nullptr); });
        expectNoThrow([&] { table4.insert().compile()(nullptr, 12.0f, nullptr, 10); });
        expectNoThrow([&] { table4.insert().compile()(nullptr, nullptr, sql::toText("def"), 20); });
        expectNoThrow([&] { table4.insert<1, 2, 3>().compile()(13.0f, sql::toText("ghi"), 30); });
        expectNoThrow([&] { table4.insert<>().compile()(); });
        expectNoThrow([&] { table4.insert<3, 2, 1>().compile()(40, sql::toText("jkl"), 14.0f); });
        expectNoThrow([&] { table4.insert<1, 3>().compile()(15.0f, 50); });
        expectNoThrow([&] {
            constexpr std::optional<float>       f;
            constexpr std::optional<std::string> s;
            constexpr std::optional<int32_t>     i;
            table4.insert().compile()(nullptr, f, i, sql::toStaticText(s));
        });

        const auto stmt = db->createStatement("SELECT * FROM Table4;", true);

        compareTrue(stmt.step());
        compareEQ(stmt.column<int64_t>(0), 1);
        compareEQ(stmt.column<float>(1), 11.0f);
        compareEQ(stmt.column<std::string>(2), "abc"s);
        compareEQ(stmt.column<int32_t>(3), 0);
        compareEQ(stmt.columnType(3), sql::Column::Type::Null);

        compareTrue(stmt.step());
        compareEQ(stmt.column<int64_t>(0), 2);
        compareEQ(stmt.column<float>(1), 12.0f);
        compareEQ(stmt.column<std::string>(2), ""s);
        compareEQ(stmt.column<int32_t>(3), 10);
        compareEQ(stmt.columnType(2), sql::Column::Type::Null);

        compareTrue(stmt.step());
        compareEQ(stmt.column<int64_t>(0), 3);
        compareEQ(stmt.column<float>(1), 0.0f);
        compareEQ(stmt.column<std::string>(2), "def"s);
        compareEQ(stmt.column<int32_t>(3), 20);
        compareEQ(stmt.columnType(1), sql::Column::Type::Null);

        compareTrue(stmt.step());
        compareEQ(stmt.column<int64_t>(0), 4);
        compareEQ(stmt.column<float>(1), 13.0f);
        compareEQ(stmt.column<std::string>(2), "ghi"s);
        compareEQ(stmt.column<int32_t>(3), 30);

        compareTrue(stmt.step());
        compareEQ(stmt.column<int64_t>(0), 5);
        compareEQ(stmt.column<float>(1), 0.0f);
        compareEQ(stmt.column<std::string>(2), ""s);
        compareEQ(stmt.column<int32_t>(3), 0);
        compareEQ(stmt.columnType(1), sql::Column::Type::Null);
        compareEQ(stmt.columnType(2), sql::Column::Type::Null);
        compareEQ(stmt.columnType(3), sql::Column::Type::Null);

        compareTrue(stmt.step());
        compareEQ(stmt.column<int64_t>(0), 6);
        compareEQ(stmt.column<float>(1), 14.0f);
        compareEQ(stmt.column<std::string>(2), "jkl"s);
        compareEQ(stmt.column<int32_t>(3), 40);

        compareTrue(stmt.step());
        compareEQ(stmt.column<int64_t>(0), 7);
        compareEQ(stmt.column<float>(1), 15.0f);
        compareEQ(stmt.column<std::string>(2), ""s);
        compareEQ(stmt.column<int32_t>(3), 50);
        compareEQ(stmt.columnType(2), sql::Column::Type::Null);

        compareTrue(stmt.step());
        compareEQ(stmt.column<int64_t>(0), 8);
        compareEQ(stmt.column<float>(1), 0.0f);
        compareEQ(stmt.column<std::string>(2), ""s);
        compareEQ(stmt.column<int32_t>(3), 0);
        compareEQ(stmt.columnType(1), sql::Column::Type::Null);
        compareEQ(stmt.columnType(2), sql::Column::Type::Null);
        compareEQ(stmt.columnType(3), sql::Column::Type::Null);
    }
}
