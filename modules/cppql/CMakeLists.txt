find_package(common REQUIRED)
# Need to explicitly specify path. For some reason, when the library is being
# built inside of the Conan cache, the find module that ships with CMake takes
# precendence.
find_package(SQLite3 PATHS ${CMAKE_BINARY_DIR} NO_DEFAULT_PATH REQUIRED)

include(cppqlVersionString)

option(CPPQL_BIND_ZERO_BASED_INDICES "Use 0-based indices for all bind methods, instead of the default 1-based indices sqlite uses" ON)

set(NAME cppql)
set(TYPE module)
set(INCLUDE_DIR "include/cppql")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/binding.h
    ${INCLUDE_DIR}/column.h
    ${INCLUDE_DIR}/database.h
    ${INCLUDE_DIR}/statement.h
    ${INCLUDE_DIR}/table.h
    ${INCLUDE_DIR}/ext/typed_table.h
    ${INCLUDE_DIR}/ext/expressions/expression.h
    ${INCLUDE_DIR}/ext/expressions/expression_column.h
    ${INCLUDE_DIR}/ext/expressions/expression_comparison.h
    ${INCLUDE_DIR}/ext/expressions/expression_filter.h
    ${INCLUDE_DIR}/ext/expressions/expression_like.h
    ${INCLUDE_DIR}/ext/expressions/expression_limit.h
    ${INCLUDE_DIR}/ext/expressions/expression_logical.h
    ${INCLUDE_DIR}/ext/expressions/expression_order_by.h
    ${INCLUDE_DIR}/ext/queries/count.h
    ${INCLUDE_DIR}/ext/queries/delete.h
    ${INCLUDE_DIR}/ext/queries/insert.h
    ${INCLUDE_DIR}/ext/queries/select.h
    ${INCLUDE_DIR}/ext/queries/select_one.h
    ${INCLUDE_DIR}/ext/queries/update.h
)

set(SOURCES
    ${SRC_DIR}/column.cpp
    ${SRC_DIR}/database.cpp
    ${SRC_DIR}/statement.cpp
    ${SRC_DIR}/table.cpp
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
    target_compile_definitions(${NAME} PRIVATE CPPQL_BIND_ZERO_BASED_INDICES)
endif()