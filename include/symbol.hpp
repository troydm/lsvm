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
        typedef struct symbol_table_t {
            uint32_t count;
            struct symbol_table_t* next;        
            symbol* end;
        } symbol_table;


        // add symbol to symbols table
        symbol* new_symbol(const char* symbol);
        symbol* new_symbol(std::string* symbol);
        symbol* new_symbol(lsvm::string::string* symbol);

        // symbol equals to symbol
        inline bool equals(symbol* sym1, symbol* sym2) { return sym1 == sym2; }
        bool symbol_equals(void* sym1, void* sym2);
        lsvm::hashmap::hash symbol_hash(void* s);
    }
}


#endif
