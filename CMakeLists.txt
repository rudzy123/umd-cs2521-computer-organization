cmake_minimum_required(VERSION 3.16)
project(cs2521_computer_organization LANGUAGES C ASM)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(FATAL_ERROR "This laboratory targets Linux.")
endif()
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|AMD64)$")
    message(FATAL_ERROR "This laboratory targets x86-64.")
endif()

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(cs2521 STATIC
    src/bits.c
    src/repr.c
    src/ref_ops.c
    src/locality.c
    src/sim.c
    assembly/add.S
    assembly/sum.S
    assembly/max.S
    assembly/factorial.S
)

target_include_directories(cs2521 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_options(cs2521 PRIVATE
    $<$<COMPILE_LANGUAGE:C>:-Wall>
    $<$<COMPILE_LANGUAGE:C>:-Wextra>
    $<$<COMPILE_LANGUAGE:C>:-Wpedantic>
    $<$<COMPILE_LANGUAGE:C>:-Werror>
)

# The locality loops are the experiment. -O2 keeps the comparison about memory
# access instead of about an unoptimized loop, including in a Debug build.
set_property(SOURCE src/locality.c APPEND PROPERTY COMPILE_OPTIONS -O2)

function(cs2521_program name source)
    add_executable(${name} ${source})
    target_link_libraries(${name} PRIVATE cs2521)
    target_compile_options(${name} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endfunction()

cs2521_program(cs2521_bits_demo src/demo_bits.c)
cs2521_program(cs2521_repr_demo src/demo_repr.c)
cs2521_program(cs2521_sim_demo src/demo_sim.c)
cs2521_program(cs2521_locality_demo src/demo_locality.c)

enable_testing()
foreach(test_name IN ITEMS test_bits test_repr test_ops test_sim test_locality)
    cs2521_program(${test_name} "tests/${test_name}.c")
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
