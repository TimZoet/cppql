#include "cppql_test/update/update.h"

#include "cppql/include_all.h"

using namespace std::string_literals;

void Update::operator()()
{
    // TODO: Add tests that don't just use the indexed methods.
    // Add test for empty parameter list that should generate statement to update all columns.
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

    // Update one column at a time.
    {
        int64_t param  = 0;
        auto    update = table.update<1>().where(table.col<0>() == &param).compile();
        auto    select = table.selectAs<float, 1>().where(table.col<0>() == &param).compileOne();

        param = 10;
        expectNoThrow([&] { update.bind(sql::BindParameters::All)(5.0f); });
        compareEQ(5.0f, select.bind(sql::BindParameters::All)());

        param = 10;
        expectNoThrow([&] {
            update.bind(sql::BindParameters::All)(nullptr);
        });
        compareEQ(0.0f, select.bind(sql::BindParameters::All)());

        param = 20;
        expectNoThrow([&] { update.bind(sql::BindParameters::All)(15.0f); });
        compareEQ(15.0f, select.bind(sql::BindParameters::All)());

        param = 30;
        expectNoThrow([&] { update.bind(sql::BindParameters::All)(-10.0f); });
        compareEQ(-10.0f, select.bind(sql::BindParameters::All)());

        param = 30;
        constexpr std::optional<float> f;
        expectNoThrow([&] { update.bind(sql::BindParameters::All)(f); });
        compareEQ(0.0f, select.bind(sql::BindParameters::All)());
    }

    // Update multiple columns at a time.
    {
        int64_t param  = 0;
        auto    update = table.update<1, 2>().where(table.col<0>() == &param).compile();
        auto    select = table.select<1, 2>().where(table.col<0>() == &param).compileOne();

        param = 10;
        expectNoThrow([&] { update.bind(sql::BindParameters::All)(1000.0f, sql::toText("val0")); });
        auto row = select.bind(sql::BindParameters::All)();
        compareEQ(1000.0f, std::get<0>(row));
        compareEQ("val0"s, std::get<1>(row));

        param = 20;
        expectNoThrow([&] { update.bind(sql::BindParameters::All)(444.0f, sql::toText("val1")); });
        row = select.bind(sql::BindParameters::All)();
        compareEQ(444.0f, std::get<0>(row));
        compareEQ("val1"s, std::get<1>(row));

        param = 30;
        expectNoThrow([&] { update.bind(sql::BindParameters::All)(-555.0f, sql::toText("val2")); });
        row = select.bind(sql::BindParameters::All)();
        compareEQ(-555.0f, std::get<0>(row));
        compareEQ("val2"s, std::get<1>(row));

        param = 30;
        constexpr std::optional<std::string> s;
        expectNoThrow([&] { update.bind(sql::BindParameters::All)(nullptr, sql::toStaticText(s)); });
        row = select.bind(sql::BindParameters::All)();
        compareEQ(0.0f, std::get<0>(row));
        compareEQ(""s, std::get<1>(row));
    }
}
