# cmake-template
# This file was last updated on 2020-08-23 (yyyy-mm-dd)

cmake_minimum_required(VERSION 3.15)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "_CMakePredefinedTargets")

################################################################################
# Project.
################################################################################

project(cppqlProject VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

################################################################################
# CMake modules.
################################################################################

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(ISA)
include(MakeTarget)

################################################################################
# Conan.
################################################################################

option(USE_CONAN "If you are using the Conan package manager, enable this option." OFF)
if(USE_CONAN)
    include(ConanPkgUtils)
    conan_pkg_utils(PROFILE gcc10.1 PACKAGES fmt/7.0.3 nlohmann_json/3.9.1)
endif()

################################################################################
# Add subdirectories.
################################################################################

add_subdirectory(external)
add_subdirectory(modules)
add_subdirectory(tests)

################################################################################
# Set startup project.
################################################################################

get_property(STARTUP_PROJECT GLOBAL PROPERTY STARTUP_PROJECT)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${STARTUP_PROJECT})