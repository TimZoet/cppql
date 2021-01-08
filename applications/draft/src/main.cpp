#include <iostream>

#include "cppql/ext/select.h"
#include "cppql/ext/typed_table.h"

#ifdef WIN32
#include "Windows.h"
#endif

struct Foo
{
    Foo(int32_t val0) : v0(val0) {}
    int32_t v0;
};

int main(int argc, char** argv)
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
    table.createColumn<int32_t>("col1");
    table.createColumn<float>("col2");
    table.createColumn<std::string>("col3");
    table.createColumn<uint8_t>("col4");
    table.commit();

    static_cast<void>(db->createStatement("INSERT INTO myTable VALUES (10, 10, 'abc', 1);", true).step());
    static_cast<void>(db->createStatement("INSERT INTO myTable VALUES (20, 20, 'def', 2);", true).step());
    static_cast<void>(db->createStatement("INSERT INTO myTable VALUES (30, 30, 'ghi', 3);", true).step());

    // Create typed table.
    sql::ext::TypedTable<int32_t, float, std::string, uint8_t> typedTable(table);
    using table_t   = decltype(typedTable);
    const auto col1 = table_t::col<0>();
    const auto col2 = table_t::col<1>();
    const auto col3 = table_t::col<2>();
    const auto col4 = table_t::col<3>();

    int32_t    param1;
    float      param2;
    const auto exp = (col1 > &param1) && (col2 > &param2);
    
    auto select = typedTable.select<0>(exp);

    param1 = 0;
    param2 = 0;
    for (const auto row : select) { std::cout << std::get<0>(row) << std::endl; }

    return 0;
}
