set(NAME cppql_test)
set(TYPE test)
set(INCLUDE_DIR "include/cppql_test")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/binding/bind.h
    ${INCLUDE_DIR}/binding/bind_blob.h
    ${INCLUDE_DIR}/binding/bind_int.h
    ${INCLUDE_DIR}/binding/bind_null.h
    ${INCLUDE_DIR}/binding/bind_real.h
    ${INCLUDE_DIR}/binding/bind_template.h
    ${INCLUDE_DIR}/binding/bind_text.h
    ${INCLUDE_DIR}/create_column/create_column_blob.h
    ${INCLUDE_DIR}/create_column/create_column_default_value.h
    ${INCLUDE_DIR}/create_column/create_column_foreign_key.h
    ${INCLUDE_DIR}/create_column/create_column_int.h
    ${INCLUDE_DIR}/create_column/create_column_null.h
    ${INCLUDE_DIR}/create_column/create_column_primary_key.h
    ${INCLUDE_DIR}/create_column/create_column_primary_key_multiple.h
    ${INCLUDE_DIR}/create_column/create_column_real.h
    ${INCLUDE_DIR}/create_column/create_column_text.h
    ${INCLUDE_DIR}/delete/delete_rows.h
    ${INCLUDE_DIR}/delete/delete_rows_all.h
    ${INCLUDE_DIR}/get_column/get_column_blob.h
    ${INCLUDE_DIR}/get_column/get_column_int.h
    ${INCLUDE_DIR}/get_column/get_column_real.h
    ${INCLUDE_DIR}/get_column/get_column_template.h
    ${INCLUDE_DIR}/get_column/get_column_text.h
    ${INCLUDE_DIR}/get_column/get_column_type.h
    ${INCLUDE_DIR}/insert/insert.h
    ${INCLUDE_DIR}/insert/insert_blob.h
    ${INCLUDE_DIR}/insert/insert_default.h
    ${INCLUDE_DIR}/insert/insert_tuple.h
    ${INCLUDE_DIR}/select/select.h
    ${INCLUDE_DIR}/select/select_blob.h
    ${INCLUDE_DIR}/select/select_custom_return_type.h
    ${INCLUDE_DIR}/select/select_one.h
    ${INCLUDE_DIR}/select/select_reorder.h
    ${INCLUDE_DIR}/select/select_to_vector.h
    ${INCLUDE_DIR}/table/create_table.h
    ${INCLUDE_DIR}/table/drop_table.h
    ${INCLUDE_DIR}/table/register_table.h
    ${INCLUDE_DIR}/typed_table/create_typed_table.h
    ${INCLUDE_DIR}/typed_table/create_typed_table_blob.h
    ${INCLUDE_DIR}/typed_table/create_typed_table_int.h
    ${INCLUDE_DIR}/typed_table/create_typed_table_real.h
    ${INCLUDE_DIR}/typed_table/create_typed_table_text.h

    ${INCLUDE_DIR}/create_database.h
    ${INCLUDE_DIR}/statement_prepare.h
    ${INCLUDE_DIR}/statement_step.h
    ${INCLUDE_DIR}/utils.h
)

set(SOURCES
    ${SRC_DIR}/binding/bind.cpp
    ${SRC_DIR}/binding/bind_blob.cpp
    ${SRC_DIR}/binding/bind_int.cpp
    ${SRC_DIR}/binding/bind_null.cpp
    ${SRC_DIR}/binding/bind_real.cpp
    ${SRC_DIR}/binding/bind_template.cpp
    ${SRC_DIR}/binding/bind_text.cpp
    ${SRC_DIR}/create_column/create_column_blob.cpp
    ${SRC_DIR}/create_column/create_column_default_value.cpp
    ${SRC_DIR}/create_column/create_column_foreign_key.cpp
    ${SRC_DIR}/create_column/create_column_int.cpp
    ${SRC_DIR}/create_column/create_column_null.cpp
    ${SRC_DIR}/create_column/create_column_primary_key.cpp
    ${SRC_DIR}/create_column/create_column_primary_key_multiple.cpp
    ${SRC_DIR}/create_column/create_column_real.cpp
    ${SRC_DIR}/create_column/create_column_text.cpp
    ${SRC_DIR}/delete/delete_rows.cpp
    ${SRC_DIR}/delete/delete_rows_all.cpp
    ${SRC_DIR}/get_column/get_column_blob.cpp
    ${SRC_DIR}/get_column/get_column_int.cpp
    ${SRC_DIR}/get_column/get_column_real.cpp
    ${SRC_DIR}/get_column/get_column_template.cpp
    ${SRC_DIR}/get_column/get_column_text.cpp
    ${SRC_DIR}/insert/insert.cpp
    ${SRC_DIR}/insert/insert_blob.cpp
    ${SRC_DIR}/insert/insert_default.cpp
    ${SRC_DIR}/insert/insert_tuple.cpp
    ${SRC_DIR}/select/select.cpp
    ${SRC_DIR}/select/select_blob.cpp
    ${SRC_DIR}/select/select_custom_return_type.cpp
    ${SRC_DIR}/select/select_one.cpp
    ${SRC_DIR}/select/select_reorder.cpp
    ${SRC_DIR}/select/select_to_vector.cpp
    ${SRC_DIR}/table/create_table.cpp
    ${SRC_DIR}/table/drop_table.cpp
    ${SRC_DIR}/table/register_table.cpp
    ${SRC_DIR}/typed_table/create_typed_table.cpp
    ${SRC_DIR}/typed_table/create_typed_table_blob.cpp
    ${SRC_DIR}/typed_table/create_typed_table_int.cpp
    ${SRC_DIR}/typed_table/create_typed_table_real.cpp
    ${SRC_DIR}/typed_table/create_typed_table_text.cpp

    ${SRC_DIR}/create_database.cpp
    ${SRC_DIR}/main.cpp
    
    ${SRC_DIR}/statement_prepare.cpp
    ${SRC_DIR}/statement_step.cpp
    ${SRC_DIR}/utils.cpp
)

set(DEPS_PRIVATE
    bettertest
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
    DEPS_PRIVATE "${DEPS_PRIVATE}"
)
