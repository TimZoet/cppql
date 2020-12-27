set(NAME pugixml)
set(TYPE module)
set(INCLUDE_DIR "include/pugixml")
set(SRC_DIR "src")

set(HEADERS
	${INCLUDE_DIR}/pugiconfig.h
	${INCLUDE_DIR}/pugixml.h
)
 
set(SOURCES
	${SRC_DIR}/pugixml.cpp
)

make_target(NAME ${NAME} TYPE ${TYPE} HEADERS "${HEADERS}" SOURCES "${SOURCES}")
