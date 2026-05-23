# Create the benchmark runner executable binary
add_executable(benchmark_runner benchmark_runner.cpp)

# Link it directly against the library we created in the root directory
target_link_libraries(benchmark_runner PRIVATE vcd_parser)