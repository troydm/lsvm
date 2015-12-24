#ifndef HASHSET_H
#define HASHSET_H

#include "hashmap.hpp"

namespace lsvm {
namespace hashset {

    using lsvm::hashmap::hash;
    using lsvm::hashmap::equals_fp;
    using lsvm::hashmap::hash_fp;
    using lsvm::hashmap::hashbucket;

    typedef struct {
        hash hash;
        void* val;
    } entry;

    typedef struct {
        equals_fp equals;
        hash_fp hash;
        uint32_t count;
        uint8_t new_bucket_indx, avg_bucket_indx;
        uint16_t bits, max_bits, max_depth, range;
    } hashset;

    typedef struct {
        hashset* h;
        uint8_t bit;
        hashbucket* b;
        entry* he;
        entry* ehe;
    } iterator;


    // allocate new hashset
    hashset* new_hashset(equals_fp efp, hash_fp hfp, uint8_t bits=1, uint8_t max_bits=3, uint16_t max_depth=3, uint16_t range=100, uint32_t new_bucket_size=7, uint32_t avg_bucket_size=10949);

    // count keys
    inline uint32_t count(hashset* h) { return h->count; }

    // put val into hashset
    void put(hashset* h, void* val);

    // get val entry from hashset
    entry* get(hashset* h, void* val);

    // remove val from hashset
    void* remove(hashset* h, void* val);

    // free hashset
    void free(hashset* h);

    // clear hashset
    void clear(hashset* h);

    // print hashset stats
    void print(hashset* h);


    // iterator functions
    iterator get_iterator(hashset* h);
    void reset_iterator(iterator* hi);
    entry* current(iterator* hi);
    entry* next(iterator* hi);
}
}


#endif
