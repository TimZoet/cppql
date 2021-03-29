#include <iostream>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/run.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql_test/binding/bind.h"
#include "cppql_test/binding/bind_blob.h"
#include "cppql_test/binding/bind_int.h"
#include "cppql_test/binding/bind_null.h"
#include "cppql_test/binding/bind_real.h"
#include "cppql_test/binding/bind_template.h"
#include "cppql_test/binding/bind_text.h"
#include "cppql_test/create_column/create_column_blob.h"
#include "cppql_test/create_column/create_column_foreign_key.h"
#include "cppql_test/create_column/create_column_int.h"
#include "cppql_test/create_column/create_column_null.h"
#include "cppql_test/create_column/create_column_primary_key.h"
#include "cppql_test/create_column/create_column_primary_key_multiple.h"
#include "cppql_test/create_column/create_column_real.h"
#include "cppql_test/create_column/create_column_text.h"
#include "cppql_test/delete/delete_rows.h"
#include "cppql_test/delete/delete_rows_all.h"
#include "cppql_test/get_column/get_column_blob.h"
#include "cppql_test/get_column/get_column_int.h"
#include "cppql_test/get_column/get_column_real.h"
#include "cppql_test/get_column/get_column_template.h"
#include "cppql_test/get_column/get_column_text.h"
#include "cppql_test/insert/insert.h"
#include "cppql_test/insert/insert_blob.h"
#include "cppql_test/insert/insert_default.h"
#include "cppql_test/insert/insert_tuple.h"
#include "cppql_test/select/select.h"
#include "cppql_test/select/select_blob.h"
#include "cppql_test/select/select_custom_return_type.h"
#include "cppql_test/select/select_one.h"
#include "cppql_test/select/select_reorder.h"
#include "cppql_test/select/select_to_vector.h"
#include "cppql_test/table/create_table.h"
#include "cppql_test/table/drop_table.h"
#include "cppql_test/table/register_table.h"
#include "cppql_test/typed_table/create_typed_table.h"
#include "cppql_test/typed_table/create_typed_table_blob.h"
#include "cppql_test/typed_table/create_typed_table_int.h"
#include "cppql_test/typed_table/create_typed_table_real.h"
#include "cppql_test/typed_table/create_typed_table_text.h"

#include "cppql_test/create_database.h"

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
            DropTable,
            RegisterTable,
            CreateTypedTable,
            CreateTypedTableBlob,
            CreateTypedTableInt,
            CreateTypedTableReal,
            CreateTypedTableText,
            DeleteRows,
            DeleteRowsAll,
            GetColumnBlob,
            GetColumnInt,
            GetColumnReal,
            GetColumnTemplate,
            GetColumnText,
            Insert,
            InsertBlob,
            InsertDefault,
            InsertTuple,
            Select,
            SelectBlob,
            SelectCustomReturnType,
            SelectOne,
            SelectReorder,
            SelectToVector,
            StatementPrepare,
            StatementStep>(argc, argv, "cppql");
    return 0;
}
