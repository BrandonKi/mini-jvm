#ifndef MINI_JVM_VM_H
#define MINI_JVM_VM_H

#include "class_file.h"

#include "common.h"

// TODO maybe remove None
enum PrimitiveTypeTag: u8 {
    None,

    Float,
    Double,

    Byte,
    Short,
    Int,
    Long,
    Char,
    
    Boolean,
    ReturnAddress,
};

struct PrimitiveType {
    PrimitiveTypeTag tag;
    union {
        struct {
            f32 value;
        } Float;
        struct {
            f64 value;
        } Double;
        struct {
            i8 value;
        } Byte;
        struct {
            i16 value;
        } Short;
        struct {
            i32 value;
        } Int;
        struct {
            i64 value;
        } Long;
        struct {
            u16 value;
        } Char;
        struct {
            i8 value;
        } Boolean;
        struct {
            void* value;
        } ReturnAddress;
    };
};

static bool is_floating_point(PrimitiveTypeTag tag) {
    return tag == Float || tag == Double; 
}

static bool is_integral(PrimitiveTypeTag tag) {
    return tag >= Float && tag <= Char;
}

// TODO maybe remove this
static bool is_primitive(PrimitiveTypeTag tag) {
    return tag != None;
}

class VM {
public:
    VM();

private:
    std::vector<Method> methods;
    // GCHeap heap;
    // ClassManager class_manager;
    ClassLoader class_loader;
};

#endif // MINI_JVM_VM_H