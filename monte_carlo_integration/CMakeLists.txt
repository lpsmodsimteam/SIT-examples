cmake_minimum_required(VERSION 3.12)
project(monte_carlo CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# add compiler warnings
add_compile_options(-Wall -Wextra -Wpedantic -faligned-new)

find_package(SIT CONFIG REQUIRED)
if (${SIT_FOUND})

    # -------------------- SST CONFIGS --------------------
    execute_process(
        COMMAND which sst-config
        OUTPUT_VARIABLE SST_CONFIG
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND ${SST_CONFIG} --ELEMENT_CXXFLAGS
        OUTPUT_VARIABLE SST_FLAGS
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND ${SST_CONFIG} --ELEMENT_LDFLAGS
        OUTPUT_VARIABLE SST_LDFLAGS
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    separate_arguments(SST_FLAGS)
    separate_arguments(SST_LDFLAGS)

    execute_process(
        COMMAND which sst-register
        OUTPUT_VARIABLE SST_REGISTER
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(
        COMMAND which sst
        OUTPUT_VARIABLE SST_BIN
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    macro(_sst_compile_link _name)
        if (TARGET ${_name})
            target_compile_definitions(${_name} PRIVATE)
            target_compile_options(${_name} PRIVATE ${SST_FLAGS})
            target_link_options(${_name} PRIVATE ${SST_LDFLAGS})
            execute_process(COMMAND ${SST_REGISTER} ${_name} ${_name}_LIBDIR=${CMAKE_BINARY_DIR})
        endif ()
    endmacro()

    message(STATUS ${CMAKE_BINARY_DIR})
    # -------------------- SST CONFIGS --------------------

    # -------------------- SYSTEMC CONFIGS --------------------
    set(CMAKE_PREFIX_PATH /opt/systemc)
    find_package(SystemCLanguage CONFIG REQUIRED)
    set(CMAKE_CXX_STANDARD ${SystemC_CXX_STANDARD})
    # -------------------- SYSTEMC CONFIGS --------------------
    # ------------------ SYSTEMC EXECUTABLES ------------------
    foreach (SYSC_MOD sc_cacc sc_div sc_sumsq)
        add_executable(${SYSC_MOD}.o ${PROJECT_SOURCE_DIR}/gen/${SYSC_MOD}_driver.cpp)
        target_link_libraries(${SYSC_MOD}.o SystemC::systemc)
    endforeach ()
    # ------------------ SYSTEMC EXECUTABLES ------------------

    # ------------------ VERILOG EXECUTABLES ------------------
    execute_process(
        COMMAND cp ${PROJECT_SOURCE_DIR}/gen/mt19937_driver.py .
        COMMAND make -f ${PROJECT_SOURCE_DIR}/gen/Makefile.config dumpconfig
    )
    # ------------------ VERILOG EXECUTABLES ------------------

    # -------------------- SST EXECUTABLES --------------------
    add_executable(
            libmonte_carlo.so
            monte_carlo.cpp
            ${PROJECT_SOURCE_DIR}/gen/sc_cacc_comp.cpp
            ${PROJECT_SOURCE_DIR}/gen/sc_div_comp.cpp
            ${PROJECT_SOURCE_DIR}/gen/sc_sumsq_comp.cpp
            ${PROJECT_SOURCE_DIR}/gen/mt19937_comp.cpp
    )
    _sst_compile_link(libmonte_carlo.so)
    # -------------------- SST EXECUTABLES --------------------

endif()
