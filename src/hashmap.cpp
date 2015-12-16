#include <cstdio>
#include "hashmap.hpp"
#include "memory.hpp"

namespace lsvm {
namespace hashmap {

// prime numbers used for a hash table section
#define PRIME_MAX 24
const uint32_t primes[PRIME_MAX] = {29,53,97,193,389,769,1543,3079,6151,12289,24593,49157,98317,196613,393241,
                                    786433,1572869,3145739,6291469,12582917,25165843,50331653,100663319,201326611};

// Function wrappers for equals and hash
bool string_equals(void* i1, void* i2){
    return lsvm::string::equals((lsvm::string::string*)i1,(lsvm::string::string*)i2);
}

hash string_hash(void* p){
    return hash_string( (lsvm::string::string*)p );
}

#define pow2n(i)  (1 << (i))
#define hashbit(h,bit) (h & bit)
#define partition_count(bits) (bits+1)
#define entry_range(h,b) (h->range*bucket_size(b))/100000
#define entry_index(b,h) (h % bucket_size(b))
#define bucket_size(b) primes[(b)->bucket_size_indx]
#define load_factor(b) (((b)->count*100)/bucket_size(b))

inline hashbucket** buckets(hashmap* h) { return reinterpret_cast<hashbucket**>(h+1); }
inline hashbucket** partition(hashmap* h, uint16_t bit) { return buckets(h)+bit; }
inline hashentry* entries(hashbucket* b) { return reinterpret_cast<hashentry*>(b+1); }
inline hashentry* entry(hashbucket* b, uint32_t indx) { return entries(b) + indx; }
inline void insert_entry(hashmap* h, hashbucket* bp, hashentry* he, hash keyhash, void* key, void* val){ 
    he->keyhash = keyhash;
    he->key = key;
    he->val  = val;
    bp->count += 1;
    h->count += 1;
}
inline void clear_entry(hashmap* h, hashbucket* bp, hashentry* he){ 
    he->keyhash = 0;
    he->key = null;
    he->val = null;
    bp->count -= 1;
    h->count -= 1;
}

// find bucket index size
inline uint8_t find_bucket_index(const uint32_t size){
    uint8_t i = 0;
    while(i < PRIME_MAX){
        if(primes[i] >= size){
            return i;
        }
        ++i;
    }
        
    return PRIME_MAX-1;
}
        
hashmap* new_hashmap(equals_fp efp, hash_fp hfp, uint8_t bits, uint8_t max_bits, uint16_t max_depth, uint16_t range, uint32_t new_bucket_size, uint32_t avg_bucket_size){
    // check if functions are provided
    if(efp == null || hfp == null)
        return null;

    // check if bits are correct
    if(bits == 0 || bits > 16 || max_bits == 0 || max_bits < bits || max_bits > 16 || avg_bucket_size < 7 || range == 0)
        return null;

    // calculate max buckets
    uint32_t max_partitions = pow2n(max_bits);

    // allocate new hashmap
    hashmap* h = (hashmap*)lsvm::memory::allocate(sizeof(hashmap)+(max_partitions*sizeof(hashbucket*)));
    h->equals = efp;
    h->hash = hfp;
    h->count = 0;
    h->bits = pow2n(bits) - 1;
    h->max_bits = max_partitions - 1;
    h->max_depth = max_depth;
    h->range = range;

    // initialize all bucket pointers to null
    memset(buckets(h),null,max_partitions*sizeof(hashbucket*));
    
    // determine correct bucket sizes
    h->new_bucket_indx = find_bucket_index(new_bucket_size);
    h->avg_bucket_indx = find_bucket_index(avg_bucket_size);

    return h;
}

// create new bucket
inline hashbucket* new_bucket(uint8_t size_indx){
    const uint32_t size = primes[size_indx];
    hashbucket* b = (hashbucket*)lsvm::memory::allocate(sizeof(hashbucket)+(size*sizeof(hashentry)));
    b->next = null;
    b->count = 0;
    b->bucket_size_indx = size_indx;

    // initialize all bucket pointers to null
    memset(entries(b),null,size*sizeof(hashentry));
    
    return b;
}

// add new bucket to partition
inline hashbucket* add_new_bucket(hashmap* h, hashbucket** b, uint8_t new_size_indx){
    hashbucket* nb = new_bucket(new_size_indx);
    nb->next = *b;
    *b = nb;
    return *b;
}


// find a bigger size for a bucket
inline uint8_t find_greater_size(const uint32_t size){
    uint8_t i = 0;
    while(i < PRIME_MAX){
        if(primes[i] > size)
            return i;
        ++i;
    }
    return PRIME_MAX-1;
}

// find a twice bigger size for a bucket
inline uint8_t find_twice_greater_size(const uint32_t size){
    uint8_t i = 0;
    while(i < PRIME_MAX-1){
        if(primes[i] > size)
            return i+1;
        ++i;
    }
    return PRIME_MAX-1;
}

// for debugging use only
void print_partition(hashbucket** pb){
    hashbucket* b = *pb;
    while(b != null){
        printf("%p[%d/%d|%d] -> ",b,b->count,bucket_size(b),load_factor(b));    
        b = b->next;
    }
    printf("null\n");
}

// swap buckets
inline void swap_bucket(hashbucket** p1, hashbucket** p2){
    hashbucket* temp1 = *p1;
    hashbucket* temp2 = (*p2)->next;
    *p1 = *p2;
    *p2 = temp1;
    (*p1)->next = temp1->next;
    (*p2)->next = temp2;
}

// balance partition
void balance_partition(hashbucket** pb){
    hashbucket** bp = pb;
    hashbucket** b = bp;
    uint8_t lf = 100;
    while(*bp != null){
        uint8_t nlf = load_factor(*bp);
        if(nlf < lf){
            lf = nlf;
            b = bp;
        }
        bp = &((*bp)->next);
    }
    if(*pb != *b){
        // printf("\n");
        // print_partition(pb);
        // printf("swapping %p[%d] <> %p[%d] \n", *pb,load_factor(*pb), *b, load_factor(*b));
        swap_bucket(pb,b);
        // print_partition(pb);
        // printf("\n");
        if((*pb)->next != null)
            balance_partition(&((*pb)->next));
    }
}


// insert into partition
void insert_into_partition(hashmap* h, hash keyhash, uint16_t bit, void* key, void* val, bool allow_reshaffle);

// copy bucket into new bucket with new size
inline void copy_into_new_bucket(hashmap* h, hashbucket** pb, hashbucket** b, uint32_t new_size_indx){

    // remove current bucket from partition and add new bigger one
    hashbucket* ob = *b;
    *b = (*b)->next;
    add_new_bucket(h,pb,new_size_indx);
    h->count -= ob->count;
    
    // rebalance partition
    balance_partition(pb);

    // insert left-over keys into partition
    hashentry* he = entries(ob); 
    hashentry* ehe = entry(ob,bucket_size(ob)-1); 
    while(he <= ehe){
        if(he->key != null){
            insert_into_partition(h,he->keyhash,hashbit(he->keyhash,h->bits),he->key,he->val,true);
        }
        ++he;
    }
    
    // remove old bucket
    lsvm::memory::retain(ob);

    // rebalance partition
    balance_partition(pb);
}

// resize bucket or create a new one if can't resize
// note: will always return pointer to bucket
inline void resize_or_new_bucket(hashmap* h, uint16_t bit){
    // find out current partition bucket
    hashbucket** pb = partition(h,bit);
    // find less densiest bucket
    hashbucket** bp = pb;
    hashbucket** b = null;
    while(*bp != null){
        if(bucket_size(*bp) != primes[PRIME_MAX-1]){
            if(b == null || bucket_size(*b) > bucket_size(*bp)){
                b = bp;
            }
        }

        bp = &((*bp)->next);
    }
    if(b != null){
        uint8_t new_size_indx;
        if(load_factor(*b) >= 75){
            new_size_indx = find_greater_size(bucket_size(*b));
            //printf("resizing! lf:%d %d -> %d\n",lf,bucket_size(*b),primes[new_size_indx]);
        }else{
            new_size_indx = find_twice_greater_size(bucket_size(*b));
            //printf("resizing twice! lf:%d %d -> %d\n",lf,bucket_size(*b),primes[new_size_indx]);
        }
        if(primes[new_size_indx] != bucket_size(*b)){
            // copy all entries to new bucket and return it
            copy_into_new_bucket(h, pb, b, new_size_indx);
        }else{
            add_new_bucket(h,pb,h->new_bucket_indx);
        }
    }else{
        add_new_bucket(h,pb,h->new_bucket_indx);
    }
}


inline void reshuffle_partition(hashmap* h, uint16_t bit){
    hashbucket** pb = partition(h,bit); 
    hashbucket** bp = pb;
    // find entries to reshuffle
    while(*bp != null){
        hashentry* he = entries(*bp);
        hashentry* ehe = entry(*bp,bucket_size(*bp)-1);
        while(he <= ehe){
            if(he->key != null){
                uint16_t ebit = hashbit(he->keyhash,h->bits);
                if(ebit != bit){
                    insert_into_partition(h,he->keyhash,ebit,he->key,he->val,false);
                    clear_entry(h,*bp,he);
                }
            }
            ++he;
        }
        //remove empty bucket
        if((*bp)->count == 0){
            hashbucket* b = (*bp)->next;
            lsvm::memory::retain(*bp);
            *bp = b;
        }else{
            bp = &((*bp)->next);
        }
    }

    // rebalance partition
    balance_partition(pb);
}

void insert_into_partition(hashmap* h, hash keyhash, uint16_t bit, void* key, void* val, bool allow_reshaffle){
    hashbucket** pb = partition(h,bit);
    hashbucket* bp = *pb;
    // find empty entry in a bucket
    uint8_t depth = 0;
    while(bp != null){
        const uint32_t bs = bucket_size(bp);
        if(bs > bp->count){
            uint32_t range = entry_range(h,bp);
            if(range >= bs) range = bs-1;
            uint32_t index = entry_index(bp,keyhash);
            if((index+range) >= bs){
                hashentry* he = entry(bp,index);
                hashentry* ehe = entry(bp,bs-1);
                while(he <= ehe){
                    if(he->key == null){
                        insert_entry(h,bp,he,keyhash,key,val);
                        return;
                    }
                    ++he;
                }
                range = (index+range) - bs;
                he = entry(bp,0);
                ehe = he+range;
                while(he <= ehe){
                    if(he->key == null){
                        insert_entry(h,bp,he,keyhash,key,val);
                        return;
                    }
                    ++he;
                }
            }else{
                hashentry* he = entry(bp,index);
                hashentry* ehe = he+range;
                while(he <= ehe){
                    if(he->key == null){
                        insert_entry(h,bp,he,keyhash,key,val);
                        return;
                    }
                    ++he;
                }
            }
        }
        
        bp = bp->next;
        ++depth;
    }

    // check if max depth reached
    if(depth < h->max_depth){
        // new bucket added to partition
        bp = add_new_bucket(h,pb,h->new_bucket_indx);
        hashentry* he = entry(bp,entry_index(bp,keyhash));
        insert_entry(h,bp,he,keyhash,key,val);
    }else{
        // check if max bits reached
        if(h->bits != h->max_bits && allow_reshaffle){
            // find out minimum bucket size in partition
            uint8_t min_bucket_size = PRIME_MAX-1;
            bp = *pb;
            while(bp != null){
                if(bp->bucket_size_indx < min_bucket_size)
                    min_bucket_size = bp->bucket_size_indx;
                bp = bp->next;
            }
            if(min_bucket_size >= h->avg_bucket_indx){
                // add bit and reshuffle entries
                uint32_t max_partitions = partition_count(h->bits);
                uint32_t new_bits = h->bits;
                new_bits = (2*new_bits)+1;
                h->bits = new_bits;
                //printf("reshuffling by %d\n",h->count);
                uint8_t old_new_bucket_indx = h->new_bucket_indx;
                h->new_bucket_indx = min_bucket_size;
                // reshuffle all partitions
                uint32_t i = 0;
                while(i < max_partitions){
                    reshuffle_partition(h,i);
                    ++i;
                }
                h->new_bucket_indx = old_new_bucket_indx;
            }else{
                // continue resizing until we get a suitable bucket for a key
                resize_or_new_bucket(h, hashbit(keyhash,h->bits));
            }
        }else{
            // continue resizing until we get a suitable bucket for a key
            resize_or_new_bucket(h, hashbit(keyhash,h->bits));
        }
        // after reshuffling or resizing try to insert key again
        insert_into_partition(h, keyhash, hashbit(keyhash,h->bits), key, val, false);
    }
}

void put(hashmap* h, void* key, void* val){
    // calculate hash for a key
    hash keyhash = h->hash(key);
    // calculate bit for a hash
    uint16_t bit = hashbit(keyhash,h->bits);
    // get bit partition bucket
    hashbucket* bp = *partition(h,bit);

    // find key in a bucket using open address and replace it's value
    while(bp != null){
        uint32_t range = entry_range(h,bp);
        uint32_t index = entry_index(bp,keyhash);
        hashentry* he = entry(bp,index);
        const uint32_t bs = bucket_size(bp);
        if(range >= bs) range = bs-1;
        if((index+range) >= bs){
            hashentry* ehe = entry(bp,bs-1);
            while(he <= ehe){
                if(he->keyhash == keyhash && he->key != null && h->equals(key,he->key)){
                    he->val = val;
                    return;
                }
                ++he;
            }
            range = (index+range) - bs;
            he = entry(bp,0);
            ehe = he+range;
            while(he <= ehe){
                if(he->keyhash == keyhash && he->key != null && h->equals(key,he->key)){
                    he->val = val;
                    return;
                }
                ++he;
            }
        }else{
            hashentry* ehe = he + range;
            while(he <= ehe){
                if(he->keyhash == keyhash && he->key != null && h->equals(key,he->key)){
                    he->val = val;
                    return;
                }
                ++he;
            }
        }
        bp = bp->next;    
    }

    // if key not found insert it into partition
    insert_into_partition(h,keyhash,bit,key,val,true);
}

// get val for key
void* get(hashmap* h, void* key){
    // calculate hash for a key
    hash keyhash = h->hash(key);
    // calculate bit for a hash
    uint16_t bit = hashbit(keyhash,h->bits);
    // get bit partition bucket
    hashbucket* bp = *(partition(h,bit));

    // find key in a bucket using open address
    while(bp != null){
        uint32_t range = entry_range(h,bp);
        uint32_t index = entry_index(bp,keyhash);
        hashentry* he = entry(bp,index);
        const uint32_t bs = bucket_size(bp);
        if(range >= bs) range = bs-1;
        if((index+range) >= bs){
            hashentry* ehe = entry(bp,bs-1);
            while(he <= ehe){
                if(he->keyhash == keyhash && he->key != null && h->equals(key,he->key))
                    return he->val;
                ++he;
            }
            range = (index+range) - bs;
            he = entry(bp,0);
            ehe = he + range;
            while(he <= ehe){
                if(he->keyhash == keyhash && he->key != null && h->equals(key,he->key))
                    return he->val;
                ++he;
            }
        }else{
            hashentry* ehe = he + range;
            while(he <= ehe){
                if(he->keyhash == keyhash && he->key != null && h->equals(key,he->key))
                    return he->val;
                ++he;
            }
        }
        bp = bp->next;    
    }

    return null;
}

// remove val for key
void* remove(hashmap* h, void* key){
    // calculate hash for a key
    hash keyhash = h->hash(key);
    // calculate bit for a hash
    uint16_t bit = hashbit(keyhash,h->bits);
    // get bit partition bucket
    hashbucket* bp = *(partition(h,bit));

    // find key in a bucket using open address
    while(bp != null){
        uint32_t range = entry_range(h,bp);
        uint32_t index = entry_index(bp,keyhash);
        hashentry* he = entry(bp,index);
        const uint32_t bs = bucket_size(bp);
        if(range >= bs) range = bs-1;
        if((index+range) >= bs){
            hashentry* ehe = entry(bp,bs-1);
            while(he <= ehe){
                if(he->keyhash == keyhash && he->key != null && h->equals(key,he->key)){
                    void* val = he->val;
                    clear_entry(h,bp,he);
                    return val;
                }
                ++he;
            }
            range = (index+range) - bs;
            he = entry(bp,0);
            ehe = he + range;
            while(he <= ehe){
                if(he->keyhash == keyhash && he->key != null && h->equals(key,he->key)){
                    void* val = he->val;
                    clear_entry(h,bp,he);
                    return val;
                }
                ++he;
            }
        }else{
            hashentry* ehe = he + range;
            while(he <= ehe){
                if(he->keyhash == keyhash && he->key != null && h->equals(key,he->key)){
                    void* val = he->val;
                    clear_entry(h,bp,he);
                    return val;
                }
                ++he;
            }
        }
        bp = bp->next;    
    }

    return null;
}

// free bucket and all it's sub-buckets
void free_bucket(hashbucket* b){
    if(b != null){
        hashbucket* next = b->next;
        lsvm::memory::retain(b);
        free_bucket(next);
    }
}

// free hashmap
void free(hashmap* h){
    // calculate current max partitions
    uint32_t max_partitions = partition_count(h->bits);

    // free all buckets
    uint32_t i = 0;
    hashbucket** bp = buckets(h);
    while(i < max_partitions){
        free_bucket(bp[i]);    
        ++i;
    }

    // free hashmap
    lsvm::memory::retain(h);
}

// clear hashmap
void clear(hashmap* h){
    // calculate current max partitions
    uint32_t max_partitions = partition_count(h->bits);

    // free all buckets
    uint32_t i = 0;
    hashbucket** bp = buckets(h);
    while(i < max_partitions){
        free_bucket(bp[i]);    
        bp[i] = null;
        ++i;
    }

    h->count = 0;
}
        
void print(hashmap* h){
    printf("hashmap: [count:%d][partitions:%d/%d][max_depth:%d][new_bucket_size:%d,range=%d,avg_bucket_size:%d]\npartitions: [",h->count, h->bits,h->max_bits, h->max_depth, primes[h->new_bucket_indx],h->range,primes[h->avg_bucket_indx]);
    
    uint32_t max_partitions = partition_count(h->bits);
    hashbucket** bp = buckets(h);
    uint32_t i = 0;
    while(i < max_partitions){
        printf(" %d:%p ",i,bp[i]);
        ++i;
    }
    printf("]\n");

    i = 0;
    uint32_t e = 0;
    float lf_sum = 0.0f; float lf_sumi = 0;
    while(i < max_partitions){
        if(bp[i] != null){
            hashbucket* b = bp[i];
            printf("%d: %p - ",i,b);
            uint32_t sum_count = 0;
            while(b != null){
                float lf = ((float)b->count)/bucket_size(b);
                lf_sum  += lf; lf_sumi += 1;
                printf("[count:%d,bucket_size:%d,load_factor=%.2f] ",b->count,bucket_size(b),lf);
                e += bucket_size(b)-b->count;
                sum_count += b->count;
                b = b->next;
            }
        
            printf("[sum_count=%d]\n",sum_count);
        }
        ++i;
    }
    printf("count=%d, empty=%d, load_factor=%.2f, avg_load_factor=%.2f\n",h->count,e,((float)h->count)/(e+h->count),lf_sum/lf_sumi);
}


// new iterator
iterator* new_iterator(hashmap* h){
    iterator* hi = reinterpret_cast<iterator*>(lsvm::memory::allocate(sizeof(iterator)));
    hi->h= h;
    reset_iterator(hi);
    return hi;
}

// reset iterator
void reset_iterator(iterator* hi){
    hi->bit = 0;
    hi->b = null;
    hi->he = null;
    hi->ehe = null;
}

// free hashmap iterator
void free_iterator(iterator* hi){
    lsvm::memory::retain(hi);
}

// current iterator entry
hashentry* current(iterator* hi){
    return hi->he;
}
        
hashentry* next(iterator* hi){
    if(hi->he < hi->ehe){
        // move to next entry
        ++(hi->he);
        while(hi->he->key == null && hi->he < hi->ehe){
            ++(hi->he);
        }
        if(hi->he == hi->ehe){
            return next(hi);
        }else{
            return hi->he;
        }
    }else if(hi->b == null){
        hi->b = *(partition(hi->h,hi->bit));
        if(hi->b == null){
            hi->bit += 1;
            if(hi->h->max_bits < hi->bit)
                return null;
            return next(hi);
        }else{
            hi->he = entry(hi->b,0);
            hi->ehe = entry(hi->b,bucket_size(hi->b));
            while(hi->he->key == null && hi->he < hi->ehe){
                ++(hi->he);
            }
            if(hi->he == hi->ehe){
                return next(hi);
            }else{
                return hi->he;
            }
        }
    }else{
        hi->b = hi->b->next;
        if(hi->b == null){
            hi->bit += 1;
            if(hi->h->max_bits < hi->bit)
                return null;
            return next(hi);
        }else{
            hi->he = entry(hi->b,0);
            hi->ehe = entry(hi->b,bucket_size(hi->b));
            while(hi->he->key == null && hi->he < hi->ehe){
                ++(hi->he);
            }
            if(hi->he == hi->ehe){
                return next(hi);
            }else{
                return hi->he;
            }
        }
    }
}


}
}
