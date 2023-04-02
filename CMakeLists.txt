cmake_minimum_required(VERSION 3.24)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "_CMakePredefinedTargets")

################################################################################
# Project.
################################################################################

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}")
include(cppqlVersionString)
project(cppqlProject VERSION ${CPPQL_VERSION} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

################################################################################
# CMake modules.
################################################################################

set(TZ_CMAKE_MODULES_DIR "" CACHE PATH "Path to the cmake-modules package directory")
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_BINARY_DIR};${TZ_CMAKE_MODULES_DIR}")
include(InstallTarget)
include(ISA)
include(MakeManual)
include(MakeTarget)

################################################################################
# Options.
################################################################################

option(CPPQL_BIND_ZERO_BASED_INDICES "Use 0-based indices for all bind methods, instead of the default 1-based indices sqlite uses" ON)

################################################################################
# Add subdirectories.
################################################################################

add_subdirectory(buildtools)
add_subdirectory(modules)

if(BUILD_MANUAL)
    make_manual()
endif()
if (BUILD_TESTS)
    add_subdirectory(tests)
endif()
