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
#include "cppql_test/create_typed_table_blob.h"
#include "cppql_test/create_typed_table_int.h"
#include "cppql_test/create_typed_table_real.h"
#include "cppql_test/create_typed_table_text.h"
#include "cppql_test/delete_rows.h"
#include "cppql_test/delete_rows_all.h"
#include "cppql_test/get_column_blob.h"
#include "cppql_test/get_column_int.h"
#include "cppql_test/get_column_real.h"
#include "cppql_test/get_column_text.h"
#include "cppql_test/insert.h"
#include "cppql_test/insert_blob.h"
#include "cppql_test/insert_default.h"
#include "cppql_test/insert_tuple.h"
#include "cppql_test/statement_prepare.h"
#include "cppql_test/statement_step.h"

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
            CreateTypedTableBlob,
            CreateTypedTableInt,
            CreateTypedTableReal,
            CreateTypedTableText,
            DeleteRows,
            DeleteRowsAll,
            GetColumnBlob,
            GetColumnInt,
            GetColumnReal,
            GetColumnText,
            Insert,
            InsertBlob,
            InsertDefault,
            InsertTuple,
            StatementPrepare,
            StatementStep>(argc, argv, "cppql");
    return 0;
}
