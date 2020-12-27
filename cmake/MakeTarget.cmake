# cmake-template
# This file was last updated on 2020-12-26 (yyyy-mm-dd)

define_property(GLOBAL PROPERTY TARGETS_ALL
	BRIEF_DOCS "All targets"
    FULL_DOCS "All targets"
)
define_property(GLOBAL PROPERTY TARGETS_APPLICATIONS
	BRIEF_DOCS "Applications"
    FULL_DOCS "Applications"
)
define_property(GLOBAL PROPERTY TARGETS_EXAMPLES
	BRIEF_DOCS "Example targets"
    FULL_DOCS "Example targets. Building them can be made optional."
)
define_property(GLOBAL PROPERTY TARGETS_MODULES
	BRIEF_DOCS "Modules"
    FULL_DOCS "Modules"
)
define_property(GLOBAL PROPERTY TARGETS_TESTS
	BRIEF_DOCS "Tests"
    FULL_DOCS "Tests"
)
define_property(GLOBAL PROPERTY STARTUP_PROJECT
	BRIEF_DOCS "Startup project"
    FULL_DOCS "If set, will mark the target as startup project in Visual Studio"
)
define_property(TARGET PROPERTY FOLDER
	BRIEF_DOCS "Target folder"
    FULL_DOCS "Target folder"
)

set_property(GLOBAL PROPERTY TARGETS_ALL "")
set_property(GLOBAL PROPERTY TARGETS_APPLICATIONS "")
set_property(GLOBAL PROPERTY TARGETS_EXAMPLES "")
set_property(GLOBAL PROPERTY TARGETS_MODULES "")
set_property(GLOBAL PROPERTY TARGETS_TESTS "")

option(BUILD_EXAMPLES "Build example targets" OFF)

function(file_to_source_group name a b)
	# Replace backward slashes by forward slashes.
	string(REGEX REPLACE "\\\\" "\/" fixedname ${name})
	
	# Get relative path.
	if (IS_ABSOLUTE ${fixedname})
		file(RELATIVE_PATH relativename ${CMAKE_SOURCE_DIR} ${fixedname})
	else()
		set(relativename ${fixedname})
	endif()

	# Split into directories and filename.
	get_filename_component(dir ${relativename} DIRECTORY)
	get_filename_component(filename ${relativename} NAME)
	
	# Replace a by b.
	string(REGEX REPLACE ${a} ${b} src_grp ${dir})
	
	string(REGEX REPLACE "\/" "\\\\" src_grp2 ${src_grp})

	# Add to source group.
	source_group(${src_grp2} FILES ${name})
endfunction()

function(make_source_groups name headers sources)
	foreach(filename ${headers})
		file_to_source_group(${filename} "^include\/${name}" "include")
	endforeach()
	
	foreach(filename ${sources})
		file_to_source_group(${filename} "^src" "src")
	endforeach()
endfunction()

function(make_parse)
	cmake_parse_arguments(
        PARSED_ARGS
        "DYNAMIC;EXAMPLE;STARTUP;WARNINGS;WERROR"
        "NAME;TYPE;VERSION;FOLDER;OUTDIR"
        "HEADERS;SOURCES;DEPS_PUBLIC;DEPS_INTERFACE;DEPS_PRIVATE;INCS_PUBLIC;INCS_INTERFACE;INCS_PRIVATE"
        ${ARGN}
    )
	
	# Get linking mode.
	if(PARSED_ARGS_DYNAMIC)
		set(LINKING DYNAMIC PARENT_SCOPE)
	else()
		set(LINKING STATIC PARENT_SCOPE)
	endif()
	
	# Get example flag.
	if(PARSED_ARGS_EXAMPLE)
		set(EXAMPLE true PARENT_SCOPE)
	else()
		set(EXAMPLE false PARENT_SCOPE)
	endif()
	
	# Get startup flag.
	if(PARSED_ARGS_STARTUP)
		set(STARTUP true PARENT_SCOPE)
	else()
		set(STARTUP false PARENT_SCOPE)
	endif()
	
	# Get warnings flag.
	if(PARSED_ARGS_WARNINGS)
		set(WARNINGS true PARENT_SCOPE)
	else()
		set(WARNINGS false PARENT_SCOPE)
	endif()
	
	# Get warnings as error flag.
	if(PARSED_ARGS_WERROR)
		set(WERROR true PARENT_SCOPE)
	else()
		set(WERROR false PARENT_SCOPE)
	endif()
	
	# Get name.
    if(NOT PARSED_ARGS_NAME)
        message(FATAL_ERROR "No name provided")
    endif()
	set(NAME ${PARSED_ARGS_NAME} PARENT_SCOPE)

	# Get type.
    if(NOT PARSED_ARGS_TYPE)
        message(FATAL_ERROR "No target type provided")
    endif()
	set(VALID_TARGET_TYPES module application test)
	if(NOT PARSED_ARGS_TYPE IN_LIST VALID_TARGET_TYPES)
		message(FATAL_ERROR "Unknown target type ${PARSED_ARGS_TYPE}")
	endif()
	set(TYPE ${PARSED_ARGS_TYPE} PARENT_SCOPE)

	# Get version.
	if(NOT PARSED_ARGS_VERSION)
        message(STATUS "No version provided for ${PARSED_ARGS_NAME}, default to 1.0")
        set(VERSION "1.0" PARENT_SCOPE)
    else()
		set(VERSION ${PARSED_ARGS_VERSION} PARENT_SCOPE)
	endif()
	
	# Get folder.
	if(NOT PARSED_ARGS_FOLDER)
		if (PARSED_ARGS_EXAMPLE)
			set(FOLDER "examples/${PARSED_ARGS_TYPE}s" PARENT_SCOPE)
		else()
			set(FOLDER "${PARSED_ARGS_TYPE}s" PARENT_SCOPE)
		endif()
	else()
		set(FOLDER ${PARSED_ARGS_FOLDER} PARENT_SCOPE)
	endif()

	# Get output subfolder.
	if(NOT PARSED_ARGS_OUTDIR)
		set(OUTDIR "" PARENT_SCOPE)
	else()
		set(OUTDIR ${PARSED_ARGS_OUTDIR} PARENT_SCOPE)
	endif()
	
	# Get dependencies.
	if(NOT PARSED_ARGS_DEPS_PUBLIC)
		set(DEPS_PUBLIC "" PARENT_SCOPE)
	else()
		set(DEPS_PUBLIC "${PARSED_ARGS_DEPS_PUBLIC}" PARENT_SCOPE)
	endif()
	if(NOT PARSED_ARGS_DEPS_INTERFACE)
		set(DEPS_INTERFACE "" PARENT_SCOPE)
	else()
		set(DEPS_INTERFACE "${PARSED_ARGS_DEPS_INTERFACE}" PARENT_SCOPE)
	endif()
	if(NOT PARSED_ARGS_DEPS_PRIVATE)
		set(DEPS_PRIVATE "" PARENT_SCOPE)
	else()
		set(DEPS_PRIVATE "${PARSED_ARGS_DEPS_PRIVATE}" PARENT_SCOPE)
	endif()
	
	# Get additional include directories.
	if(NOT PARSED_ARGS_INCS_PUBLIC)
		set(INCS_PUBLIC "" PARENT_SCOPE)
	else()
		set(INCS_PUBLIC "${PARSED_ARGS_INCS_PUBLIC}" PARENT_SCOPE)
	endif()
	if(NOT PARSED_ARGS_INCS_INTERFACE)
		set(INCS_INTERFACE "" PARENT_SCOPE)
	else()
		set(INCS_INTERFACE "${PARSED_ARGS_INCS_INTERFACE}" PARENT_SCOPE)
	endif()
	if(NOT PARSED_ARGS_INCS_PRIVATE)
		set(INCS_PRIVATE "" PARENT_SCOPE)
	else()
		set(INCS_PRIVATE "${PARSED_ARGS_INCS_PRIVATE}" PARENT_SCOPE)
	endif()
	
	# Get headers and sources.
	if(NOT PARSED_ARGS_HEADERS)
		set(HEADERS "" PARENT_SCOPE)
		set(HEADER_COUNT 0 PARENT_SCOPE)
	else()
		set(HEADERS "${PARSED_ARGS_HEADERS}" PARENT_SCOPE)
		list(LENGTH PARSED_ARGS_HEADERS HEADER_COUNT)
		set(HEADER_COUNT ${HEADER_COUNT} PARENT_SCOPE)
	endif()
	if(NOT PARSED_ARGS_SOURCES)
		set(SOURCES "" PARENT_SCOPE)
		set(SOURCE_COUNT 0 PARENT_SCOPE)
	else()
		set(SOURCES "${PARSED_ARGS_SOURCES}" PARENT_SCOPE)
		list(LENGTH PARSED_ARGS_SOURCES SOURCE_COUNT)
		set(SOURCE_COUNT ${SOURCE_COUNT} PARENT_SCOPE)
	endif()
	
endfunction()

function(make_target)
	# Utility function for creating targets.
	# 
	# NAME (value): Name of the target.
	# TYPE (value): Type of the target. Currently supported types are:
	#    module: Creates a library target.
	#    application: Creates an executable target.
	#    test: Creates an executable target.
	# DYNAMIC (option): Enables dynamic linking for library targets. Otherwise 
	# target is linked statically.
	# STARTUP (option): Marks this target as startup project (if it is an 
	#   application).
	# EXAMPLE (option): Marks this target as an example. Adds an option to 
	#   disable building of this specific target.
	# WARNINGS (option) Enable highest warning level.
	# WERROR (option): Treat warnings as errors.
	# VERSION (value): Version of the target.
	# FOLDER (value): Folder the target is placed in. If not provided, targets 
	#   are placed in a folder based on their type.
	# OUTDIR (value): Subfolder the binaries are placed in. Default empty.
	# HEADERS (list): List of header files.
	# SOURCES (list): List of source files.
	# DEPS_PUBLIC (list): List of library targets that are linked with PUBLIC.
	# DEPS_PRIVATE (list): List of library targets that are linked with PRIVATE.
	# DEPS_INTERFACE (list): List of library targets that are linked with 
	#   INTERFACE.
	# INCS_PUBLIC (list): List of include directories that are added with 
	#   PUBLIC.
	# INCS_PRIVATE (list): List of include directories that are added with 
	#   PRIVATE.
	# INCS_INTERFACE (list): List of include directories that are added with
	#   INTERFACE.
	message(STATUS "start make_target()")
	list(APPEND CMAKE_MESSAGE_INDENT "  ")
	make_parse("${ARGV}")
	
	if(EXAMPLE)
		# Return if examples are globally disable.
		if(NOT BUILD_EXAMPLES)
			return()
		endif()
		# Add option for this specific target.
		option(BUILD_${NAME} "Build this example target" ON)
		if(NOT BUILD_${NAME})
			return()
		endif()
	endif()
	
    # Create target.
	message(STATUS "Making target ${NAME} (${TYPE}, v${VERSION}, example=${EXAMPLE}) with ${HEADER_COUNT} header and ${SOURCE_COUNT} source files")
	set_property(GLOBAL APPEND PROPERTY TARGETS_ALL ${NAME})
	if(TYPE STREQUAL "module")
		add_library(${NAME} ${LINKING} "${HEADERS}" "${SOURCES}")
		message(STATUS "${LINKING} linking")
		set_property(GLOBAL APPEND PROPERTY TARGETS_MODULES ${NAME})
	elseif(TYPE STREQUAL "application")
		add_executable(${NAME} "${HEADERS}" "${SOURCES}")
		set_property(GLOBAL APPEND PROPERTY TARGETS_APPLICATIONS ${NAME})
		if(STARTUP)
			set_property(GLOBAL PROPERTY STARTUP_PROJECT ${NAME})
		endif()
	elseif(TYPE STREQUAL "test")
		add_executable(${NAME} "${HEADERS}" "${SOURCES}")
		set_property(GLOBAL APPEND PROPERTY TARGETS_TESTS ${NAME})
	else()
		message(FATAL_ERROR "Unknown target type ${TYPE}")
	endif()
	
	# Add target to examples.
	if(EXAMPLE)
		set_property(GLOBAL APPEND PROPERTY TARGETS_EXAMPLES ${NAME})
	endif()

	# Set version and folder.
    set_target_properties(${NAME} PROPERTIES VERSION ${VERSION} FOLDER ${FOLDER})
	
	# Set warnings.
	if (WARNINGS)
		if (WERROR)
			target_compile_options(${NAME} PRIVATE
				$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
				$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic -Werror>
			)
		else()
			target_compile_options(${NAME} PRIVATE
				$<$<CXX_COMPILER_ID:MSVC>:/W4>
				$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic>
			)
		endif()
	endif()
    
    # Linking.
    message(STATUS "Linking with: PUBLIC [${DEPS_PUBLIC}] INTERFACE [${DEPS_INTERFACE}] PRIVATE [${DEPS_PRIVATE}]")
    target_link_libraries(
        ${NAME}
        PUBLIC ${DEPS_PUBLIC}
        INTERFACE ${DEPS_INTERFACE}
        PRIVATE ${DEPS_PRIVATE}
    )
	
	# Additional include directories.
    message(STATUS "Additional include directories: PUBLIC [${INCS_PUBLIC}] INTERFACE [${INCS_INTERFACE}] PRIVATE [${INCS_PRIVATE}]")
    target_include_directories(
        ${NAME}
        PUBLIC ${INCS_PUBLIC}
        INTERFACE ${INCS_INTERFACE}
        PRIVATE ${INCS_PRIVATE}
    )
    
    # Add own include directories to target.
    target_include_directories(${NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
	
	# Language.
	set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)

	# Output directories.
	set_target_properties(${NAME} PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${OUTDIR}"
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${OUTDIR}"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${OUTDIR}"
	)

    # Create source groups.
    make_source_groups(${NAME} "${HEADERS}" "${SOURCES}")
	
	list(POP_BACK CMAKE_MESSAGE_INDENT)
	message(STATUS "end make_target()")
endfunction(make_target)