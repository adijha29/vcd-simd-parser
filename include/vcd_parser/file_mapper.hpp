#ifndef VCD_PARSER_FILE_MAPPER_HPP
#define VCD_PARSER_FILE_MAPPER_HPP

#include <string>
#include <stdexcept>

#if defined(_WIN32) || defined(_WIN64)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else
    #include <fcntl.h>
    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <unistd.h>
#endif

namespace vcd {

class MappedFile {
private:
    size_t file_size = 0;
    void* mapped_data = nullptr;
#if defined(_WIN32) || defined(_WIN64)
    HANDLE file_handle = INVALID_HANDLE_VALUE;
    HANDLE mapping_handle = NULL;
#else
    int fd = -1;
#endif

public:
    explicit MappedFile(const std::string& filepath) {
#if defined(_WIN32) || defined(_WIN64)
        file_handle = CreateFileA(filepath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
        if (file_handle == INVALID_HANDLE_VALUE) throw std::runtime_error("OS Error: Failed to open file target.");
        LARGE_INTEGER size;
        GetFileSizeEx(file_handle, &size);
        file_size = static_cast<size_t>(size.QuadPart);
        mapping_handle = CreateFileMappingA(file_handle, NULL, PAGE_READONLY, 0, 0, NULL);
        mapped_data = MapViewOfFile(mapping_handle, FILE_MAP_READ, 0, 0, 0);
#else
        fd = open(filepath.c_str(), O_RDONLY);
        if (fd == -1) throw std::runtime_error("OS Error: Failed to open descriptor.");
        struct stat sb;
        fstat(fd, &sb);
        file_size = sb.st_size;
        mapped_data = mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
        madvise(mapped_data, file_size, MADV_SEQUENTIAL);
#endif
    }

    ~MappedFile() {
#if defined(_WIN32) || defined(_WIN64)
        if (mapped_data) UnmapViewOfFile(mapped_data);
        if (mapping_handle) CloseHandle(mapping_handle);
        if (file_handle != INVALID_HANDLE_VALUE) CloseHandle(file_handle);
#else
        if (mapped_data != MAP_FAILED && mapped_data != nullptr) munmap(mapped_data, file_size);
        if (fd != -1) close(fd);
#endif
    }

    MappedFile(const MappedFile&) = delete;
    MappedFile& operator=(const MappedFile&) = delete;

    const char* data() const { return reinterpret_cast<const char*>(mapped_data); }
    size_t size() const { return file_size; }
};

} // namespace vcd

#endif