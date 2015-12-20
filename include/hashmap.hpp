#ifndef HASHMAP_H
#define HASHMAP_H

#include "common.hpp"
#include "string.hpp"

// csiphash function
extern "C" {
    uint64_t hash(const void *src, unsigned long src_sz);
}

namespace lsvm {
namespace hashmap {
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

        typedef struct {
            hash keyhash;
            void* key;
            void* val;
        } hashentry;

        typedef struct hashbucket_t {
            struct hashbucket_t* next;
            uint32_t count;
            uint32_t bucket_size_indx;
        } hashbucket;

        typedef struct {
            equals_fp equals;
            hash_fp hash;
            uint32_t count;
            uint8_t new_bucket_indx, avg_bucket_indx;
            uint16_t bits, max_bits, max_depth, range;
        } hashmap;

        typedef struct {
            hashmap* h;
            uint8_t bit;
            hashbucket* b;
            hashentry* he;
            hashentry* ehe;
        } iterator;


        // allocate new hashmap
        hashmap* new_hashmap(equals_fp efp, hash_fp hfp, uint8_t bits=1, uint8_t max_bits=3, uint16_t max_depth=3, uint16_t range=100, uint32_t new_bucket_size=7, uint32_t avg_bucket_size=10949);

        // count keys
        inline uint32_t count(hashmap* h) { return h->count; }

        // put key into hashmap
        void put(hashmap* h, void* key, void* val);

        // get val for key
        hashentry* get(hashmap* h, void* key);
       
        // remove val for key
        void* remove(hashmap* h, void* key);

        // free hashmap
        void free(hashmap* h);
        
        // clear hashmap
        void clear(hashmap* h);

        // print hashmap stats
        void print(hashmap* h);


        // iterator functions
        iterator get_iterator(hashmap* h);
        void reset_iterator(iterator* hi);
        hashentry* current(iterator* hi);
        hashentry* next(iterator* hi);
}
}


#endif
