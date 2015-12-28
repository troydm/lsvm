#include "hash.hpp"

namespace lsvm {
namespace hash {

// Function wrappers for equals and hash
bool string_equals(void* i1, void* i2){
    return lsvm::string::equals((lsvm::string::string*)i1,(lsvm::string::string*)i2);
}

hash string_hash(void* p){
    return hash_string( (lsvm::string::string*)p );
}

}
}
