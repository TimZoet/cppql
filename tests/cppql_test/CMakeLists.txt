set(NAME cppql_test)
set(TYPE test)
set(INCLUDE_DIR "include/cppql_test")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/create_column_blob.h
    ${INCLUDE_DIR}/create_column_foreign_key.h
    ${INCLUDE_DIR}/create_column_int.h
    ${INCLUDE_DIR}/create_column_null.h
    ${INCLUDE_DIR}/create_column_primary_key.h
    ${INCLUDE_DIR}/create_column_primary_key_multiple.h
    ${INCLUDE_DIR}/create_column_real.h
    ${INCLUDE_DIR}/create_column_text.h
    ${INCLUDE_DIR}/create_database.h
    ${INCLUDE_DIR}/create_table.h
    ${INCLUDE_DIR}/utils.h
)
 
set(SOURCES
    ${SRC_DIR}/create_column_blob.cpp
    ${SRC_DIR}/create_column_foreign_key.cpp
    ${SRC_DIR}/create_column_int.cpp
    ${SRC_DIR}/create_column_null.cpp
    ${SRC_DIR}/create_column_primary_key.cpp
    ${SRC_DIR}/create_column_primary_key_multiple.cpp
    ${SRC_DIR}/create_column_real.cpp
    ${SRC_DIR}/create_column_text.cpp
    ${SRC_DIR}/create_database.cpp
    ${SRC_DIR}/create_table.cpp
    
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/utils.cpp
)

set(DEPS_PRIVATE
    bettertest
	cppql
)

make_target(TYPE ${TYPE} NAME ${NAME} OUTDIR "tests" STARTUP HEADERS "${HEADERS}" SOURCES "${SOURCES}" DEPS_PRIVATE "${DEPS_PRIVATE}")
