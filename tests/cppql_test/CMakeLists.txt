find_package(bettertest REQUIRED)

set(NAME cppql_test)
set(TYPE test)
set(INCLUDE_DIR "include/cppql_test")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/aggregates/aggregate_average.h
    ${INCLUDE_DIR}/aggregates/aggregate_count.h
    ${INCLUDE_DIR}/aggregates/aggregate_max.h
    ${INCLUDE_DIR}/aggregates/aggregate_min.h
    ${INCLUDE_DIR}/aggregates/aggregate_sum.h
    ${INCLUDE_DIR}/aggregates/aggregate_total.h
    ${INCLUDE_DIR}/binding/bind.h
    ${INCLUDE_DIR}/binding/bind_blob.h
    ${INCLUDE_DIR}/binding/bind_int.h
    ${INCLUDE_DIR}/binding/bind_null.h
    ${INCLUDE_DIR}/binding/bind_real.h
    ${INCLUDE_DIR}/binding/bind_template.h
    ${INCLUDE_DIR}/binding/bind_text.h
    ${INCLUDE_DIR}/clauses/clause_columns.h
    ${INCLUDE_DIR}/clauses/clause_group_by.h
    ${INCLUDE_DIR}/clauses/clause_having.h
    ${INCLUDE_DIR}/clauses/clause_limit.h
    ${INCLUDE_DIR}/clauses/clause_on.h
    ${INCLUDE_DIR}/clauses/clause_order_by.h
    ${INCLUDE_DIR}/clauses/clause_union.h
    ${INCLUDE_DIR}/clauses/clause_using.h
    ${INCLUDE_DIR}/clauses/clause_where.h
    ${INCLUDE_DIR}/create_column/create_column_blob.h
    ${INCLUDE_DIR}/create_column/create_column_check.h
    ${INCLUDE_DIR}/create_column/create_column_collate.h
    ${INCLUDE_DIR}/create_column/create_column_default_value.h
    ${INCLUDE_DIR}/create_column/create_column_foreign_key.h
    ${INCLUDE_DIR}/create_column/create_column_int.h
    ${INCLUDE_DIR}/create_column/create_column_null.h
    ${INCLUDE_DIR}/create_column/create_column_primary_key.h
    ${INCLUDE_DIR}/create_column/create_column_primary_key_multiple.h
    ${INCLUDE_DIR}/create_column/create_column_real.h
    ${INCLUDE_DIR}/create_column/create_column_text.h
    ${INCLUDE_DIR}/create_column/create_column_unique.h
    ${INCLUDE_DIR}/database/database_create.h
    ${INCLUDE_DIR}/database/database_vacuum.h
    ${INCLUDE_DIR}/expressions/expression_aggregate.h
    ${INCLUDE_DIR}/expressions/expression_column.h
    ${INCLUDE_DIR}/expressions/expression_column_comparison.h
    ${INCLUDE_DIR}/expressions/expression_comparison.h
    ${INCLUDE_DIR}/expressions/expression_like.h
    ${INCLUDE_DIR}/expressions/expression_logical.h
    ${INCLUDE_DIR}/expressions/expression_order_by.h
    ${INCLUDE_DIR}/get_column/get_column_blob.h
    ${INCLUDE_DIR}/get_column/get_column_int.h
    ${INCLUDE_DIR}/get_column/get_column_real.h
    ${INCLUDE_DIR}/get_column/get_column_template.h
    ${INCLUDE_DIR}/get_column/get_column_text.h
    ${INCLUDE_DIR}/get_column/get_column_type.h
    ${INCLUDE_DIR}/queries/query_count.h
    ${INCLUDE_DIR}/queries/query_delete.h
    ${INCLUDE_DIR}/queries/query_insert.h
    ${INCLUDE_DIR}/queries/query_join.h
    ${INCLUDE_DIR}/queries/query_select.h
    ${INCLUDE_DIR}/queries/query_union.h
    ${INCLUDE_DIR}/queries/query_update.h
    ${INCLUDE_DIR}/statements/statement_count.h
    ${INCLUDE_DIR}/statements/statement_delete.h
    ${INCLUDE_DIR}/statements/statement_insert.h
    ${INCLUDE_DIR}/statements/statement_select.h
    ${INCLUDE_DIR}/statements/statement_select_one.h
    ${INCLUDE_DIR}/statements/statement_update.h
    ${INCLUDE_DIR}/table/create_table.h
    ${INCLUDE_DIR}/table/drop_table.h
    ${INCLUDE_DIR}/table/register_table.h
    ${INCLUDE_DIR}/typed_table/create_typed_table.h
    ${INCLUDE_DIR}/typed_table/create_typed_table_blob.h
    ${INCLUDE_DIR}/typed_table/create_typed_table_int.h
    ${INCLUDE_DIR}/typed_table/create_typed_table_real.h
    ${INCLUDE_DIR}/typed_table/create_typed_table_text.h

    ${INCLUDE_DIR}/statement_prepare.h
    ${INCLUDE_DIR}/statement_step.h
    ${INCLUDE_DIR}/transaction.h
    ${INCLUDE_DIR}/utils.h
)

set(SOURCES
    ${SRC_DIR}/aggregates/aggregate_average.cpp
    ${SRC_DIR}/aggregates/aggregate_count.cpp
    ${SRC_DIR}/aggregates/aggregate_max.cpp
    ${SRC_DIR}/aggregates/aggregate_min.cpp
    ${SRC_DIR}/aggregates/aggregate_sum.cpp
    ${SRC_DIR}/aggregates/aggregate_total.cpp
    ${SRC_DIR}/binding/bind.cpp
    ${SRC_DIR}/binding/bind_blob.cpp
    ${SRC_DIR}/binding/bind_int.cpp
    ${SRC_DIR}/binding/bind_null.cpp
    ${SRC_DIR}/binding/bind_real.cpp
    ${SRC_DIR}/binding/bind_template.cpp
    ${SRC_DIR}/binding/bind_text.cpp
    ${SRC_DIR}/clauses/clause_columns.cpp
    ${SRC_DIR}/clauses/clause_group_by.cpp
    ${SRC_DIR}/clauses/clause_having.cpp
    ${SRC_DIR}/clauses/clause_limit.cpp
    ${SRC_DIR}/clauses/clause_on.cpp
    ${SRC_DIR}/clauses/clause_order_by.cpp
    ${SRC_DIR}/clauses/clause_union.cpp
    ${SRC_DIR}/clauses/clause_using.cpp
    ${SRC_DIR}/clauses/clause_where.cpp
    ${SRC_DIR}/create_column/create_column_blob.cpp
    ${SRC_DIR}/create_column/create_column_check.cpp
    ${SRC_DIR}/create_column/create_column_collate.cpp
    ${SRC_DIR}/create_column/create_column_default_value.cpp
    ${SRC_DIR}/create_column/create_column_foreign_key.cpp
    ${SRC_DIR}/create_column/create_column_int.cpp
    ${SRC_DIR}/create_column/create_column_null.cpp
    ${SRC_DIR}/create_column/create_column_primary_key.cpp
    ${SRC_DIR}/create_column/create_column_primary_key_multiple.cpp
    ${SRC_DIR}/create_column/create_column_real.cpp
    ${SRC_DIR}/create_column/create_column_text.cpp
    ${SRC_DIR}/create_column/create_column_unique.cpp
    ${SRC_DIR}/database/database_create.cpp
    ${SRC_DIR}/database/database_vacuum.cpp
    ${SRC_DIR}/expressions/expression_aggregate.cpp
    ${SRC_DIR}/expressions/expression_column.cpp
    ${SRC_DIR}/expressions/expression_column_comparison.cpp
    ${SRC_DIR}/expressions/expression_comparison.cpp
    ${SRC_DIR}/expressions/expression_like.cpp
    ${SRC_DIR}/expressions/expression_logical.cpp
    ${SRC_DIR}/expressions/expression_order_by.cpp
    ${SRC_DIR}/get_column/get_column_blob.cpp
    ${SRC_DIR}/get_column/get_column_int.cpp
    ${SRC_DIR}/get_column/get_column_real.cpp
    ${SRC_DIR}/get_column/get_column_template.cpp
    ${SRC_DIR}/get_column/get_column_text.cpp
    ${SRC_DIR}/get_column/get_column_type.cpp
    ${SRC_DIR}/queries/query_count.cpp
    ${SRC_DIR}/queries/query_delete.cpp
    ${SRC_DIR}/queries/query_insert.cpp
    ${SRC_DIR}/queries/query_join.cpp
    ${SRC_DIR}/queries/query_select.cpp
    ${SRC_DIR}/queries/query_union.cpp
    ${SRC_DIR}/queries/query_update.cpp
    ${SRC_DIR}/statements/statement_count.cpp
    ${SRC_DIR}/statements/statement_delete.cpp
    ${SRC_DIR}/statements/statement_insert.cpp
    ${SRC_DIR}/statements/statement_select.cpp
    ${SRC_DIR}/statements/statement_select_one.cpp
    ${SRC_DIR}/statements/statement_update.cpp
    ${SRC_DIR}/table/create_table.cpp
    ${SRC_DIR}/table/drop_table.cpp
    ${SRC_DIR}/table/register_table.cpp
    ${SRC_DIR}/typed_table/create_typed_table.cpp
    ${SRC_DIR}/typed_table/create_typed_table_blob.cpp
    ${SRC_DIR}/typed_table/create_typed_table_int.cpp
    ${SRC_DIR}/typed_table/create_typed_table_real.cpp
    ${SRC_DIR}/typed_table/create_typed_table_text.cpp

    ${SRC_DIR}/main.cpp
    
    ${SRC_DIR}/statement_prepare.cpp
    ${SRC_DIR}/statement_step.cpp
    ${SRC_DIR}/transaction.cpp
    ${SRC_DIR}/utils.cpp
)

set(DEPS_PRIVATE
    bettertest::bettertest
	cppql
)

make_target(
    TYPE ${TYPE}
    NAME ${NAME}
    OUTDIR "tests"
    WARNINGS WERROR
    STARTUP
    HEADERS "${HEADERS}"
    SOURCES "${SOURCES}"
    DEPS_PUBLIC "${DEPS_PRIVATE}"
)
