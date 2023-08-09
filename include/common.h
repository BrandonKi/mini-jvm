#ifndef MINI_JVM_COMMON_H
#define MINI_JVM_COMMON_H

#include "cprint.h"

#include <cstdint>
#include <cassert>
#include <bit>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using f32 = float;
using f64 = double;

struct FileBuffer {
    size_t size;
    char* raw;
};
// meh
[[nodiscard]] static FileBuffer read_file(const std::string& filepath) {
    FileBuffer buffer;
    std::ifstream t(filepath, std::ios::binary);
    t.seekg(0, std::ios::end);
    buffer.size = t.tellg();
    buffer.raw = new char[buffer.size];
    t.seekg(0);
    t.read(buffer.raw, buffer.size); 

    return buffer;
}

#endif // MINI_JVM_COMMON_H