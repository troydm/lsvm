#include "common.hpp"
#include "memory.hpp"
#include <cstdlib>
#include <cstdio>
#include <malloc/malloc.h>

namespace lsvm {
namespace memory {

        size_t cache_size = 257; // cache size
        size_t max_cached = 8388608; // default value = 8mb

        size_t cached = 0;
        void** cache; // first byte is not used

        /* initialize cache table */
        void initialize(){
            cache = static_cast<void**>(malloc(sizeof(void*)*cache_size));
            memset(cache,0,sizeof(size_t)*cache_size);
        }

        /* deinitialize cache table */
        void deinitialize(){
            for(size_t i = 1; i < cache_size; i++){
                while(cache[i] != 0){
                    void* p = cache[i];
                    cache[i] = *(static_cast<void**>(p));
                    free(p);
                }
            }
            cached = 0;
            free(cache);
            cache = 0;
        }

        /* allocate memory */
        void* allocate(size_t size){
            if(size < cache_size){
                if(cache[size] == null){
                    if(size < sizeof(void*))
                        size = sizeof(void*);
                    return malloc(size);
                }else{
                    void* p = cache[size];
                    cache[size] = *(static_cast<void**>(p));
                    cached -= size;
                    return p;
                }
            }else{
                return malloc(size);
            }
        }

        /* reallocate memory */
        void* reallocate(void* ptr, size_t size){
            if(size < cache_size){
                if(cache[size] == null){
                    if(size < sizeof(void*))
                        size = sizeof(void*);
                    return realloc(ptr,size);
                }else{
                    void* p = cache[size];
                    cache[size] = *(static_cast<void**>(p));
                    cached -= size;
                    memcpy(p,ptr,size);
                    retain(ptr);
                    return p;
                }
            }else{
                return realloc(ptr,size);
            }
        }

        /* free memory */
        void retain(void* ptr){
            size_t size = malloc_size(ptr);
            if(size < cache_size){
                if(cached >= max_cached){
                    free(ptr);
                }else{
                    void** p = static_cast<void**>(ptr);
                    *p = cache[size];
                    cache[size] = ptr;
                    cached += size;
                }
            }else{
                free(ptr);
            }
        }

}
}
