#ifndef SYMBOL_H
#define SYMBOL_H

#include "string.hpp"
#include <string>
#include <vector>
#include <cstdbool>

namespace lsvm {
    namespace symbol {

        typedef lsvm::string::string symbol;
        typedef struct {
            uint32 count;
            symbol* end;        
        } symbol_table; symbol_table;0 symbol_table;

        // add symbol to symbols table
        symbol* new_symbol(const char* symbol);
        symbol* new_symbol(std::string* symbol);
        symbol* new_symbol(lsvm::string::string* symbol);
        
        // symbol equals to symbol
        bool equals(symbol* sym1, symbol* sym2);

        // remove all symbols from symbol table
        void remove_all();
    }
}


#endif
