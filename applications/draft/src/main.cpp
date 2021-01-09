#include <iostream>

#include "cppql/ext/select.h"
#include "cppql/ext/typed_table.h"

#ifdef WIN32
#include "Windows.h"
#endif

struct Foo
{
    Foo(int32_t val0, std::vector<float> val1) : v0(val0), v1(std::move(val1)) {}
    int32_t            v0;
    std::vector<float> v1;
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
    table.createColumn<float*>("col2");
    /*table.createColumn<std::string>("col3");
    table.createColumn<uint8_t>("col4");*/
    table.commit();

    /*static_cast<void>(db->createStatement("INSERT INTO myTable VALUES (10, 10.5, 'abc', 1);", true).step());
    static_cast<void>(db->createStatement("INSERT INTO myTable VALUES (20, 20.5, 'def', 2);", true).step());
    static_cast<void>(db->createStatement("INSERT INTO myTable VALUES (30, 30.5, 'ghi', 3);", true).step());*/

    auto stmt = db->createStatement("INSERT INTO myTable VALUES (?, ?);", true);

    stmt.bindInt(0, 10);
    std::vector<float> vals{1, 2, 3, 4, 5};
    stmt.bindTransientBlob(1, vals.data(), sizeof(float) * vals.size());
    stmt.step();

    stmt.reset();

    stmt.bindInt(0, 20);
    vals.push_back(6);
    vals.push_back(7);
    vals.push_back(8);
    vals.push_back(9);
    vals.push_back(10);
    stmt.bindTransientBlob(1, vals.data(), sizeof(float) * vals.size());
    stmt.step();
    stmt.reset();

    // Create typed table.
    sql::ext::TypedTable<int32_t, float*> typedTable(table);
    using table_t   = decltype(typedTable);
    const auto col1 = table_t::col<0>();
    const auto col2 = table_t::col<1>();
    /*const auto col3 = table_t::col<2>();
    const auto col4 = table_t::col<3>();*/

    const auto exp = (col1 > 0);

    auto select = typedTable.select<0, 1>(exp);

    for (const auto row : select)
    {
        std::cout << std::get<0>(row) << "\n  ";
        for (const auto& f : std::get<1>(row)) std::cout << f << " ";
        std::cout << "\n";
    }

    return 0;
}
