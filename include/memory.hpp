#ifndef MEMORY_H
#define MEMORY_H

#include <cstring>


namespace lsvm {
    namespace memory {

        /* initialize cache table */
        void initialize();

        /* deinitialize cache table */
        void deinitialize();

        /* allocate memory */
        void* allocate(size_t size);

        /* reallocate memory */
        void* reallocate(void* ptr, size_t size);

        /* free memory */
        void retain(void* ptr);

    }
}

#endif
