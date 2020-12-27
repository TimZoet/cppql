# cmake-template
# This file was last updated on 2020-08-23 (yyyy-mm-dd)

function(conan_pkg_utils)
    # Get profile and packages.
    cmake_parse_arguments(
        PARSED_ARGS
        ""
        "PROFILE"
        "PACKAGES"
        ${ARGN}
    )

    # Download conan.cmake to binary dir.
    if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
        message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
        file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
                        "${CMAKE_BINARY_DIR}/conan.cmake")
    endif()
    include(${CMAKE_BINARY_DIR}/conan.cmake)

    # Run conan.
    conan_cmake_run(
        REQUIRES ${PARSED_ARGS_PACKAGES}
        BASIC_SETUP
        CMAKE_TARGETS
        OPTIONS
        PROFILE ${PARSED_ARGS_PROFILE}
        BUILD missing
        GENERATORS cmake_find_package
    )
    
    # Append binary dir to module and prefix paths.
    set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH} PARENT_SCOPE)
    set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} ${CMAKE_PREFIX_PATH} PARENT_SCOPE)
endfunction()