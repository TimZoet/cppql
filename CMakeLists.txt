cmake_minimum_required(VERSION 3.15)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "_CMakePredefinedTargets")

################################################################################
# Project.
################################################################################

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/modules/cppql")
include(cppqlVersionString)
project(cppqlProject VERSION ${CPPQL_VERSION} LANGUAGES CXX)
if(MSVC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.29.30129 AND CMAKE_VERSION VERSION_GREATER 3.20.3)
    set(CMAKE_CXX_STANDARD 23)
else()
    set(CMAKE_CXX_STANDARD 20)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED True)

################################################################################
# CMake modules.
################################################################################

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_BINARY_DIR};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(InstallTarget)
include(ISA)
include(MakeTarget)

################################################################################
# Add subdirectories.
################################################################################

add_subdirectory(modules)
if (BUILD_TESTS)
    add_subdirectory(tests)
endif()

################################################################################
# Set startup project.
################################################################################

get_property(STARTUP_PROJECT GLOBAL PROPERTY STARTUP_PROJECT)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${STARTUP_PROJECT})
