#include <iostream>
#include <string>
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
    /*table.createColumn<std::string>("col3");
    table.createColumn<uint8_t>("col4");*/
    table.commit();

    sql::ext::TypedTable<int32_t, std::string> typedTable(table);

    auto insert = typedTable.insert();
    insert(nullptr);
    insert(nullptr, std::string("def"));
    insert(nullptr, std::string("ghi"));
    /*insert(nullptr, sql::TransientText{.data = "abc", .size = 3});
    insert(nullptr, sql::TransientText{.data = "def", .size = 3});
    insert(nullptr, sql::TransientText{.data = "ghi", .size = 3});
    const auto row = std::make_tuple<std::nullptr_t, sql::TransientText>(nullptr, sql::TransientText{.data = "abc", .size = 3});
    insert(row);*/
    return 0;
}
