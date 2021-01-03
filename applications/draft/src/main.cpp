#include <iostream>

#include "cppql/ext/select.h"
#include "cppql/ext/typed_table.h"

#ifdef WIN32
#include "Windows.h"
#endif

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
    table.createColumn("col1", sql::Column::Type::Int);
    table.createColumn("col2", sql::Column::Type::Real);
    table.createColumn("col3", sql::Column::Type::Text);
    table.createColumn("col4", sql::Column::Type::Int);
    table.commit();

    // Create typed table.
    sql::ext::TypedTable<int32_t, float, std::string, uint8_t> typedTable(table);
    using table_t   = decltype(typedTable);
    const auto col1 = table_t::col<0>();
    const auto col2 = table_t::col<1>();
    const auto col3 = table_t::col<2>();
    const auto col4 = table_t::col<3>();

    sql::ext::TypedTable<int32_t, float, std::string> typedTable2(table);

    auto       select = typedTable.select<0, 1>();
    const auto eq     = col1 == 10 || col4 == 20;
    const auto str    = eq.toString(table);
    return 0;
}
