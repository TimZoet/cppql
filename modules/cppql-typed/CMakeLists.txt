set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../cppql-core")
include(cppql_version_string)

set(NAME cppql-typed)
set(TYPE module)
set(INCLUDE_DIR "include/cppql-typed")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/typed_table.h
    ${INCLUDE_DIR}/expressions/expression.h
    ${INCLUDE_DIR}/expressions/expression_column.h
    ${INCLUDE_DIR}/expressions/expression_comparison.h
    ${INCLUDE_DIR}/expressions/expression_filter.h
    ${INCLUDE_DIR}/expressions/expression_like.h
    ${INCLUDE_DIR}/expressions/expression_limit.h
    ${INCLUDE_DIR}/expressions/expression_logical.h
    ${INCLUDE_DIR}/expressions/expression_order_by.h
    ${INCLUDE_DIR}/queries/count.h
    ${INCLUDE_DIR}/queries/delete.h
    ${INCLUDE_DIR}/queries/insert.h
    ${INCLUDE_DIR}/queries/select.h
    ${INCLUDE_DIR}/queries/select_one.h
    ${INCLUDE_DIR}/queries/update.h
)

set(SOURCES
    ${SRC_DIR}/lib.cpp
)

set(DEPS_PUBLIC
    cppql-core
)

make_target(
    NAME ${NAME}
    TYPE ${TYPE}
    VERSION ${CPPQL_VERSION}
    WARNINGS WERROR
    HEADERS "${HEADERS}"
    SOURCES "${SOURCES}"
    DEPS_PUBLIC "${DEPS_PUBLIC}"
)

install_target(
    NAME ${NAME}
    TYPE ${TYPE}
    VERSION ${CPPQL_VERSION}
    HEADERS "${HEADERS}"
)
