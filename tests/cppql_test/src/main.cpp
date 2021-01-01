#include <iostream>

#include "bettertest/run.h"
#include "cppql_test/create_column_blob.h"
#include "cppql_test/create_column_foreign_key.h"
#include "cppql_test/create_column_int.h"
#include "cppql_test/create_column_null.h"
#include "cppql_test/create_column_primary_key.h"
#include "cppql_test/create_column_primary_key_multiple.h"
#include "cppql_test/create_column_real.h"
#include "cppql_test/create_column_text.h"
#include "cppql_test/create_database.h"
#include "cppql_test/create_table.h"

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
    bt::run<CreateColumnBlob,
            CreateColumnForeignKey,
            CreateColumnInt,
            CreateColumnNull,
            CreateColumnPrimaryKey,
            CreateColumnPrimaryKeyMultiple,
            CreateColumnReal,
            CreateColumnText,
            CreateDatabase,
            CreateTable>(argc, argv, "cppql");
    return 0;
}
