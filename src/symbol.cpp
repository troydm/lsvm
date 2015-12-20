#include "common.hpp"
#include "symbol.hpp"
#include "memory.hpp"
#include "hashmap.hpp"
#include <stdio.h>

namespace lsvm {
namespace symbol {

// symbols table
lsvm::hashmap::hashmap* symbols[256] = { NULL };

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
        throw "string empty";

    lsvm::string::iterator it = lsvm::string::get_iterator(symbol);
    lsvm::string::string_char c = lsvm::string::next(&it);

    if(c > 255)
        throw "invalid symbol";

    lsvm::hashmap::hashmap* table = symbols[c];

    if(table == null){
        table = lsvm::hashmap::new_hashmap(&lsvm::hashmap::string_equals,&lsvm::hashmap::string_hash);   
        symbols[c] = table;
        lsvm::string::string* s = lsvm::string::new_string(symbol);
        lsvm::hashmap::put(table,s,null);
        return s;
    }else{
        lsvm::hashmap::hashentry* entry = lsvm::hashmap::get(table,symbol);
        if(entry == null){
            lsvm::string::string* s = lsvm::string::new_string(symbol);
            lsvm::hashmap::put(table,s,null);
            return s;
        }else{
            return reinterpret_cast<lsvm::symbol::symbol*>(entry->key);
        }
    }
}

symbol* get_symbol(const char* symbol){
    lsvm::string::string* s = lsvm::string::new_string(symbol);
    lsvm::symbol::symbol* sym = get_symbol(s);
    lsvm::string::free(s);
    return sym;
}
    
symbol* get_symbol(std::string* symbol){
    lsvm::string::string* s = lsvm::string::new_string(symbol->c_str());
    lsvm::symbol::symbol* sym = get_symbol(s);
    lsvm::string::free(s);
    return sym;
}

symbol* get_symbol(lsvm::string::string* symbol){
    if(symbol->count == 0)
        throw "string empty";

    lsvm::string::iterator it = lsvm::string::get_iterator(symbol);
    lsvm::string::string_char c = lsvm::string::next(&it);

    if(c > 255)
        throw "invalid symbol";

    lsvm::hashmap::hashmap* table = symbols[c];

    if(table == null)
        return null;

    lsvm::hashmap::hashentry* entry = lsvm::hashmap::get(table,symbol);
    if(entry == null)
        return null;
    else
        return reinterpret_cast<lsvm::symbol::symbol*>(entry->key);
}

bool symbol_equals(void* sym1, void* sym2){
    return sym1 == sym2;
}

lsvm::hashmap::hash symbol_hash(void* s){
    return (lsvm::hashmap::hash)s;
}

}
}
