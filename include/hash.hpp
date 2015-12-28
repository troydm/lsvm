#ifndef HASH_H
#define HASH_H

#include "common.hpp"
#include "string.hpp"

// csiphash function
extern "C" {
    uint64_t hash(const void *src, unsigned long src_sz);
}

namespace lsvm {
namespace hash {

    typedef uint64_t hash;

    // Equality and hash function pointer
    typedef bool (*equals_fp)(void* i1, void* i2);
    typedef hash (*hash_fp)(void* p);

    // Hash functions
    inline hash hash_chr(const char* str){
        return ::hash(str,strlen(str)+1);
    }

    inline hash hash_str(std::string* str){
        return ::hash(str->data(),str->size()+1);
    }

    inline hash hash_string(lsvm::string::string* str){
        return ::hash(lsvm::string::c_str(str),str->size+1);
    }

    // Function wrappers for hashmap
    bool string_equals(void* i1, void* i2);
    hash string_hash(void* p);

}
}

#endif
