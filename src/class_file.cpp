#include "class_file.h"

ClassFile::ClassFile() {

}

bool ClassFile::load_constant_pool(std::string& bytes, i32& index) {
    constant_pool_count = read_from_bytes<u16>(bytes.data(), index);

    constant_pool = new CPInfo[constant_pool_count];
    for(i32 i = 1; i < constant_pool_count; ++i) {
        auto& current = constant_pool[i];
        current.tag = read_from_bytes<u8>(bytes.data(), index);

        // TODO check if tag is valid
        switch(current.tag) {
            case CPInfoTag::Utf8: {
                current.Utf8_info.length = read_from_bytes<u16>(bytes.data(), index);
                current.Utf8_info.bytes = new u8[current.Utf8_info.length];
                std::memcpy(current.Utf8_info.bytes, bytes.data() + index, current.Utf8_info.length);
                index += current.Utf8_info.length;
                break;
            }
            case CPInfoTag::Class: {
                current.Class_info.name_index = read_from_bytes<u16>(bytes.data(), index);
                break;
            }
            case CPInfoTag::String: {
                current.String_info.string_index = read_from_bytes<u16>(bytes.data(), index);
                break;
            }
            case CPInfoTag::Fieldref: {
                current.Fieldref_info.class_index = read_from_bytes<u16>(bytes.data(), index);
                current.Fieldref_info.name_and_type_index = read_from_bytes<u16>(bytes.data(), index);
                break;
            }
            case CPInfoTag::Methodref: {
                current.Methodref_info.class_index = read_from_bytes<u16>(bytes.data(), index);
                current.Methodref_info.name_and_type_index = read_from_bytes<u16>(bytes.data(), index);
                break;
            }
            case CPInfoTag::InterfaceMethodref: {
                current.InterfaceMethodref_info.class_index = read_from_bytes<u16>(bytes.data(), index);
                current.InterfaceMethodref_info.name_and_type_index = read_from_bytes<u16>(bytes.data(), index);
                break;
            }
            case CPInfoTag::NameAndType: {
                current.NameAndType_info.name_index = read_from_bytes<u16>(bytes.data(), index);
                current.NameAndType_info.descriptor_index = read_from_bytes<u16>(bytes.data(), index);
                break;
            }
            default: {
                assert(false);
            }
        }
    }
    return true;
}

bool ClassFile::load_interfaces(std::string& bytes, i32& index) {
    interfaces_count = read_from_bytes<u16>(bytes.data(), index);

    interfaces = new u16[interfaces_count];

    assert(interfaces_count == 0);

    return true;
}

bool ClassFile::load_fields(std::string& bytes, i32& index) {
    fields_count = read_from_bytes<u16>(bytes.data(), index);

    field_info = new FieldInfo[fields_count];

    assert(fields_count == 0);

    return true;
}

bool ClassFile::load_methods(std::string& bytes, i32& index) {
    methods_count = read_from_bytes<u16>(bytes.data(), index);

    methods = new MethodInfo[methods_count];
    for(i32 i = 0; i < methods_count; ++i) {
        auto& current = methods[i];

        current.access_flags = read_from_bytes<u16>(bytes.data(), index);
        current.name_index = read_from_bytes<u16>(bytes.data(), index);
        current.descriptor_index = read_from_bytes<u16>(bytes.data(), index);
        auto result = load_attributes(Location::MethodInfo, bytes, index);
        current.attributes_count = result.attributes_count;
        current.attributes = result.attributes;
    }

    return true;
}

// FIXME hash?, also add the rest of the attributes here
static AttributeTag cpinfo_to_attribute_tag(CPInfo& cpinfo) {
    assert(cpinfo.tag == CPInfoTag::Utf8);

    if(std::memcmp(cpinfo.Utf8_info.bytes, "Code", cpinfo.Utf8_info.length))
        return AttributeTag::Code;

    assert(false);
    return AttributeTag::Code;
}

ExceptionTableResult ClassFile::load_exception_table(std::string& bytes, i32& index) {
    ExceptionTableResult result;

    result.exception_table_length = read_from_bytes<u16>(bytes.data(), index);
    result.exception_table = new ExceptionTableEntry[result.exception_table_length];
    for(i32 i = 0; i < result.exception_table_length; ++i) {
        auto& current = result.exception_table[i];

        current.start_pc = read_from_bytes<u16>(bytes.data(), index);
        current.end_pc = read_from_bytes<u16>(bytes.data(), index);
        current.handler_pc = read_from_bytes<u16>(bytes.data(), index);
        current.catch_type = read_from_bytes<u16>(bytes.data(), index);
    }
    
    return result;
}


AttributeResult ClassFile::load_attributes(Location loc, std::string& bytes, i32& index) {
    AttributeResult result;
    result.attributes_count = read_from_bytes<u16>(bytes.data(), index);

    result.attributes = new AttributeInfo[attributes_count];
    for(i32 i = 0; i < attributes_count; ++i) {
        auto& current = attributes[i];

        current.attribute_name_index = read_from_bytes<u16>(bytes.data(), index);
        current.attribute_length = read_from_bytes<u32>(bytes.data(), index);

        CPInfo& tag_str = this->constant_pool[current.attribute_name_index];
        current.attribute_tag = cpinfo_to_attribute_tag(tag_str);

        // NOTE https://docs.oracle.com/javase/specs/jvms/se20/html/jvms-4.html#jvms-4.7-C
        switch(current.attribute_tag) {
            using enum AttributeTag;
            case AttributeTag::ConstantValue: {
                break;
            }
            // TODO verify constraints https://docs.oracle.com/javase/specs/jvms/se20/html/jvms-4.html#jvms-4.9
            case Code: {
                assert(loc == Location::MethodInfo);

                current.Code_attribute.max_stack = read_from_bytes<u16>(bytes.data(), index);
                current.Code_attribute.max_locals = read_from_bytes<u16>(bytes.data(), index);
                current.Code_attribute.code_length = read_from_bytes<u32>(bytes.data(), index);
                   
                std::memcpy(current.Code_attribute.code, bytes.data() + index, current.Code_attribute.code_length);
                index += current.Code_attribute.code_length;

                auto exception_result = load_exception_table(bytes, index);
                current.Code_attribute.exception_table_length = exception_result.exception_table_length;
                current.Code_attribute.exception_table = exception_result.exception_table;

                auto attribute_result = load_attributes(Location::Code, bytes, index);
                break;
            }
            case StackMapTable: {
                break;
            }
            case BootstrapMethods: {
                break;
            }
            case NestHost: {
                break;
            }
            case NestMembers: {
                break;
            }
            case PermittedSubclasses: {
                break;
            }
            case Exceptions: {
                break;
            }
            case InnerClasses: {
                break;
            }
            case EnclosingMethod: {
                break;
            }
            case Synthetic: {
                break;
            }
            case Signature: {
                break;
            }
            case Record: {
                break;
            }
            case SourceFile: {
                assert(loc == Location::ClassFile);

                current.SourceFile_attribute.sourcefile_index = read_from_bytes<u16>(bytes.data(), index);
                break;
            }
            case LineNumberTable: {
                assert(loc == Location::Code);

                current.LineNumberTable_attribute.line_number_table_length = read_from_bytes<u16>(bytes.data(), index);
                current.LineNumberTable_attribute.line_number_table = new LineNumberTableEntry[current.LineNumberTable_attribute.line_number_table_length];
                for(i32 j = 0; j < current.LineNumberTable_attribute.line_number_table_length; ++j) {
                    auto& inner_current = current.LineNumberTable_attribute.line_number_table[j];

                    inner_current.start_pc = read_from_bytes<u16>(bytes.data(), index);
                    inner_current.line_number = read_from_bytes<u16>(bytes.data(), index);
                }
                break;
            }
            case LocalVariableTable: {
                break;
            }
            case LocalVariableTypeTable: {
                break;
            }
            case SourceDebugExtension: {
                break;
            }
            case Deprecated: {
                break;
            }
            case RuntimeVisibleAnnotations: {
                break;
            }
            case RuntimeInvisibleAnnotations: {
                break;
            }
            case RuntimeVisibleParameterAnnotations: {
                break;
            }
            case RuntimeInvisibleParameterAnnotations: {
                break;
            }
            case RuntimeVisibleTypeAnnotations: {
                break;
            }
            case RuntimeInvisibleTypeAnnotations: {
                break;
            }
            case AnnotationDefault: {
                break;
            }
            case MethodParameters: {
                break;
            }
            case Module: {
                break;
            }
            case ModulePackages: {
                break;
            }
            case ModuleMainClass: {
                break;
            }
            default: {
                assert(false);
            }

        }
    }
    return result;
}


bool ClassFile::parse_file(std::string&& file_path) {
    i32 index = 0;
    std::string bytes = read_file(file_path);

    // check magic
    u32 magic = read_from_bytes<u32>(bytes.data(), index);
    // std::memcpy(&magic, bytes.data() + index, 4);
    // magic = std::byteswap(magic);
    // index += 4;
    if(magic != ClassFileMagic)
        return false;

    // check version
    u16 minor_version = read_from_bytes<u16>(bytes.data(), index);
    u16 major_version = read_from_bytes<u16>(bytes.data(), index);
    if(!version_is_supported(minor_version, major_version))
        return false;

    load_constant_pool(bytes, index);

    access_flags = read_from_bytes<u16>(bytes.data(), index);
    this_class = read_from_bytes<u16>(bytes.data(), index);
    super_class = read_from_bytes<u16>(bytes.data(), index);

    load_interfaces(bytes, index);

    return true;
}

