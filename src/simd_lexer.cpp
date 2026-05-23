#include "vcd_parser/simd_lexer.hpp"
#include <immintrin.h>
#include <cstdint>

#if defined(_WIN32) || defined(_WIN64)
    #include <intrin.h>
#endif

namespace vcd {

bool SIMDLexer::IsWhitespaceOnly(const char* start, size_t length) const {
    if (length == 0) return true;
    for (size_t i = 0; i < length; ++i) {
        char c = start[i];
        if (c != ' ' && c != '\t' && c != '\r') return false;
    }
    return true;
}

std::vector<std::string_view> SIMDLexer::Tokenize(const char* data, size_t length) const {
    std::vector<std::string_view> tokens;
    size_t i = 0;
    size_t vectorized_length = (length / 32) * 32;
    size_t last_pos = 0;

    __m256i space_mask   = _mm256_set1_epi8(' ');
    __m256i newline_mask = _mm256_set1_epi8('\n');

    for (; i < vectorized_length; i += 32) {
        __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
        __m256i cmp_space   = _mm256_cmpeq_epi8(chunk, space_mask);
        __m256i cmp_newline = _mm256_cmpeq_epi8(chunk, newline_mask);
        __m256i combined    = _mm256_or_si256(cmp_space, cmp_newline);
        uint32_t mask       = _mm256_movemask_epi8(combined);

        while (mask > 0) {
            unsigned long bit_index;
#if defined(_WIN32) || defined(_WIN64)
            _BitScanForward(&bit_index, mask);
#else
            bit_index = __builtin_ctz(mask);
#endif
            size_t current_pos = i + bit_index;
            size_t token_length = current_pos - last_pos;
            if (token_length > 0) {
                tokens.emplace_back(data + last_pos, token_length);
            }
            last_pos = current_pos + 1;
            mask &= (mask - 1);
        }
    }

    for (; i < length; ++i) {
        if (data[i] == ' ' || data[i] == '\n') {
            size_t token_length = i - last_pos;
            if (token_length > 0) tokens.emplace_back(data + last_pos, token_length);
            last_pos = i + 1;
        }
    }
    if (last_pos < length) {
        tokens.emplace_back(data + last_pos, length - last_pos);
    }
    return tokens;
}

std::vector<std::string_view> SIMDLexer::ExtractLines(const char* data, size_t length) const {
    std::vector<std::string_view> lines;
    size_t i = 0;
    size_t vectorized_length = (length / 32) * 32;
    size_t last_pos = 0;

    __m256i newline_mask = _mm256_set1_epi8('\n');

    for (; i < vectorized_length; i += 32) {
        __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
        __m256i cmp_newline = _mm256_cmpeq_epi8(chunk, newline_mask);
        uint32_t mask       = _mm256_movemask_epi8(cmp_newline);

        while (mask > 0) {
            unsigned long bit_index;
#if defined(_WIN32) || defined(_WIN64)
            _BitScanForward(&bit_index, mask);
#else
            bit_index = __builtin_ctz(mask);
#endif
            size_t current_pos = i + bit_index;
            size_t line_length = current_pos - last_pos;
            if (!IsWhitespaceOnly(data + last_pos, line_length)) {
                lines.emplace_back(data + last_pos, line_length);
            }
            last_pos = current_pos + 1;
            mask &= (mask - 1);
        }
    }

    for (; i < length; ++i) {
        if (data[i] == '\n') {
            size_t line_length = i - last_pos;
            if (!IsWhitespaceOnly(data + last_pos, line_length)) {
                lines.emplace_back(data + last_pos, line_length);
            }
            last_pos = i + 1;
        }
    }
    if (last_pos < length) {
        size_t line_length = length - last_pos;
        if (!IsWhitespaceOnly(data + last_pos, line_length)) {
            lines.emplace_back(data + last_pos, line_length);
        }
    }
    return lines;
}

} // namespace vcd