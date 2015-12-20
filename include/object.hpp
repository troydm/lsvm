#ifndef OBJECT_H
#define OBJECT_H

namespace lsvm {
namespace string {
    struct string_t;
}

namespace object {

    typedef struct {
        struct lsvm::string::string_t* name;
    } package;

    typedef struct {
        package* pkg;
        struct lsvm::string::string_t* name;
    } obj_class;

    typedef struct {
        obj_class cls;
    } object;

}
}

#endif
