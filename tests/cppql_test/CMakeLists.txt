set(NAME cppql_test)
set(TYPE test)
set(INCLUDE_DIR "include/cppql_test")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/create_database.h
    ${INCLUDE_DIR}/create_table.h
    ${INCLUDE_DIR}/foreign_key.h
)
 
set(SOURCES
    ${SRC_DIR}/create_database.cpp
    ${SRC_DIR}/create_table.cpp
    ${SRC_DIR}/foreign_key.cpp
	${SRC_DIR}/main.cpp
)

set(DEPS_PRIVATE
    bettertest
	cppql
)

make_target(TYPE ${TYPE} NAME ${NAME} OUTDIR "tests" STARTUP HEADERS "${HEADERS}" SOURCES "${SOURCES}" DEPS_PRIVATE "${DEPS_PRIVATE}")
