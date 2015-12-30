#ifndef OBJECT_H
#define OBJECT_H

#include "string.hpp"
#include "symbol.hpp"
#include "hashmap.hpp"

namespace lsvm {
namespace object {

    using lsvm::symbol::symbol;
    using lsvm::string::string;

    typedef struct package_t {
        symbol* name;
        lsvm::hashmap::hashmap* classes;
        struct package_t* next;
    } package;

    typedef struct object_class_t {
        package* pkg;
        symbol* name;
        struct object_class_t* super;
        lsvm::hashmap::hashmap* methods;
        uint16_t fields_count;
        symbol* fields[];
    } object_class;

    struct object_t;

    typedef struct variable_t {
        object_class* cls;
        union {
            bool b;
            int64_t i;        
            double d;        
            struct object_t* o;        
            string* s;
            package* pk;
            object_class* cl;
            symbol* sy;
        };
    } variable;

    typedef struct object_t {
        object_class cls;
        variable fields[];
    } object;

    package* new_package(symbol* name);
    package* get_package(symbol* name);
    void free_packages();

    void define_class(package* pkg, object_class* cls);
    object_class* new_class(package* pkg, symbol* name, object_class* super, uint16_t fields_count);
    void free_class(object_class* cls);
}
}

#endif
