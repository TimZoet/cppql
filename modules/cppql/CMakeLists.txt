find_package(common REQUIRED)
# Need to explicitly specify path. For some reason, when the library is being
# built inside of the Conan cache, the find module that ships with CMake takes
# precendence.
find_package(SQLite3 PATHS ${CMAKE_BINARY_DIR} NO_DEFAULT_PATH REQUIRED)

include(cppql_version_string)

option(CPPQL_BIND_ZERO_BASED_INDICES "Use 0-based indices for all bind methods, instead of the default 1-based indices sqlite uses" ON)

set(NAME cppql)
set(TYPE module)
set(INCLUDE_DIR "include/cppql")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/include_all.h
    ${INCLUDE_DIR}/aggregates/average_aggregate.h
    ${INCLUDE_DIR}/aggregates/count_aggregate.h
    ${INCLUDE_DIR}/aggregates/max_aggregate.h
    ${INCLUDE_DIR}/aggregates/min_aggregate.h
    ${INCLUDE_DIR}/aggregates/sum_aggregate.h
    ${INCLUDE_DIR}/aggregates/total_aggregate.h
    ${INCLUDE_DIR}/clauses/columns.h
    ${INCLUDE_DIR}/clauses/fwd.h
    ${INCLUDE_DIR}/clauses/group_by.h
    ${INCLUDE_DIR}/clauses/having.h
    ${INCLUDE_DIR}/clauses/limit.h
    ${INCLUDE_DIR}/clauses/on.h
    ${INCLUDE_DIR}/clauses/order_by.h
    ${INCLUDE_DIR}/clauses/union.h
    ${INCLUDE_DIR}/clauses/using.h
    ${INCLUDE_DIR}/clauses/where.h
    ${INCLUDE_DIR}/core/binding.h
    ${INCLUDE_DIR}/core/column.h
    ${INCLUDE_DIR}/core/database.h
    ${INCLUDE_DIR}/core/statement.h
    ${INCLUDE_DIR}/core/table.h
    ${INCLUDE_DIR}/error/cppql_error.h
    ${INCLUDE_DIR}/error/sqlite_error.h
    ${INCLUDE_DIR}/expressions/aggregate_expression.h
    ${INCLUDE_DIR}/expressions/base_filter_expression.h
    ${INCLUDE_DIR}/expressions/column_comparison_expression.h
    ${INCLUDE_DIR}/expressions/column_expression.h
    ${INCLUDE_DIR}/expressions/comparison_expression.h
    ${INCLUDE_DIR}/expressions/filter_expression.h
    ${INCLUDE_DIR}/expressions/like_expression.h
    ${INCLUDE_DIR}/expressions/logical_expression.h
    ${INCLUDE_DIR}/expressions/order_by_expression.h
    ${INCLUDE_DIR}/queries/count_query.h
    ${INCLUDE_DIR}/queries/delete_query.h
    ${INCLUDE_DIR}/queries/fwd.h
    ${INCLUDE_DIR}/queries/insert_query.h
    ${INCLUDE_DIR}/queries/select_query.h
    ${INCLUDE_DIR}/queries/update_query.h
    ${INCLUDE_DIR}/statements/count_statement.h
    ${INCLUDE_DIR}/statements/delete_statement.h
    ${INCLUDE_DIR}/statements/fwd.h
    ${INCLUDE_DIR}/statements/insert_statement.h
    ${INCLUDE_DIR}/statements/select_statement.h
    ${INCLUDE_DIR}/statements/select_one_statement.h
    ${INCLUDE_DIR}/statements/update_statement.h
    ${INCLUDE_DIR}/typed/enums.h
    ${INCLUDE_DIR}/typed/fwd.h
    ${INCLUDE_DIR}/typed/join.h
    ${INCLUDE_DIR}/typed/join_type.h
    ${INCLUDE_DIR}/typed/typed_table.h
)

set(SOURCES
    ${SRC_DIR}/core/column.cpp
    ${SRC_DIR}/core/database.cpp
    ${SRC_DIR}/core/statement.cpp
    ${SRC_DIR}/core/table.cpp
    ${SRC_DIR}/error/cppql_error.cpp
    ${SRC_DIR}/error/sqlite_error.cpp
)

set(DEPS_PUBLIC
    common::common
    SQLite::SQLite3
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

if(CPPQL_BIND_ZERO_BASED_INDICES)
    target_compile_definitions(${NAME} PUBLIC CPPQL_BIND_ZERO_BASED_INDICES)
endif()