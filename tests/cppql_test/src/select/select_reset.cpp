#include "cppql_test/select/select_reset.h"

#include "cppql/include_all.h"

using namespace std::string_literals;

void SelectReset::operator()()
{
    // Create table.
    sql::Table* t;
    expectNoThrow([&t, this] {
        t = &db->createTable("myTable");
        t->createColumn("col1", sql::Column::Type::Int);
        t->commit();
    });
    sql::TypedTable<int64_t> table(*t);

    // Insert several rows.
    auto insert = table.insert().compile();
    expectNoThrow([&insert] { insert(10); });
    expectNoThrow([&insert] { insert(20); });
    expectNoThrow([&insert] { insert(30); });
    expectNoThrow([&insert] { insert(40); });
    expectNoThrow([&insert] { insert(40); });

    // Create select query.
    int64_t val = 0;
    auto    sel = table.selectAs<int64_t>()
                 .where(table.col<0>() > &val)
                 .orderBy(ascending(table.col<0>()))
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
    compareEQ(res, std::vector<int64_t>{30, 40, 40});
}
