cmake_minimum_required(VERSION 3.28)

project(BitForge-16 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

# ============================================================
# Dependencies
# ============================================================

include(FetchContent)

# -------------------------
# Catch2
# -------------------------

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.0
)

FetchContent_MakeAvailable(Catch2)


# -------------------------
# SDL3
# -------------------------

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.0
)

FetchContent_MakeAvailable(SDL3)


# ============================================================
# Include directories
# ============================================================

set(BITFORGE_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)


# ============================================================
# Hardware Tests
# ============================================================

add_executable(
    gates_test

    tests/hardware/gates_test.cpp

    src/hardware/gates.cpp
)

target_include_directories(
    gates_test
    PRIVATE
    ${BITFORGE_INCLUDE_DIR}
)

target_link_libraries(
    gates_test
    PRIVATE
    Catch2::Catch2WithMain
)

add_test(
    NAME gates_test
    COMMAND gates_test
)


# ------------------------------------------------------------

add_executable(
    arithmetic_test

    tests/hardware/arithmetic_test.cpp

    src/hardware/arithmetic.cpp
    src/hardware/gates.cpp
)

target_include_directories(
    arithmetic_test
    PRIVATE
    ${BITFORGE_INCLUDE_DIR}
)

target_link_libraries(
    arithmetic_test
    PRIVATE
    Catch2::Catch2WithMain
)

add_test(
    NAME arithmetic_test
    COMMAND arithmetic_test
)


# ------------------------------------------------------------

add_executable(
    mux_test

    tests/hardware/mux_test.cpp

    src/hardware/mux.cpp
    src/hardware/gates.cpp
)

target_include_directories(
    mux_test
    PRIVATE
    ${BITFORGE_INCLUDE_DIR}
)

target_link_libraries(
    mux_test
    PRIVATE
    Catch2::Catch2WithMain
)

add_test(
    NAME mux_test
    COMMAND mux_test
)


# ------------------------------------------------------------

add_executable(
    alu_test

    tests/hardware/alu_test.cpp

    src/hardware/alu.cpp
    src/hardware/gates.cpp
    src/hardware/arithmetic.cpp
    src/hardware/mux.cpp
)

target_include_directories(
    alu_test
    PRIVATE
    ${BITFORGE_INCLUDE_DIR}
)

target_link_libraries(
    alu_test
    PRIVATE
    Catch2::Catch2WithMain
)

add_test(
    NAME alu_test
    COMMAND alu_test
)


# ------------------------------------------------------------

add_executable(
    memory_test

    tests/hardware/memory_test.cpp

    src/hardware/memory.cpp
    src/hardware/gates.cpp
)

target_include_directories(
    memory_test
    PRIVATE
    ${BITFORGE_INCLUDE_DIR}
)

target_link_libraries(
    memory_test
    PRIVATE
    Catch2::Catch2WithMain
)

add_test(
    NAME memory_test
    COMMAND memory_test
)


# ------------------------------------------------------------

add_executable(
    registers_test

    tests/hardware/registers_test.cpp

    src/hardware/registers.cpp
    src/hardware/memory.cpp
    src/hardware/mux.cpp
    src/hardware/gates.cpp
)

target_include_directories(
    registers_test
    PRIVATE
    ${BITFORGE_INCLUDE_DIR}
)

target_link_libraries(
    registers_test
    PRIVATE
    Catch2::Catch2WithMain
)

add_test(
    NAME registers_test
    COMMAND registers_test
)


# ------------------------------------------------------------

add_executable(
    ram_test

    tests/hardware/ram_test.cpp

    src/hardware/ram.cpp
    src/hardware/registers.cpp
    src/hardware/memory.cpp
    src/hardware/mux.cpp
    src/hardware/gates.cpp
)

target_include_directories(
    ram_test
    PRIVATE
    ${BITFORGE_INCLUDE_DIR}
)

target_link_libraries(
    ram_test
    PRIVATE
    Catch2::Catch2WithMain
)

add_test(
    NAME ram_test
    COMMAND ram_test
)


# ------------------------------------------------------------

add_executable(
    pc_test

    tests/hardware/pc_test.cpp

    src/hardware/pc.cpp
    src/hardware/registers.cpp
    src/hardware/memory.cpp
    src/hardware/mux.cpp
    src/hardware/arithmetic.cpp
    src/hardware/gates.cpp
)

target_include_directories(
    pc_test
    PRIVATE
    ${BITFORGE_INCLUDE_DIR}
)

target_link_libraries(
    pc_test
    PRIVATE
    Catch2::Catch2WithMain
)

add_test(
    NAME pc_test
    COMMAND pc_test
)


# ------------------------------------------------------------

add_executable(
    memory_mapped_test

    tests/hardware/memory_mapped_test.cpp

    src/hardware/memory_mapped.cpp
    src/hardware/ram.cpp
    src/hardware/registers.cpp
    src/hardware/memory.cpp
    src/hardware/mux.cpp
    src/hardware/gates.cpp
)

target_include_directories(
    memory_mapped_test
    PRIVATE
    ${BITFORGE_INCLUDE_DIR}
)

target_link_libraries(
    memory_mapped_test
    PRIVATE
    Catch2::Catch2WithMain
)

add_test(
    NAME memory_mapped_test
    COMMAND memory_mapped_test
)


# ------------------------------------------------------------

add_executable(
    screen_key_test

    tests/display/screen_key_test.cpp

    src/display/screen_view.cpp
)

target_include_directories(
    screen_key_test
    PRIVATE
    ${BITFORGE_INCLUDE_DIR}
)

target_link_libraries(
    screen_key_test
    PRIVATE
    Catch2::Catch2WithMain
    SDL3::SDL3
)

add_test(
    NAME screen_key_test
    COMMAND screen_key_test
)


# ============================================================
# BitForge Applications
# ============================================================

add_executable(
    bitforge_simulator

    apps/simulator/main.cpp

    src/display/screen_view.cpp
    src/hardware/memory_mapped.cpp
    src/hardware/ram.cpp
    src/hardware/registers.cpp
    src/hardware/memory.cpp
    src/hardware/mux.cpp
    src/hardware/gates.cpp
)

target_include_directories(
    bitforge_simulator
    PRIVATE
    ${BITFORGE_INCLUDE_DIR}
)

target_link_libraries(
    bitforge_simulator
    PRIVATE
    SDL3::SDL3
)

# BitForge Assembler
# ============================================================

add_executable(
    bitforge_assembler

    apps/assembler/main.cpp

    src/asm/assembler.cpp
)

target_include_directories(
    bitforge_assembler
    PRIVATE
    ${BITFORGE_INCLUDE_DIR}
)

target_link_libraries(
    bitforge_assembler
    PRIVATE
)

add_test(
    NAME bitforge_assembler
    COMMAND bitforge_assembler
)
