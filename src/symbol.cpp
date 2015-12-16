#include "common.hpp"
#include "symbol.hpp"
#include "memory.hpp"
#include "hashmap.hpp"
#include <stdio.h>

namespace lsvm {
namespace symbol {

// symbols table
lsvm::hashmap::hashmap* symbols = null;
lsvm::hashmap::iterator* symbols_it = null;

bool symbol_equals(void* sym1, void* sym2){
    return sym1 == sym2;
}

lsvm::hashmap::hash symbol_hash(void* s){
    return (lsvm::hashmap::hash)s;
}


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
    if(symbols == null){
        symbols = lsvm::hashmap::new_hashmap(&symbol_equals,&symbol_hash);
        symbols_it = lsvm::hashmap::new_iterator(symbols);
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
    
    return sym;
}


// symbol equals to symbol
bool equals(symbol* sym1, symbol* sym2){
    return sym1 == sym2;
}

// remove all symbols from symbol table
void remove_all(){
    if(symbols != null)
        lsvm::hashmap::clear(symbols);
}

}
}
