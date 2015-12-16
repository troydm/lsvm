#include "common.hpp"
#include "symbol.hpp"
#include "memory.hpp"
#include "hashmap.hpp"
#include <stdio.h>

namespace lsvm {
namespace symbol {

// symbols table
symbol_table* symbols[256] = { NULL };

// add symbol to symbols table
symbol* new_symbol(const char* symbol){
    lsvm::string::string* s = lsvm::string::new_string(symbol);
    lsvm::symbol::symbol* sym = new_symbol(s);
    lsvm::string::free(s);
    return sym;
}
    
symbol* new_symbol(std::string* symbol){
    lsvm::string::string* s = lsvm::string::new_string(symbol->c_str());
    lsvm::symbol::symbol* sym = new_symbol(s);
    lsvm::string::free(s);
    return sym;
}

symbol* new_symbol(lsvm::string::string* symbol){
    if(lsvm::string::size(symbol) == 0)
        return null;

    /*
    if(symbols == null){
    }
    lsvm::hashmap::reset_iterator(symbols_it);
    while(lsvm::hashmap::next(symbols_it) != null){
        lsvm::symbol::symbol* s = (lsvm::symbol::symbol*)lsvm::hashmap::current(symbols_it)->key; 
        if(lsvm::string::equals(s, symbol)){
            return s;
        }
    }

    lsvm::string::string* sym = lsvm::string::new_string(symbol);
    lsvm::hashmap::put(symbols,sym,null);
    */
    
    return null;
}

bool symbol_equals(void* sym1, void* sym2){
    return sym1 == sym2;
}

lsvm::hashmap::hash symbol_hash(void* s){
    return (lsvm::hashmap::hash)s;
}

}
}
