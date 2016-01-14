#ifndef STRING_H
#define STRING_H

#include "common.hpp"
#include "string.hpp"

namespace lsvm {
namespace string {

    typedef uint32_t string_char;

    typedef struct string_t { 
        uint32_t count; // number of utf-8 characters
        uint32_t size; // size in bytes
    } string;

    typedef struct {
        string* s;
        int32_t p;
    } iterator;

    // new string
    string* new_string();
    string* new_string(const char* s);
    string* new_string(std::string* s);
    string* new_string(string* s);

    inline uint32_t size(string* s){ return s->size; }
    inline uint32_t count(string* s){ return s->count; }

    inline char* c_str(string* s){
        return reinterpret_cast<char*>(reinterpret_cast<uint8_t*>(s) + sizeof(string));
    }

    // iterator
    iterator get_iterator(string* s);
    void reset_iterator(iterator* it);
    string_char next(iterator* it);

    // access
    string_char char_at(string* s, uint32_t index);

    // string equals
    bool equals(string* s1, string* s2);
    bool equals(string* s1, const char* s2);

    // free string
    void free(string* s);

    // char
    bool is_upper(string_char c);

}
}


#endif
