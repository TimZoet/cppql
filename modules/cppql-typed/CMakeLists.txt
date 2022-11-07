set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../cppql-core")
include(cppql_version_string)

set(NAME cppql-typed)
set(TYPE module)
set(INCLUDE_DIR "include/cppql-typed")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/type_traits.h
    ${INCLUDE_DIR}/typed_table.h
    ${INCLUDE_DIR}/expressions/bind_parameters.h
    ${INCLUDE_DIR}/expressions/column_comparison_expression.h
    ${INCLUDE_DIR}/expressions/column_expression.h
    ${INCLUDE_DIR}/expressions/comparison_expression.h
    ${INCLUDE_DIR}/expressions/filter_expression.h
    ${INCLUDE_DIR}/expressions/like_expression.h
    ${INCLUDE_DIR}/expressions/limit_expression.h
    ${INCLUDE_DIR}/expressions/logical_expression.h
    ${INCLUDE_DIR}/expressions/order_by_expression.h
    ${INCLUDE_DIR}/expressions/single_filter_expression.h
    ${INCLUDE_DIR}/joins/inner_join.h
    ${INCLUDE_DIR}/joins/join_select.h
    ${INCLUDE_DIR}/joins/type_traits.h
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
