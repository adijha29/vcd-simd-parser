cmake_minimum_required(VERSION 3.15)
project(VcdSimdParser VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force optimizations and SIMD vector lanes
if(MSVC)
    add_compile_options(/arch:AVX2 /O2)
else()
    add_compile_options(-O3 -mavx2)
endif()

# Build the custom parsing core as a Static Library Target
add_library(vcd_parser STATIC src/simd_lexer.cpp)

target_include_directories(vcd_parser PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_subdirectory(apps)