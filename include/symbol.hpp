#ifndef SYMBOL_H
#define SYMBOL_H

#include "string.hpp"
#include "hashmap.hpp"
#include <string>
#include <vector>
#include <cstdbool>

namespace lsvm {
namespace symbol {

        typedef lsvm::string::string symbol;

        // add symbol to symbols table
        symbol* new_symbol(const char* symbol);
        symbol* new_symbol(std::string* symbol);
        symbol* new_symbol(lsvm::string::string* symbol);
        
        // lookup
        symbol* get_symbol(const char* symbol);
        symbol* get_symbol(std::string* symbol);
        symbol* get_symbol(lsvm::string::string* symbol);

        // symbol equals to symbol
        inline bool equals(symbol* sym1, symbol* sym2) { return sym1 == sym2; }
        inline lsvm::hashmap::hash hash(symbol* s){ return (lsvm::hashmap::hash)s; }
        bool symbol_equals(void* sym1, void* sym2);
        lsvm::hashmap::hash symbol_hash(void* s);
}
}


#endif
