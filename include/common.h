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

// meh
[[nodiscard]] static std::string read_file(const std::string& filepath) {
    std::ifstream file;
    file.open(filepath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return std::string(buffer.str());
}

#endif // MINI_JVM_COMMON_H