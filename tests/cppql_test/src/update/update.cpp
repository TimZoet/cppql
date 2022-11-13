#include "cppql_test/update/update.h"

#include "cppql-typed/include_all.h"

using namespace std::string_literals;

void Update::operator()()
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

    // Insert several rows.
    auto insert = table.insert()();
    expectNoThrow([&insert] { insert(10, 20.0f, sql::toText("abc")); });
    expectNoThrow([&insert] { insert(20, 40.5f, sql::toText("def")); });
    expectNoThrow([&insert] { insert(30, 80.2f, sql::toText("ghij")); });

    // Update one column at a time.
    {
        int64_t param  = 0;
        auto    update = table.update<1>().where(table.col<0>() == &param)(sql::BindParameters::None);
        auto    select = table.select<float, 1>().where(table.col<0>() == &param).one(sql::BindParameters::None);

        param = 10;
        expectNoThrow([&] { update(sql::BindParameters::All, 5.0f); });
        compareEQ(5.0f, select(sql::BindParameters::All));

        param = 10;
        expectNoThrow([&] { update(sql::BindParameters::All, nullptr); });
        compareEQ(0.0f, select(sql::BindParameters::All));

        param = 20;
        expectNoThrow([&] { update(sql::BindParameters::All, 15.0f); });
        compareEQ(15.0f, select(sql::BindParameters::All));

        param = 30;
        expectNoThrow([&] { update(sql::BindParameters::All, -10.0f); });
        compareEQ(-10.0f, select(sql::BindParameters::All));

        param = 30;
        constexpr std::optional<float> f;
        expectNoThrow([&] { update(sql::BindParameters::All, f); });
        compareEQ(0.0f, select(sql::BindParameters::All));
    }

    // Update multiple columns at a time.
    {
        int64_t param  = 0;
        auto    update = table.update<1, 2>().where(table.col<0>() == &param)(sql::BindParameters::None);
        auto    select = table.select<1, 2>().where(table.col<0>() == &param).one(sql::BindParameters::None);

        param = 10;
        expectNoThrow([&] { update(sql::BindParameters::All, 1000.0f, sql::toText("val0")); });
        auto row = select(sql::BindParameters::All);
        compareEQ(1000.0f, std::get<0>(row));
        compareEQ("val0"s, std::get<1>(row));

        param = 20;
        expectNoThrow([&] { update(sql::BindParameters::All, 444.0f, sql::toText("val1")); });
        row = select(sql::BindParameters::All);
        compareEQ(444.0f, std::get<0>(row));
        compareEQ("val1"s, std::get<1>(row));

        param = 30;
        expectNoThrow([&] { update(sql::BindParameters::All, -555.0f, sql::toText("val2")); });
        row = select(sql::BindParameters::All);
        compareEQ(-555.0f, std::get<0>(row));
        compareEQ("val2"s, std::get<1>(row));

        param = 30;
        constexpr std::optional<std::string> s;
        expectNoThrow([&] { update(sql::BindParameters::All, nullptr, sql::toStaticText(s)); });
        row = select(sql::BindParameters::All);
        compareEQ(0.0f, std::get<0>(row));
        compareEQ(""s, std::get<1>(row));
    }
}
