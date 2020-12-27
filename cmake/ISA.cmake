# cmake-template
# This file was last updated on 2020-08-23 (yyyy-mm-dd)

#
#
#
#
################################################################
# Instruction set architectures for SIMD.
################################################################
#
#
#
#

# Minimum supported instruction set architecture.
set(ISA_MINIMUM "AVX2" CACHE STRING "Supported ISA")
set_property(CACHE ISA_MINIMUM PROPERTY STRINGS SSE4_1 SSE4_2 AVX AVX2)

# Support Fused Multiply and Add.
option(ISA_FMA "Use FMA" ON)

# Set preprocessor definitions based on above options.
if(ISA_MINIMUM STREQUAL "AVX2")
	add_compile_definitions(__SSE4_1__ __SSE4_2__ __AVX__ __AVX2__)
elseif(ISA_MINIMUM STREQUAL "AVX")
	add_compile_definitions(__SSE4_1__ __SSE4_2__ __AVX__)
elseif(ISA_MINIMUM STREQUAL "__SSE4_2__")
	add_compile_definitions(__SSE4_1__ __SSE4_2__)
elseif(ISA_MINIMUM STREQUAL "__SSE4_1__")
	add_compile_definitions(__SSE4_1__)
endif()
if(ISA_FMA)
	add_compile_definitions(__FMA__)
endif()