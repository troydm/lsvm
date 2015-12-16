#ifndef OBJECT_H
#define OBJECT_H

namespace lsvm {
    namespace object {

        enum type { NIL_TYPE=0, OBJECT_TYPE=1, INTEGER=2, FLOAT=3, SYMBOL=4, STRING=5 };

        typedef struct {
            unsigned int ref_count;
        } object;

    }
}


#endif
