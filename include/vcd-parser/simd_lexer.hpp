#ifndef VCD_PARSER_SIMD_LEXER_HPP
#define VCD_PARSER_SIMD_LEXER_HPP

#include <vector>
#include <string_view>

namespace vcd {

class SIMDLexer {
private:
    bool IsWhitespaceOnly(const char* start, size_t length) const;

public:
    SIMDLexer() = default;

    // Splits input buffer cleanly into isolated word-by-word token matrices
    std::vector<std::string_view> Tokenize(const char* data, size_t length) const;

    // Slices input buffer into full-line matrices (automatically drops whitespace lines)
    std::vector<std::string_view> ExtractLines(const char* data, size_t length) const;
};

} // namespace vcd

#endif