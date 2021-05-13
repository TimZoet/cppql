#include "cppql_test/update/update.h"

#include "cppql/ext/typed_table.h"
#include "cppql/ext/queries/insert.h"
#include "cppql/ext/queries/select_one.h"
#include "cppql/ext/queries/update.h"

using namespace std::string_literals;

void Update::operator()()
{
    // Create table.
    sql::Table* t;
    expectNoThrow([&t, this]() {
        t = &db->createTable("myTable");
        t->createColumn("col1", sql::Column::Type::Int);
        t->createColumn("col2", sql::Column::Type::Real);
        t->createColumn("col3", sql::Column::Type::Text);
        t->commit();
    });
    sql::ext::TypedTable<int64_t, float, std::string> table(*t);

    // Insert several rows.
    auto insert = table.insert();
    expectNoThrow([&insert]() { insert(10, 20.0f, "abc"s); });
    expectNoThrow([&insert]() { insert(20, 40.5f, "def"s); });
    expectNoThrow([&insert]() { insert(30, 80.2f, "ghij"s); });

    // Update one column at a time.
    {
        int64_t param  = 0;
        auto    update = table.update<1>(table.col<0>() == &param, false);
        auto    select = table.selectOne<float, 1>(table.col<0>() == &param, false);

        param = 10;
        expectNoThrow([&] { update(true, 5.0f); });
        compareEQ(5.0f, select(true));

        param = 20;
        expectNoThrow([&] { update(true, 15.0f); });
        compareEQ(15.0f, select(true));

        param = 30;
        expectNoThrow([&] { update(true, -10.0f); });
        compareEQ(-10.0f, select(true));
    }

    // Update multiple columns at a time.
    {
        int64_t param  = 0;
        auto    update = table.update<1, 2>(table.col<0>() == &param, false);
        auto    select = table.selectOne<1, 2>(table.col<0>() == &param, false);

        param = 10;
        expectNoThrow([&] { update(true, 1000.0f, "val0"s); });
        auto row = select(true);
        compareEQ(1000.0f, std::get<0>(row));
        compareEQ("val0"s, std::get<1>(row));

        param = 20;
        expectNoThrow([&] { update(true, 444.0f, "val1"s); });
        row = select(true);
        compareEQ(444.0f, std::get<0>(row));
        compareEQ("val1"s, std::get<1>(row));

        param = 30;
        expectNoThrow([&] { update(true, -555.0f, "val2"s); });
        row = select(true);
        compareEQ(-555.0f, std::get<0>(row));
        compareEQ("val2"s, std::get<1>(row));
    }
}
