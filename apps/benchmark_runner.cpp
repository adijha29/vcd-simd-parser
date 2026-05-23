#include "vcd_parser/file_mapper.hpp"
#include "vcd_parser/simd_lexer.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

void RunScalarTokenBenchmark(const std::string& path) {
    auto start = std::chrono::high_resolution_clock::now();
    std::ifstream file(path);
    if (!file.is_open()) return;

    std::vector<std::string> tokens; std::string line, token;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        while (ss >> token) tokens.push_back(token);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto d = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "--- Scalar Token Result ---\nTokens parsed (Scalar): " << tokens.size() 
              << "\nTime elapsed: " << d << " microseconds (" << d / 1000.0 << " ms)\n\n";
}

void RunScalarLineBenchmark(const std::string& path) {
    auto start = std::chrono::high_resolution_clock::now();
    std::ifstream file(path);
    if (!file.is_open()) return;

    std::vector<std::string> lines; std::string line;
    while (std::getline(file, line)) lines.push_back(line);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto d = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "--- Scalar Line Result ---\nLines parsed (Scalar): " << lines.size() 
              << "\nTime elapsed: " << d << " microseconds (" << d / 1000.0 << " ms)\n\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <vcd_file>\n";
        return 1;
    }
    std::string file_path = argv[1];

    try {
        // Consumer Interface calling our Custom Static Library
        vcd::MappedFile file(file_path);
        vcd::SIMDLexer lexer;

        // 1. Run SIMD Tokenizer
        auto s1 = std::chrono::high_resolution_clock::now();
        auto tokens = lexer.Tokenize(file.data(), file.size());
        auto e1 = std::chrono::high_resolution_clock::now();
        auto d1 = std::chrono::duration_cast<std::chrono::microseconds>(e1 - s1).count();
        std::cout << "--- SIMD Tokenizer Result ---\nTokens parsed (SIMD): " << tokens.size() 
                  << "\nTime elapsed: " << d1 << " microseconds (" << d1 / 1000.0 << " ms)\n\n";

        // 2. Run SIMD Line Parser
        auto s2 = std::chrono::high_resolution_clock::now();
        auto lines = lexer.ExtractLines(file.data(), file.size());
        auto e2 = std::chrono::high_resolution_clock::now();
        auto d2 = std::chrono::duration_cast<std::chrono::microseconds>(e2 - s2).count();
        std::cout << "--- SIMD Line Benchmark Result ---\nLines parsed (SIMD): " << lines.size() 
                  << "\nTime elapsed: " << d2 << " microseconds (" << d2 / 1000.0 << " ms)\n\n";

        // 3. Run Baselines
        RunScalarTokenBenchmark(file_path);
        RunScalarLineBenchmark(file_path);

    } catch (const std::exception& e) {
        std::cerr << "Exception encountered: " << e.what() << "\n";
        return 1;
    }
    return 0;
}