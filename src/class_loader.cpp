#include "class_loader.h"

#include <filesystem>

ClassLoader::ClassLoader() {}

// FIXME this is temporary
Class ClassLoader::load(std::string class_name) {
    ClassFile file;
    if(class_name == "java/lang/Object") {
        file.parse_file("C:/Users/Kirin/OneDrive/Desktop/mini-jvm/Object.class");
    }
    else if(class_name == "java/lang/String") {
        file.parse_file("C:/Users/Kirin/OneDrive/Desktop/mini-jvm/String.class");
    }
    else {
        file.parse_file("./" + class_name);
    }

    return Class(file);
}