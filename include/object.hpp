#ifndef OBJECT_H
#define OBJECT_H

#include "string.hpp"
#include "symbol.hpp"

namespace lsvm {
namespace bytecode {
    struct bytecode_op_t;
}

namespace object {

    using lsvm::symbol::symbol;
    using lsvm::string::string;

    typedef struct package_t {
        symbol* name;
        lsvm::symbol::symbolmap* classes;
        struct package_t* next;
    } package;

    typedef struct object_class_t {
        package* pkg;
        symbol* name;
        struct object_class_t* super;
        lsvm::symbol::symbolmap* methods;
        uint16_t fields_count;
        symbol* fields[];
    } object_class;

    struct object_t;
    struct message_frame_t;
    struct variable_t;
    struct block_t;

    typedef struct variable_t {
        object_class* cls;
        union {
            // value types
            bool b;
            int64_t i;
            double d;
            // reference types
            struct object_t* o;        
            string* s;
            package* pk;
            object_class* cl;
            symbol* sy;
            struct block_t* bl;        
        };
    } variable;

    typedef struct message_frame_t {
        struct message_frame_t* next;
        uint32_t ret_var_indx;
        struct lsvm::bytecode::bytecode_op_t* op;
        struct block_t* b;
        variable v[];    
    } message_frame ;

    typedef struct block_t {
        bool on_frame;
        struct lsvm::bytecode::bytecode_op_t* op;
        uint32_t v_args;
        uint32_t v_copy;
        uint32_t v_temp;
        variable v[];    
    } block;

    typedef struct object_t {
        object_class cls;
        variable fields[];
    } object;

    package* new_package(symbol* name);
    package* get_package(symbol* name);
    void free_packages();

    void define_class(package* pkg, object_class* cls);
    object_class* new_class(package* pkg, symbol* name, object_class* super, uint16_t fields_count);
    object_class* get_class(symbol* package_name, symbol* name);
    void free_class(object_class* cls);

    void define_method(object_class* cls, symbol* name, block* block);
    block* find_method(object_class* cls, symbol* name);

    message_frame* new_message_frame(block* b);
    void free_message_frame(message_frame* f);

    void set_nil(variable* v);
    void set_int(variable* v, int64_t val);
}
}

#endif
