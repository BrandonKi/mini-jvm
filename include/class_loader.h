#pragma once

#include "common.h"

#include "class_file.h"

class Class {
public:
    Class(ClassFile class_file) {

    }

private:
    // std::vector<CPInfo> constant_pool;
    // u16 access_flags;
    // u16 this_class;
    // u16 super_class_index;
    // Class* super_class;
    // u16 interfaces_count;
    // u16* interfaces;
    // u16 fields_count;
    // FieldInfo* field_info;
    // u16 methods_count;
    // MethodInfo* methods;
    // u16 attributes_count;
    // AttributeInfo* attributes;
};

class ClassLoader {
public:
    ClassLoader();

    Class load(std::string);

private:
    std::vector<std::string> class_paths{"."};
};