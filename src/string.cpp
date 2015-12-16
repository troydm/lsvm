#include "common.hpp"
#include "string.hpp"
#include "memory.hpp"

namespace lsvm {
    namespace string {

// new string
string* new_string(){
    string* s = (string*) lsvm::memory::allocate(sizeof(string)+1);
    s->count = 0;
    s->size = 0;

    *(c_str(s)) = 0;

    return s;
}


string* new_string(const char* str){
    uint32_t count = 0;
    uint8_t* s = reinterpret_cast<uint8_t*>(const_cast<char*>(str));
    uint8_t* s2;
    uint8_t* s3;
    uint8_t* s4;
    while(*s != 0){
        if (*s < 0x80) {
            ++s;
        } else if (*s < 0xC2) {
            /* continuation or overlong 2-byte sequence */
            return null;
        } else if (*s < 0xE0) {
            /* 2-byte sequence */
            s2 = s+1;
            if ((*s2 & 0xC0) != 0x80)
                return null;
            s += 2;
        } else if (*s < 0xF0) {
            /* 3-byte sequence */
            s2 = s+1;
            if ((*s2 & 0xC0) != 0x80)
                return null;
            if (*s == 0xE0 && *s2 < 0xA0)
                return null; /* overlong */
            s3 = s2+1;
            if ((*s3 & 0xC0) != 0x80)
               return null; 
            s += 3;
        } else if (*s < 0xF5) {
            /* 4-byte sequence */
            s2 = s+1;
            if ((*s2 & 0xC0) != 0x80)
                return null;
            if (*s == 0xF0 && *s2 < 0x90)
                return null; /* overlong */
            if (*s == 0xF4 && *s2 >= 0x90)
                return null; /* > U+10FFFF */
            s3 = s2+1;
            if ((*s3 & 0xC0) != 0x80)
                return null;
            s4 = s3+1;
            if ((*s4 & 0xC0) != 0x80)
                return null;
            s += 4;
        } else {
            return null;
        }
        ++count; 
    }

    uint32_t size = static_cast<uint32_t>(s - reinterpret_cast<uint8_t*>(const_cast<char*>(str)));

    string* r = (string*) lsvm::memory::allocate(sizeof(string)+size+1);
    r->count = count;
    r->size = size;

    memcpy(c_str(r),str,size+1);

    return r;
}

string* new_string(std::string* s){
    return new_string(s->c_str());
}
        
string* new_string(string* s){
    string* r = (string*) lsvm::memory::allocate(sizeof(string)+s->size+1);
    r->count = s->count;
    r->size = s->size;

    memcpy(c_str(r),c_str(s),s->size+1);

    return r;
}

bool equals(string* s1, string* s2){
    if(s1->size != s2->size || s1->count != s2->count)
        return false;

    char* s1p = c_str(s1);
    char* s2p = c_str(s2);

    while(*s1p != '\0'){
        if(*s1p != *s2p)
            return false;
        ++s1p; ++s2p;
    }

    return *s2p == '\0' && *s1p == *s2p;
}
        
bool equals(string* s1, const char* s2){
    char* s1p = c_str(s1);
    char* s2p = const_cast<char*>(s2); 

    while(*s1p != '\0'){
        if(*s1p != *s2p)
            return false;
        ++s1p; ++s2p;
    }

    return *s2p == '\0' && *s1p == *s2p;

}
        
// free string
void free(string* s){
    lsvm::memory::retain(s);
}

    }
}
