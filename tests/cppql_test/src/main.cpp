#include <iostream>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "sqlite3.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/run.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql_test/aggregates/aggregate_average.h"
#include "cppql_test/aggregates/aggregate_count.h"
#include "cppql_test/aggregates/aggregate_max.h"
#include "cppql_test/aggregates/aggregate_min.h"
#include "cppql_test/aggregates/aggregate_sum.h"
#include "cppql_test/aggregates/aggregate_total.h"
#include "cppql_test/binding/bind.h"
#include "cppql_test/binding/bind_blob.h"
#include "cppql_test/binding/bind_int.h"
#include "cppql_test/binding/bind_null.h"
#include "cppql_test/binding/bind_real.h"
#include "cppql_test/binding/bind_template.h"
#include "cppql_test/binding/bind_text.h"
#include "cppql_test/clauses/clause_columns.h"
#include "cppql_test/clauses/clause_group_by.h"
#include "cppql_test/clauses/clause_having.h"
#include "cppql_test/clauses/clause_limit.h"
#include "cppql_test/clauses/clause_on.h"
#include "cppql_test/clauses/clause_order_by.h"
#include "cppql_test/clauses/clause_union.h"
#include "cppql_test/clauses/clause_using.h"
#include "cppql_test/clauses/clause_where.h"
#include "cppql_test/create_column/create_column_blob.h"
#include "cppql_test/create_column/create_column_default_value.h"
#include "cppql_test/create_column/create_column_foreign_key.h"
#include "cppql_test/create_column/create_column_int.h"
#include "cppql_test/create_column/create_column_null.h"
#include "cppql_test/create_column/create_column_primary_key.h"
#include "cppql_test/create_column/create_column_primary_key_multiple.h"
#include "cppql_test/create_column/create_column_real.h"
#include "cppql_test/create_column/create_column_text.h"
#include "cppql_test/database/database_create.h"
#include "cppql_test/database/database_vacuum.h"
#include "cppql_test/expressions/expression_aggregate.h"
#include "cppql_test/expressions/expression_column.h"
#include "cppql_test/expressions/expression_column_comparison.h"
#include "cppql_test/expressions/expression_comparison.h"
#include "cppql_test/expressions/expression_like.h"
#include "cppql_test/expressions/expression_logical.h"
#include "cppql_test/expressions/expression_order_by.h"
#include "cppql_test/get_column/get_column_blob.h"
#include "cppql_test/get_column/get_column_int.h"
#include "cppql_test/get_column/get_column_real.h"
#include "cppql_test/get_column/get_column_template.h"
#include "cppql_test/get_column/get_column_text.h"
#include "cppql_test/get_column/get_column_type.h"
#include "cppql_test/queries/query_count.h"
#include "cppql_test/queries/query_delete.h"
#include "cppql_test/queries/query_insert.h"
#include "cppql_test/queries/query_join.h"
#include "cppql_test/queries/query_select.h"
#include "cppql_test/queries/query_update.h"
#include "cppql_test/statements/statement_count.h"
#include "cppql_test/statements/statement_delete.h"
#include "cppql_test/statements/statement_insert.h"
#include "cppql_test/statements/statement_select.h"
#include "cppql_test/statements/statement_select_one.h"
#include "cppql_test/statements/statement_update.h"
#include "cppql_test/table/create_table.h"
#include "cppql_test/table/drop_table.h"
#include "cppql_test/table/register_table.h"
#include "cppql_test/typed_table/create_typed_table.h"
#include "cppql_test/typed_table/create_typed_table_blob.h"
#include "cppql_test/typed_table/create_typed_table_int.h"
#include "cppql_test/typed_table/create_typed_table_real.h"
#include "cppql_test/typed_table/create_typed_table_text.h"
#include "cppql_test/statement_prepare.h"
#include "cppql_test/statement_step.h"

#ifdef WIN32
#include "Windows.h"
#endif

int main(const int argc, char** argv)
{
    // Set path next to executable.
#ifdef WIN32
    {
        WCHAR path[MAX_PATH];
        GetModuleFileNameW(nullptr, path, MAX_PATH);
        const std::filesystem::path workdir(path);
        const auto                  p = workdir.parent_path();
        current_path(p);
    }
#endif

    bt::run<AggregateAverage,
            AggregateCount,
            AggregateMax,
            AggregateMin,
            AggregateSum,
            AggregateTotal,
            Bind,
            BindBlob,
            BindInt,
            BindNull,
            BindReal,
            BindTemplate,
            BindText,
            ClauseColumns,
            ClauseGroupBy,
            ClauseHaving,
            ClauseLimit,
            ClauseOn,
            ClauseOrderBy,
            ClauseUnion,
            ClauseUsing,
            ClauseWhere,
            CreateColumnBlob,
            CreateColumnDefaultValue,
            CreateColumnForeignKey,
            CreateColumnInt,
            CreateColumnNull,
            CreateColumnPrimaryKey,
            CreateColumnPrimaryKeyMultiple,
            CreateColumnReal,
            CreateColumnText,
            CreateTable,
            DatabaseCreate,
            DatabaseVacuum,
            DropTable,
            RegisterTable,
            CreateTypedTable,
            CreateTypedTableBlob,
            CreateTypedTableInt,
            CreateTypedTableReal,
            CreateTypedTableText,
            ExpressionAggregate,
            ExpressionColumn,
            ExpressionColumnComparison,
            ExpressionComparison,
            ExpressionLike,
            ExpressionLogical,
            ExpressionOrderBy,
            GetColumnBlob,
            GetColumnInt,
            GetColumnReal,
            GetColumnTemplate,
            GetColumnText,
            GetColumnType,
            QueryCount,
            QueryDelete,
            QueryInsert,
            QueryJoin,
            QuerySelect,
            QueryUpdate,
            StatementCount,
            StatementDelete,
            StatementInsert,
            StatementPrepare,
            StatementSelect,
            StatementSelectOne,
            StatementStep,
            StatementUpdate>(argc, argv, "cppql");

    return sqlite3_shutdown();
}
