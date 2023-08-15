#include "class_loader.h"
#include "vm.h"

int main(int argc, char* argv[]) {
    ClassLoader loader;
    auto java_class = loader.load(argv[1]);



}