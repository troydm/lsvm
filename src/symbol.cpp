#include "common.hpp"
#include "symbol.hpp"
#include "memory.hpp"
#include "hashset.hpp"
#include <stdio.h>

namespace lsvm {
namespace symbol {

// symbols table
lsvm::hashset::hashset* symbols[256] = { NULL };

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

    lsvm::hashset::hashset* table = symbols[c];

    if(table == null){
        table = lsvm::hashset::new_hashset(&lsvm::hashmap::string_equals,&lsvm::hashmap::string_hash);   
        symbols[c] = table;
        lsvm::string::string* s = lsvm::string::new_string(symbol);
        lsvm::hashset::put(table,s);
        return s;
    }else{
        lsvm::hashset::entry* entry = lsvm::hashset::get(table,symbol);
        if(entry == null){
            lsvm::string::string* s = lsvm::string::new_string(symbol);
            lsvm::hashset::put(table,s);
            return s;
        }else{
            return reinterpret_cast<lsvm::symbol::symbol*>(entry->val);
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

    lsvm::hashset::hashset* table = symbols[c];

    if(table == null)
        return null;

    lsvm::hashset::entry* entry = lsvm::hashset::get(table,symbol);
    if(entry == null)
        return null;
    else
        return reinterpret_cast<lsvm::symbol::symbol*>(entry->val);
}

void clear(){
    for(uint16_t i = 0; i < 256; ++i){
        lsvm::hashset::hashset* table = symbols[i];
        if(table != null){

            lsvm::hashset::iterator it = lsvm::hashset::get_iterator(table);
            lsvm::hashset::entry* he = lsvm::hashset::next(&it);
            while(he != null){
                lsvm::string::free(reinterpret_cast<lsvm::string::string*>(he->val));            
                he = lsvm::hashset::next(&it);
            }
        
            lsvm::hashset::free(table);
            symbols[i] = null;
        }
    }
}

bool symbol_equals(void* sym1, void* sym2){
    return sym1 == sym2;
}

lsvm::hashset::hash symbol_hash(void* s){
    return (lsvm::hashset::hash)s;
}

}
}
