#include <iostream>
#include <string>
#include "cppql/ext/delete.h"
#include "cppql/ext/insert.h"
#include "cppql/ext/select.h"
#include "cppql/ext/typed_table.h"

using namespace std::string_literals;

#ifdef WIN32
#include "Windows.h"
#endif

struct Foo
{
    Foo(int32_t val0, std::vector<float> val1) : v0(val0), v1(std::move(val1)) {}
    int32_t            v0;
    std::vector<float> v1;
};

int main(int, char**)
{
    // Set path next to executable.
#ifdef WIN32
    {
        WCHAR path[MAX_PATH];
        GetModuleFileNameW(nullptr, path, MAX_PATH);
        const std::filesystem::path workdir(path);
        const auto                  p = workdir.parent_path();
        std::filesystem::current_path(p);
    }
#endif

    // Create database and table.
    std::filesystem::remove(std::filesystem::current_path() / "db.db");
    auto  db    = sql::Database::create(std::filesystem::current_path() / "db.db");
    auto& table = db->createTable("myTable");
    table.createColumn<int32_t>("col1").setAutoIncrement(true).setPrimaryKey(true).setNotNull(true);
    table.createColumn<std::string>("col2");
    table.commit();

    sql::ext::TypedTable<int32_t, std::string> typedTable(table);

    auto col1 = typedTable.col<0>();
    auto col2 = typedTable.col<1>();

    //
    auto insert = typedTable.insert();
    insert(nullptr, "abc"s);
    insert(nullptr, "def"s);
    insert(nullptr, "ghi"s);

    auto select = typedTable.select<0, 1>(col1 > 0, true);
    auto vec    = std::vector<decltype(typedTable)::row_t>(select.begin(), select.end());

    //
    int32_t    id   = 1;
    const auto expr = col1 == &id;

    const auto del = typedTable.del(expr, true);
    for (const auto& row : vec)
    {
        id = std::get<0>(row);
        del();
    }

    return 0;
}
