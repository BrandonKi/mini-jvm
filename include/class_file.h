#ifndef MINI_JVM_CLASS_FILE_H
#define MINI_JVM_CLASS_FILE_H

#include "common.h"

constexpr u32 ClassFileMagic = 0xCAFEBABE;

// not accurate
inline bool version_is_supported(u16 minor_version, u16 major_version) {
    if(major_version >= 45 && major_version <= 64)
        return true;
    return false;
}

enum CPInfoTag: u8 {
    Utf8               = 1,
    Integer            = 3,
    Float              = 4,
    Long               = 5,
    Double             = 6,
    Class              = 7,
    String             = 8,
    Fieldref           = 9,
    Methodref          = 10,
    InterfaceMethodref = 11,
    NameAndType        = 12,
    MethodHandle       = 15,
    MethodType         = 16,
    Dynamic            = 17,
    InvokeDynamic      = 18,
    Module             = 19,
    Package            = 20,
};

struct CPInfo {
    u8 tag;
    union {
        struct {
            u16 length;
            u8* bytes;
        } Utf8_info;

        struct {
            u32 bytes;
        } Integer_info;

        struct {
            u32 bytes;
        } Float_info;

        struct {
            u32 high_bytes;
            u32 low_bytes;
        } Long_info;

        struct {
            u32 high_bytes;
            u32 low_bytes;
        } Double_info;

        struct {
            u16 name_index;
        } Class_info;

        struct {
            u16 string_index;
        } String_info;

        struct {
            u16 class_index;
            u16 name_and_type_index;
        } Fieldref_info;

        struct  {
            u16 class_index;
            u16 name_and_type_index;
        } Methodref_info;

        struct  {
            u16 class_index;
            u16 name_and_type_index;
        } InterfaceMethodref_info;

        struct {
            u16 name_index;
            u16 descriptor_index;
        } NameAndType_info;

        struct {
            u8 reference_kind;
            u16 reference_index;
        } MethodHandle_info;

        struct {
            u16 descriptor_index;
        } MethodType_info;

        struct {
            u16 bootstrap_method_attr_index;
            u16 name_and_type_index;
        } Dynamic_info;

        struct {
            u16 bootstrap_method_attr_index;
            u16 name_and_type_index;
        } InvokeDynamic_info;

        struct {
            u16 name_index;
        } Module_info;

        struct {
            u16 name_index;
        } Package_info;
    };
};

enum class ClassFileAccessFlags: u16 {
    PUBLIC = 0x0001,
    FINAL = 0x0010,
    SUPER = 0x0020,
    INTERFACE = 0x0200,
    ABSTRACT = 0x0400,
    SYNTHETIC = 0x1000,
    ANNOTATION = 0x2000,
    ENUM = 0x4000,
    MODULE = 0x8000,
};

// NOTE range is [start_pc, end_pc)
struct ExceptionTableEntry {
    u16 start_pc;
    u16 end_pc;
    u16 handler_pc;
    u16 catch_type;
};



enum class VerificationTypeInfoTag: u8 {
    Top = 0,
    Integer = 1,
    Float = 2,
    Null = 5,
    UninitializedThis = 6,
    Object = 7,
    Uninitialized = 8,
    
    Long = 4,
    Double = 3,
};

struct VerificationTypeInfo {
    VerificationTypeInfoTag tag;
    union {
        struct {
            u16 cpool_index;
        } Object_variable_info;
        struct {
            u16 offset;
        } Uninitialized_variable_info;
    };
};

struct StackMapFrame {
    u8 tag;
    union {
        /* 0-63 */
        struct {} same_frame;
        /* 64-127 */
        struct {
            VerificationTypeInfo stack;
        } same_locals_1_stack_item_frame;
        /* 247 */
        struct {
            u16 offset_delta;
            VerificationTypeInfo stack;
        } same_locals_1_stack_item_frame_extended;
        /* 248-250 */
        struct {
            u16 offset_delta;
        } chop_frame;
        /* 251 */
        struct {
            u16 offset_delta;
        } same_frame_extended;
        /* 252-254 */
        struct {
            u16 offset_delta;
            VerificationTypeInfo* locals; // [tag - 251]
        } append_frame;
        /* 255 */
        struct {
            u16 offset_delta;
            u16 number_of_locals;
            VerificationTypeInfo locals; // [number_of_locals];
            u16 number_of_stack_items;
            VerificationTypeInfo stack; // [number_of_stack_items];
        } full_frame;
    };
};


struct InnerClassEntry {
    u16 inner_class_info_index;
    u16 outer_class_info_index;
    u16 inner_name_index;
    u16 inner_class_access_flags;
};

struct LineNumberTableEntry {
    u16 start_pc;
    u16 line_number;	
};

struct LocalVariableTableEntry {
    u16 start_pc;
    u16 length;
    u16 name_index;
    u16 descriptor_index;
    u16 index;
};

struct LocalVariableTypeTableEntry {
    u16 start_pc;
    u16 length;
    u16 name_index;
    u16 signature_index;
    u16 index;
};

struct ElementValuePair;

struct Annotation {
    u16 type_index;
    u16 num_element_value_pairs;
    ElementValuePair* element_value_pairs;
};

// TAGS:
// B	byte	const_value_index	CONSTANT_Integer
// C	char	const_value_index	CONSTANT_Integer
// D	double	const_value_index	CONSTANT_Double
// F	float	const_value_index	CONSTANT_Float
// I	int	const_value_index	CONSTANT_Integer
// J	long	const_value_index	CONSTANT_Long
// S	short	const_value_index	CONSTANT_Integer
// Z	boolean	const_value_index	CONSTANT_Integer
// s	String	const_value_index	CONSTANT_Utf8
// e	Enum class	enum_const_value	Not applicable
// c	Class	class_info_index	Not applicable
// @	Annotation interface	annotation_value	Not applicable
// [	Array type	array_value	Not applicable
struct ElementValue {
    u8 tag;
    union {
        u16 const_value_index;

        struct {
            u16 type_name_index;
            u16 const_name_index;
        } enum_const_value;

        u16 class_info_index;

        Annotation annotation_value;

        struct {
            u16 num_values;
            ElementValue* values;
        } array_value;
    } value;
};

struct ElementValuePair {
    u16 element_name_index;
    ElementValue value;
};

struct ParameterAnnotation {
    u16 num_annotations;
    Annotation* annotations;
};

struct LocalVarTypeAnnotationTableEntry {
    u16 start_pc;
    u16 length;
    u16 index;
};


// https://docs.oracle.com/javase/specs/jvms/se20/html/jvms-4.html#jvms-4.7.20.2
struct TypePathEntry {
    u8 type_path_kind;
    u8 type_argument_index;
};

struct TypePath {
    u8 path_length;
    TypePathEntry* path;
};

// Value	Kind of target	target_info item
// 0x00	type parameter declaration of generic class or interface	type_parameter_target
// 0x01	type parameter declaration of generic method or constructor	type_parameter_target
// 0x10	type in extends or implements clause of class declaration (including the direct superclass or direct superinterface of an anonymous class declaration), or in extends clause of interface declaration	supertype_target
// 0x11	type in bound of type parameter declaration of generic class or interface	type_parameter_bound_target
// 0x12	type in bound of type parameter declaration of generic method or constructor	type_parameter_bound_target
// 0x13	
// type in field or record component declaration

// empty_target
// 0x14	return type of method, or type of newly constructed object	empty_target
// 0x15	receiver type of method or constructor	empty_target
// 0x16	type in formal parameter declaration of method, constructor, or lambda expression	formal_parameter_target
// 0x17	type in throws clause of method or constructor	throws_target

// Value	Kind of target	target_info item
// 0x40	type in local variable declaration	localvar_target
// 0x41	type in resource variable declaration	localvar_target
// 0x42	type in exception parameter declaration	catch_target
// 0x43	type in instanceof expression	offset_target
// 0x44	type in new expression	offset_target
// 0x45	type in method reference expression using ::new	offset_target
// 0x46	type in method reference expression using ::Identifier	offset_target
// 0x47	type in cast expression	type_argument_target
// 0x48	type argument for generic constructor in new expression or explicit constructor invocation statement	type_argument_target
// 0x49	type argument for generic method in method invocation expression	type_argument_target
// 0x4A	type argument for generic constructor in method reference expression using ::new	type_argument_target
// 0x4B	type argument for generic method in method reference expression using ::Identifier	type_argument_target

// Value	Kind of target	Location
// 0x00	type parameter declaration of generic class or interface	ClassFile
// 0x01	type parameter declaration of generic method or constructor	method_info
// 0x10	type in extends clause of class or interface declaration, or in implements clause of interface declaration	ClassFile
// 0x11	type in bound of type parameter declaration of generic class or interface	ClassFile
// 0x12	type in bound of type parameter declaration of generic method or constructor	method_info
// 0x13	
// type in field or record component declaration

// field_info, record_component_info

// 0x14	return type of method or constructor	method_info
// 0x15	receiver type of method or constructor	method_info
// 0x16	type in formal parameter declaration of method, constructor, or lambda expression	method_info
// 0x17	type in throws clause of method or constructor	method_info
// 0x40-0x4B	types in local variable declarations, resource variable declarations, exception parameter declarations, expressions	Code
struct TypeAnnotation {
    u8 tag;
    union {
        struct {
            u8 type_parameter_index;
        } type_parameter_target;

        struct {
            u16 supertype_index;
        } supertype_target;

        struct {
            u8 type_parameter_index;
            u8 bound_index;
        } type_parameter_bound_target;

        struct {} empty_target;

        struct {
            u8 formal_parameter_index;
        } formal_parameter_target;

        struct {
            u16 throws_type_index;
        } throws_target;

        struct {
            u16 table_length;
            LocalVarTypeAnnotationTableEntry* table;
        } localvar_target;

        struct {
            u16 exception_table_index;
        } catch_target;

        struct {
            u16 offset;
        } offset_target;

        struct {
            u16 offset;
            u8 type_argument_index;
        } type_argument_target;
    } target_info;
    TypePath target_path;
    u16 type_index;
    u16 num_element_value_pairs;
    ElementValuePair* element_value_pairs;
};

// https://docs.oracle.com/javase/specs/jvms/se20/html/jvms-4.html#jvms-4.7.23
struct BootstrapMethodEntry {
    u16 bootstrap_method_ref;
    u16 num_bootstrap_arguments;
    u16* bootstrap_arguments;
};

// 0x0010 (ACC_FINAL)
// 0x1000 (ACC_SYNTHETIC)
// 0x8000 (ACC_MANDATED)
// https://docs.oracle.com/javase/specs/jvms/se20/html/jvms-4.html#jvms-4.7.24
struct MethodParameterEntry {
    u16 name_index;
    u16 access_flags;
};

struct RequiresEntry {
    u16 requires_index;
    u16 requires_flags;
    u16 requires_version_index;
};

struct ExportsEntry {
    u16 exports_index;
    u16 exports_flags;
    u16 exports_to_count;
    u16* exports_to_index;
};

struct OpensEntry {
    u16 opens_index;
    u16 opens_flags;
    u16 opens_to_count;
    u16* opens_to_index;
};

struct ProvidesEntry {
    u16 provides_index;
    u16 provides_with_count;
    u16* provides_with_index;
};

struct AttributeInfo;

struct RecordComponentInfo {
    u16 name_index;
    u16 descriptor_index;
    u16 attributes_count;
    AttributeInfo* attributes;
};

enum class AttributeTag: u8 {
    ConstantValue,
    Code,
    StackMapTable,
    BootstrapMethods,
    NestHost,
    NestMembers,
    PermittedSubclasses,
    Exceptions,
    InnerClasses,
    EnclosingMethod,
    Synthetic,
    Signature,
    Record,
    SourceFile,
    LineNumberTable,
    LocalVariableTable,
    LocalVariableTypeTable,
    SourceDebugExtension,
    Deprecated,
    RuntimeVisibleAnnotations,
    RuntimeInvisibleAnnotations,
    RuntimeVisibleParameterAnnotations,
    RuntimeInvisibleParameterAnnotations,
    RuntimeVisibleTypeAnnotations,
    RuntimeInvisibleTypeAnnotations,
    AnnotationDefault,
    MethodParameters,
    Module,
    ModulePackages,
    ModuleMainClass,
};

struct AttributeInfo {
    AttributeTag attribute_tag;
    u16 attribute_name_index;
    u32 attribute_length;
    union {
        struct {
            u16 attribute_name_index;
            u16 constantvalue_index;
        } ConstantValue_attribute;

        struct {
            u16 max_stack;
            u16 max_locals;
            u32 code_length;
            u8* code;
            u16 exception_table_length;
            ExceptionTableEntry* exception_table;
            u16 attributes_length;
            AttributeInfo* attributes;
        } Code_attribute;

        struct {
            u16 entries_length;
            StackMapFrame* entries;
        } StackMapTable_attribute;

        struct {
            u16 number_of_exceptions;
            u16* exception_index_table;
        } Exceptions_attribute;

        struct {
            u16 number_of_classes;
            InnerClassEntry* classes;
        } InnerClasses_attribute;

        struct {
            u16 class_index;
            u16 method_index;
        } EnclosingMethod_attribute;

        struct {} Synthetic_attribute;

        struct {
            u16 signature_index;
        } Signature_attribute;

        struct {
            u16 sourcefile_index;
        } SourceFile_attribute;

        struct {
            u8* debug_extension; // [attribute_length]
        } SourceDebugExtension_attribute;

        struct {
            u16 line_number_table_length;
            LineNumberTableEntry* line_number_table;
        } LineNumberTable_attribute;

        struct {
            u16 local_variable_table_length;
            LocalVariableTableEntry* local_variable_table;
        } LocalVariableTable_attribute;

        struct {
            u16 local_variable_type_table_length;
            LocalVariableTypeTableEntry* local_variable_type_table;
        } LocalVariableTypeTable_attribute;

        struct {} Deprecated_attribute;

        struct {
            u16 num_annotations;
            Annotation* annotations;
        } RuntimeVisibleAnnotations_attribute;

        struct {
            u16 num_annotations;
            Annotation* annotations;
        } RuntimeInvisibleAnnotations_attribute;

        struct {
            u8 num_parameters;
            ParameterAnnotation* parameter_annotations;
        } RuntimeVisibleParameterAnnotations_attribute;

        struct {
            u8 num_parameters;
            ParameterAnnotation* parameter_annotations;
        } RuntimeInvisibleParameterAnnotations_attribute;

        struct {
            u16 num_annotations;
            TypeAnnotation* annotations;
        } RuntimeVisibleTypeAnnotations_attribute;

        struct {
            u16 num_annotations;
            TypeAnnotation* annotations;
        } RuntimeInvisibleTypeAnnotations_attribute;

        struct {
            ElementValue default_value;
        } AnnotationDefault_attribute;
        // https://docs.oracle.com/javase/specs/jvms/se20/html/jvms-4.html#jvms-4.7.23
        struct {
            u16 num_bootstrap_methods;
            BootstrapMethodEntry* bootstrap_methods;
        } BootstrapMethods_attribute;
        // https://docs.oracle.com/javase/specs/jvms/se20/html/jvms-4.html#jvms-4.7.24
        struct {
            u8 parameters_count;
            MethodParameterEntry* parameters;
        } MethodParameters_attribute;

        // https://docs.oracle.com/javase/specs/jvms/se20/html/jvms-4.html#jvms-4.7.25
        struct {
            u16 module_name_index;
            u16 module_flags;
            u16 module_version_index;

            u16 requires_count;
            RequiresEntry* requires_list;

            u16 exports_count;
            ExportsEntry* exports;

            u16 opens_count;
            OpensEntry* opens;

            u16 uses_count;
            u16* uses_index;

            u16 provides_count;
            ProvidesEntry* provides;
        } Module_attribute;
        // https://docs.oracle.com/javase/specs/jvms/se20/html/jvms-4.html#jvms-4.7.26
        struct {
            u16 package_count;
            u16* package_index;
        } ModulePackages_attribute;
        // https://docs.oracle.com/javase/specs/jvms/se20/html/jvms-4.html#jvms-4.7.27
        struct {
            u16 main_class_index;
        } ModuleMainClass_attribute;
        // https://docs.oracle.com/javase/specs/jvms/se20/html/jvms-4.html#jvms-4.7.28
        struct {
            u16 host_class_index;
        } NestHost_attribute;
        // https://docs.oracle.com/javase/specs/jvms/se20/html/jvms-4.html#jvms-4.7.29
        struct {
            u16 number_of_classes;
            u16* classes;
        } NestMembers_attribute;
        // https://docs.oracle.com/javase/specs/jvms/se20/html/jvms-4.html#jvms-4.7.30
        struct {
            u16 components_count;
            RecordComponentInfo* components;
        } Record_attribute;
        // https://docs.oracle.com/javase/specs/jvms/se20/html/jvms-4.html#jvms-4.7.31
        struct {
            u16 number_of_classes;
            u16* classes;
        } PermittedSubclasses_attribute;
    };
};

struct FieldInfo {
    u16 access_flags;
    u16 name_index;
    u16 descriptor_index;
    u16 attributes_count;
    AttributeInfo* attributes;
};

enum class MethodInfoAccessFlags: u16 {
    ACC_PUBLIC = 0x0001,
    ACC_PRIVATE = 0x0002,	
    ACC_PROTECTED = 0x0004,
    ACC_STATIC = 0x0008,
    ACC_FINAL = 0x0010,
    ACC_SYNCHRONIZED = 0x0020,
    ACC_BRIDGE = 0x0040,
    ACC_VARARGS = 0x0080,
    ACC_NATIVE = 0x0100,
    ACC_ABSTRACT = 0x0400,
    ACC_STRICT = 0x0800,
    ACC_SYNTHETIC = 0x1000,
};

// https://docs.oracle.com/javase/specs/jvms/se20/html/jvms-4.html#jvms-4.6
struct MethodInfo {
    u16 access_flags;
    u16 name_index;
    u16 descriptor_index;
    u16 attributes_count;
    AttributeInfo* attributes;
};

enum class Location: u8 {
    ClassFile,
    FieldInfo,
    MethodInfo,
    RecordComponentInfo,
    Code,
};

struct AttributeResult {
    u16 attributes_count;
    AttributeInfo* attributes;
};

struct ExceptionTableResult {
    u16 exception_table_length;
    ExceptionTableEntry* exception_table;
};

class ClassFile {
public:
    ClassFile();

    bool parse_file(std::string&& file_path);

    // FIXME first entry in constant pool is empty for now
    u16 constant_pool_count;
    CPInfo* constant_pool;
    u16 access_flags;
    u16 this_class;
    u16 super_class;
    u16 interfaces_count;
    u16* interfaces;
    u16 fields_count;
    FieldInfo* field_info;
    u16 methods_count;
    MethodInfo* methods;
    u16 attributes_count;
    AttributeInfo* attributes;
private:
    bool load_constant_pool(std::string& bytes, i32& index);
    bool load_interfaces(std::string& bytes, i32& index);
    bool load_fields(std::string& bytes, i32& index);
    bool load_methods(std::string& bytes, i32& index);
    AttributeResult load_attributes(Location loc, std::string& bytes, i32& index);
    ExceptionTableResult load_exception_table(std::string& bytes, i32& index);

};

// big endian
template<typename T>
requires requires(T a) {
    { std::is_integral_v<T> };
    { std::is_floating_point_v<T> };
}
T read_from_bytes(char* bytes, i32& index) {
    T temp;
    std::memcpy(&temp, bytes + index, sizeof(T));
    temp = std::byteswap(temp);
    index += sizeof(T);
    return temp;
}

#endif // MINI_JVM_CLASS_FILE_H