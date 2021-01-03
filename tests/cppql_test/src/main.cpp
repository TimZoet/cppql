#include <iostream>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/run.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql_test/bind.h"
#include "cppql_test/bind_blob.h"
#include "cppql_test/bind_int.h"
#include "cppql_test/bind_null.h"
#include "cppql_test/bind_real.h"
#include "cppql_test/bind_template.h"
#include "cppql_test/bind_text.h"
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
#include "cppql_test/typed_table.h"

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

    bt::run<Bind,
            BindBlob,
            BindInt,
            BindNull,
            BindReal,
            BindTemplate,
            BindText,
            CreateColumnBlob,
            CreateColumnForeignKey,
            CreateColumnInt,
            CreateColumnNull,
            CreateColumnPrimaryKey,
            CreateColumnPrimaryKeyMultiple,
            CreateColumnReal,
            CreateColumnText,
            CreateDatabase,
            CreateTable,
            TypedTable>(argc, argv, "cppql");
    return 0;
}
