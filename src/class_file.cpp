#include "class_file.h"

ClassFile::ClassFile() {

}

bool ClassFile::load_constant_pool(FileBuffer& bytes, i32& index) {
    constant_pool_count = read_from_bytes<u16>(bytes.raw, index);

    constant_pool = new CPInfo[constant_pool_count];
    for(i32 i = 1; i < constant_pool_count; ++i) {
        auto& current = constant_pool[i];
        current.tag = read_from_bytes<u8>(bytes.raw, index);

        // TODO check if tag is valid
        switch(current.tag) {
            case CPInfoTag::Utf8: {
                current.Utf8_info.length = read_from_bytes<u16>(bytes.raw, index);
                current.Utf8_info.bytes = new u8[current.Utf8_info.length];
                std::memcpy(current.Utf8_info.bytes, bytes.raw + index, current.Utf8_info.length);
                index += current.Utf8_info.length;
                break;
            }
            case CPInfoTag::Class: {
                current.Class_info.name_index = read_from_bytes<u16>(bytes.raw, index);
                break;
            }
            case CPInfoTag::String: {
                current.String_info.string_index = read_from_bytes<u16>(bytes.raw, index);
                break;
            }
            case CPInfoTag::Fieldref: {
                current.Fieldref_info.class_index = read_from_bytes<u16>(bytes.raw, index);
                current.Fieldref_info.name_and_type_index = read_from_bytes<u16>(bytes.raw, index);
                break;
            }
            case CPInfoTag::Methodref: {
                current.Methodref_info.class_index = read_from_bytes<u16>(bytes.raw, index);
                current.Methodref_info.name_and_type_index = read_from_bytes<u16>(bytes.raw, index);
                break;
            }
            case CPInfoTag::InterfaceMethodref: {
                current.InterfaceMethodref_info.class_index = read_from_bytes<u16>(bytes.raw, index);
                current.InterfaceMethodref_info.name_and_type_index = read_from_bytes<u16>(bytes.raw, index);
                break;
            }
            case CPInfoTag::NameAndType: {
                current.NameAndType_info.name_index = read_from_bytes<u16>(bytes.raw, index);
                current.NameAndType_info.descriptor_index = read_from_bytes<u16>(bytes.raw, index);
                break;
            }
            default: {
                assert(false);
            }
        }
    }
    return true;
}

bool ClassFile::load_interfaces(FileBuffer& bytes, i32& index) {
    interfaces_count = read_from_bytes<u16>(bytes.raw, index);

    interfaces = new u16[interfaces_count];

    assert(interfaces_count == 0);

    return true;
}

bool ClassFile::load_fields(FileBuffer& bytes, i32& index) {
    fields_count = read_from_bytes<u16>(bytes.raw, index);

    field_info = new FieldInfo[fields_count];

    assert(fields_count == 0);

    return true;
}

bool ClassFile::load_methods(FileBuffer& bytes, i32& index) {
    methods_count = read_from_bytes<u16>(bytes.raw, index);

    methods = new MethodInfo[methods_count];
    for(i32 i = 0; i < methods_count; ++i) {
        auto& current = methods[i];

        current.access_flags = read_from_bytes<u16>(bytes.raw, index);
        current.name_index = read_from_bytes<u16>(bytes.raw, index);
        current.descriptor_index = read_from_bytes<u16>(bytes.raw, index);
        auto result = load_attributes(Location::MethodInfo, bytes, index);
        current.attributes_count = result.attributes_count;
        current.attributes = result.attributes;

        auto& cpinfo = constant_pool[current.descriptor_index];
        assert(cpinfo.tag == CPInfoTag::Utf8);

        current.parsed_method = parse_method_descriptor(cpinfo.Utf8_info.bytes, cpinfo.Utf8_info.length);
        continue;
    }

    return true;
}

// FIXME hash?, also add the rest of the attributes here
static AttributeTag cpinfo_to_attribute_tag(CPInfo& cpinfo) {
    assert(cpinfo.tag == CPInfoTag::Utf8);

    if(std::memcmp(cpinfo.Utf8_info.bytes, "Code", cpinfo.Utf8_info.length) == 0)
        return AttributeTag::Code;
    else if(std::memcmp(cpinfo.Utf8_info.bytes, "LineNumberTable", cpinfo.Utf8_info.length) == 0)
        return AttributeTag::LineNumberTable;
    else if(std::memcmp(cpinfo.Utf8_info.bytes, "SourceFile", cpinfo.Utf8_info.length) == 0)
        return AttributeTag::SourceFile;

    assert(false);
    return AttributeTag::Code;
}

ExceptionTableResult ClassFile::load_exception_table(FileBuffer& bytes, i32& index) {
    ExceptionTableResult result;

    result.exception_table_length = read_from_bytes<u16>(bytes.raw, index);
    result.exception_table = new ExceptionTableEntry[result.exception_table_length];
    for(i32 i = 0; i < result.exception_table_length; ++i) {
        auto& current = result.exception_table[i];

        current.start_pc = read_from_bytes<u16>(bytes.raw, index);
        current.end_pc = read_from_bytes<u16>(bytes.raw, index);
        current.handler_pc = read_from_bytes<u16>(bytes.raw, index);
        current.catch_type = read_from_bytes<u16>(bytes.raw, index);
    }
    
    return result;
}


AttributeResult ClassFile::load_attributes(Location loc, FileBuffer& bytes, i32& index) {
    AttributeResult result;
    result.attributes_count = read_from_bytes<u16>(bytes.raw, index);

    result.attributes = new AttributeInfo[attributes_count];
    for(i32 i = 0; i < result.attributes_count; ++i) {
        auto& current = result.attributes[i];

        current.attribute_name_index = read_from_bytes<u16>(bytes.raw, index);
        current.attribute_length = read_from_bytes<u32>(bytes.raw, index);

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

                current.Code_attribute.max_stack = read_from_bytes<u16>(bytes.raw, index);
                current.Code_attribute.max_locals = read_from_bytes<u16>(bytes.raw, index);
                current.Code_attribute.code_length = read_from_bytes<u32>(bytes.raw, index);
                
                current.Code_attribute.code = new u8[current.Code_attribute.code_length];
                std::memcpy(current.Code_attribute.code, bytes.raw + index, current.Code_attribute.code_length);
                index += current.Code_attribute.code_length;

                auto exception_result = load_exception_table(bytes, index);
                current.Code_attribute.exception_table_length = exception_result.exception_table_length;
                current.Code_attribute.exception_table = exception_result.exception_table;

                auto attribute_result = load_attributes(Location::Code, bytes, index);
                current.Code_attribute.attributes = attribute_result.attributes;
                current.Code_attribute.attributes_length = attribute_result.attributes_count;
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

                current.SourceFile_attribute.sourcefile_index = read_from_bytes<u16>(bytes.raw, index);
                break;
            }
            case LineNumberTable: {
                assert(loc == Location::Code);

                current.LineNumberTable_attribute.line_number_table_length = read_from_bytes<u16>(bytes.raw, index);
                current.LineNumberTable_attribute.line_number_table = new LineNumberTableEntry[current.LineNumberTable_attribute.line_number_table_length];
                for(i32 j = 0; j < current.LineNumberTable_attribute.line_number_table_length; ++j) {
                    auto& inner_current = current.LineNumberTable_attribute.line_number_table[j];

                    inner_current.start_pc = read_from_bytes<u16>(bytes.raw, index);
                    inner_current.line_number = read_from_bytes<u16>(bytes.raw, index);
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
    FileBuffer bytes = read_file(file_path);

    // check magic
    u32 magic = read_from_bytes<u32>(bytes.raw, index);
    if(magic != ClassFileMagic)
        return false;

    // check version
    u16 minor_version = read_from_bytes<u16>(bytes.raw, index);
    u16 major_version = read_from_bytes<u16>(bytes.raw, index);
    if(!version_is_supported(minor_version, major_version))
        return false;

    load_constant_pool(bytes, index);

    access_flags = read_from_bytes<u16>(bytes.raw, index);
    this_class = read_from_bytes<u16>(bytes.raw, index);
    super_class = read_from_bytes<u16>(bytes.raw, index);

    load_interfaces(bytes, index);

    load_fields(bytes, index);
    load_methods(bytes, index);
    load_attributes(Location::ClassFile, bytes, index);

    return true;
}

FieldType ClassFile::parse_field_type(u8* bytes, u16 length, u16& index) {
    FieldType result;
    switch(bytes[index]) {
        case 'B': {
            result = FieldType{FieldTypeTag::BaseType};
            result.base_type = BaseType::Byte;
            break;
        }
        case 'C': {
            result = FieldType{FieldTypeTag::BaseType};
            result.base_type = BaseType::Char;
            break;
        }
        case 'D': {
            result = FieldType{FieldTypeTag::BaseType};
            result.base_type = BaseType::Double;
            break;
        }
        case 'F': {
            result = FieldType{FieldTypeTag::BaseType};
            result.base_type = BaseType::Float;
            break;
        }
        case 'I': {
            result = FieldType{FieldTypeTag::BaseType};
            result.base_type = BaseType::Int;
            break;
        }
        case 'J': {
            result = FieldType{FieldTypeTag::BaseType};
            result.base_type = BaseType::Long;
            break;
        }
        case 'L': {
            result = FieldType{FieldTypeTag::ObjectType};
            ++index;
            u8* start = bytes + index;
            result.object_type.length = 0;
            for(; bytes[index] != ';'; ++index) {
                result.object_type.length++;
            }
            result.object_type.class_name = new u8[result.object_type.length];
            memcpy(result.object_type.class_name, start, result.object_type.length);
            break;
        }
        case 'S': {
            result = FieldType{FieldTypeTag::BaseType};
            result.base_type = BaseType::Short;
            break;
        }
        case 'Z': {
            result = FieldType{FieldTypeTag::BaseType};
            result.base_type = BaseType::Boolean;
            break;
        }
        case '[': {
            result = FieldType{FieldTypeTag::ArrayType};
            result.array_type.dimensions = 1;
            // FIXME this is dumb
            result.array_type.component_type = new FieldType(parse_field_type(bytes, length, ++index));
            break;
        }
        default: {
            assert(false);
            break;
        }
    }
    return result;
}

FieldDescriptor ClassFile::parse_field_descriptor(u8* bytes, u16 length, u16& index) {
    return FieldDescriptor{parse_field_type(bytes, length, index)};
}

ParameterDescriptor ClassFile::parse_parameter_descriptor(u8* bytes, u16 length, u16& index) {
    return ParameterDescriptor{parse_field_type(bytes, length, index)};
}

ReturnDescriptor ClassFile::parse_return_descriptor(u8* bytes, u16 length, u16& index) {
    ReturnDescriptor result;
    if(bytes[index] == 'V')
        result.is_void = true;
    else
        result.type = parse_field_type(bytes, length, index); 
    return result;
}

MethodDescriptor ClassFile::parse_method_descriptor(u8* bytes, u16 length) {
    assert(length >= 3);
    assert(bytes[0] == '(');

    MethodDescriptor result;

    u16 index;
    for(index = 1; bytes[index] != ')'; ++index) {
        result.parameter_types.push_back(parse_parameter_descriptor(bytes, length, index));
    }
    ++index;
    result.return_type = parse_return_descriptor(bytes, length, index);
    return result;
}

