#include <iostream>

#include "bettertest/run.h"
#include "cppql_test/create_database.h"
#include "cppql_test/create_table.h"
#include "cppql_test/foreign_key.h"

#ifdef WIN32
#include "Windows.h"
#endif

int main(int argc, char** argv)
{
    // Set path next to executable.
#ifdef WIN32
    {
        WCHAR path[MAX_PATH];
        GetModuleFileNameW(NULL, path, MAX_PATH);
        std::filesystem::path workdir(path);
        const auto            p = workdir.parent_path();
        std::filesystem::current_path(p);
    }
#endif
    bt::run<test::CreateDatabase, test::CreateTable, test::ForeignKey>(argc, argv, "cppql");
    return 0;
}
