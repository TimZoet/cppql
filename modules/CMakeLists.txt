add_subdirectory(common)
add_subdirectory(cppql)
# Modules required just for tests.
if(BUILD_TESTS)
    add_subdirectory(bettertest)
    add_subdirectory(parsertongue)
endif()