#include "cppql_test/select/select_all.h"

#include "cppql-typed/typed_table.h"
#include "cppql-typed/queries/insert.h"
#include "cppql-typed/queries/select.h"

using namespace std::string_literals;

void SelectAll::operator()()
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
    sql::TypedTable<int64_t, float, std::string> table(*t);

    // Insert several rows.
    auto insert = table.insert();
    expectNoThrow([&insert]() { insert(10, 20.0f, "abc"s); });
    expectNoThrow([&insert]() { insert(20, 40.5f, "def"s); });
    expectNoThrow([&insert]() { insert(30, 80.2f, "ghij"s); });
    expectNoThrow([&insert]() { insert(40, 100.0f, "aaaa"s); });

    // Check rows.
    auto select = table.selectAll();
    std::vector<std::tuple<int64_t, float, std::string>> rows(select.begin(), select.end());
    compareEQ(rows.size(), static_cast<size_t>(4)).fatal("");
    compareEQ(10, std::get<0>(rows[0]));
    compareEQ(20.0f, std::get<1>(rows[0]));
    compareEQ("abc"s, std::get<2>(rows[0]));
    compareEQ(20, std::get<0>(rows[1]));
    compareEQ(40.5f, std::get<1>(rows[1]));
    compareEQ("def"s, std::get<2>(rows[1]));
    compareEQ(30, std::get<0>(rows[2]));
    compareEQ(80.2f, std::get<1>(rows[2]));
    compareEQ("ghij"s, std::get<2>(rows[2]));
    compareEQ(40, std::get<0>(rows[3]));
    compareEQ(100.0f, std::get<1>(rows[3]));
    compareEQ("aaaa"s, std::get<2>(rows[3]));
}
