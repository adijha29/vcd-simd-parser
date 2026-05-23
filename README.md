# 📂 `README.md`

# 🚀 VCD SIMD Parser

An ultra-high-performance, zero-copy VCD (Value Change Dump) lexing and parsing library written in modern C++17. By leveraging hardware-level **AVX2 (Advanced Vector Extensions)** instructions and OS memory-mapped files (`mmap` / `CreateFileMapping`), this engine bypasses traditional standard library bottleneck constraints to process text streams at near-memory-bandwidth speeds.

## 📊 Performance Benchmarks (64MB VCD File)

The metrics below highlight the real-world processing times of this library against traditional C++ streaming patterns using an EDA simulation file containing **23.7 Million Tokens** and **20.7 Million Lines**:

| Engine Variant | Operational Target | Internal Process Latency (`std::chrono`) | OS Kernel Execution Time | Speedup Multiplier |
| :--- | :--- | :--- | :--- | :--- |
| 🥇 **SIMD Line Parser** | Full-Line Ingestion | **771.94 ms** | 437 ms | **2.55x Faster** vs `std::getline` |
| 🥈 **SIMD Token Parser** | Word-Token Ingestion | **836.07 ms** | 453 ms | **5.66x Faster** vs `std::stringstream` |
| 🐢 **Scalar Line Baseline** | `std::getline` Buffer | **1,973.15 ms** | 578 ms | *Baseline* |
| 🐌 **Scalar Token Baseline** | Stream Allocation | **4,738.53 ms** | 640 ms | *Baseline* |

### Key Architectural Breakthroughs:
* **Zero Heap Allocations:** Individual tokens and rows are extracted as lightweight `std::string_view` windows (16-byte coordinate pairs) referencing the raw memory-mapped file natively, causing **0 bytes** of copy allocation thrashing.
* **Vectorized Character Swizzling:** Uses a single 256-bit register instruction (`_mm256_cmpeq_epi8`) to evaluate 32 bytes of text in a single CPU cycle, utilizing bitmask lookups (`_BitScanForward` / `__builtin_ctz`) to step directly between delimiters.
* **Noise Filtering:** Includes a high-speed vector validation bypass that safely discards blank lines and structural whitespace before vector insertions, eliminating vector resizing reallocations.


## 📁 Repository Structure

```text
vcd-simd-parser/
├── CMakeLists.txt                # Core root CMake build orchestration
├── include/                      # Public facing header file APIs
│   └── vcd_parser/
│       ├── file_mapper.hpp       # Zero-copy memory mapped file engine
│       └── simd_lexer.hpp        # AVX2 Vector tokenizer/line extractor API
├── src/                          # Internal library implementations
│   └── simd_lexer.cpp            # Hardware AVX2 vectorized implementation
├── apps/                         # Sandbox space for user apps/benchmarks
│   ├── CMakeLists.txt            # Executable build router
│   └── benchmark_runner.cpp      # High-precision 4-engine benchmark suite
├── run_matrix.ps1                # Automated Windows performance profiler script
└── run_matrix.sh                 # Automated Linux/macOS performance profiler script

```

---

## 🛠️ Build and Execution Requirements

Before compiling, ensure your target host system meets the following prerequisites:

* **Hardware:** Intel Haswell, AMD Excavator, or any newer x86_64 architecture with **AVX2 support enabled**.
* **Compiler:** GCC 9+, Clang 10+, or MSVC 2019+ (Supporting standard C++17 primitives).
* **Build Tools:** CMake 3.15 or newer.

---

## 🚀 Quick Start Compilation & Profiling Run

### 💻 On Windows (PowerShell)

You can build the static library targets and capture internal profiling counters natively with a single command via our automation matrix:

```powershell
# Execute the full matrix build and run loop automatically
.\run_matrix.ps1 -FilePath ".\split_chunk_1.vcd"

```

### 🐧 On Linux / macOS (Bash)

Provide execution permissions to the automated scripting target to compile and profile under UNIX environments:

```bash
chmod +x run_matrix.sh
./run_matrix.sh split_chunk_1.vcd

```

### 🏗️ Manual Step-by-Step Compilation

If you wish to integrate or compile the modules manually within your workflow structures, follow the standard CMake chain:

```bash
# Initialize build directory
mkdir build && cd build

# Generate Makefiles with Release optimizations
cmake -DCMAKE_BUILD_TYPE=Release ..

# Compile the library and applications simultaneously
cmake --build . --config Release

# Execute target binary manually
./apps/benchmark_runner ../split_chunk_1.vcd

```

---

## 💻 API Developer Usage Example

Integrating this parser into your downstream post-processing or telemetry wave tools is incredibly straightforward. Include the library headers and access the data layers directly:

```cpp
#include "vcd_parser/file_mapper.hpp"
#include "vcd_parser/simd_lexer.hpp"
#include <iostream>

int main() {
    try {
        // 1. Instantly map file contents into process memory spaces
        vcd::MappedFile vcd_file("simulation_dump.vcd");
        vcd::SIMDLexer lexer;

        // 2. Extract zero-copy line frames chronologically
        std::vector<std::string_view> lines = lexer.ExtractLines(vcd_file.data(), vcd_file.size());

        // 3. Process records at extreme performance speeds
        for (std::string_view line : lines) {
            if (line.rfind("$var", 0) == 0) {
                // Instantly filter out registration metrics line segments natively...
            }
        }
        
        std::cout << "Successfully parsed " << lines.size() << " non-empty VCD lines." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Parser Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

```

---

## 📜 License

This project is open-source and licensed under the **MIT License**. Feel free to use, modify, and distribute it across industrial, commercial, or academic workflows.

```

```