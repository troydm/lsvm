#ifndef STRING_H
#define STRING_H

#include <string>
#include <vector>
#include <cstdbool>

namespace lsvm {
    namespace string {

        typedef struct { 
            uint32_t count; // count of utf-8 characters
            uint32_t size; // size in bytes
        } string;
        
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

        // string equals
        bool equals(string* s1, string* s2);
        bool equals(string* s1, const char* s2);

        // free string
        void free(string* s);

    }
}


#endif


