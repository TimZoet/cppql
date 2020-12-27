set(NAME cppql_test)
set(TYPE test)
set(INCLUDE_DIR "include/cppql_test")
set(SRC_DIR "src")

set(HEADERS
	${INCLUDE_DIR}/test01.h
)
 
set(SOURCES
	${SRC_DIR}/test01.cpp
	${SRC_DIR}/main.cpp
)

set(DEPS_PRIVATE
    bettertest
	cppql
)

make_target(TYPE ${TYPE} NAME ${NAME} OUTDIR "tests" STARTUP HEADERS "${HEADERS}" SOURCES "${SOURCES}" DEPS_PRIVATE "${DEPS_PRIVATE}")
