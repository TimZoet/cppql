set(NAME hhdate)
set(TYPE module)
set(INCLUDE_DIR "include/hhdate")
set(SRC_DIR "src")

set(HEADERS
	${INCLUDE_DIR}/date.h
)

set(SOURCES
	${SRC_DIR}/liboutput.cpp
)

make_target(NAME ${NAME} TYPE ${TYPE} HEADERS "${HEADERS}" SOURCES "${SOURCES}")
