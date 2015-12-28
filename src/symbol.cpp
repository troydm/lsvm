#include "common.hpp"
#include "symbol.hpp"
#include "memory.hpp"
#include "hashset.hpp"
#include <stdio.h>

namespace lsvm {
namespace symbol {

#define alpha1(c) (0x41 <= c && c <= 0x5A)
#define alpha2(c) (0x61 <= c && c <= 0x7A)
    
// symbols table
lsvm::hashset::hashset* symbols_alpha[52] = { null };
lsvm::hashset::hashset* symbols_other = null;

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

    lsvm::hashset::hashset* table;
    if(alpha1(c)){
        table = symbols_alpha[c-0x41];
        if(table == null){
            table = lsvm::hashset::new_hashset(&lsvm::hash::string_equals,&lsvm::hash::string_hash);   
            symbols_alpha[c-0x41] = table;
        }
    }else if(alpha2(c)){
        table = symbols_alpha[c-0x61+0x1A];
        if(table == null){
            table = lsvm::hashset::new_hashset(&lsvm::hash::string_equals,&lsvm::hash::string_hash);   
            symbols_alpha[c-0x61+0x1A] = table;
        }
    }else{
        table = symbols_other;
        if(table == null){
            table = lsvm::hashset::new_hashset(&lsvm::hash::string_equals,&lsvm::hash::string_hash);   
            symbols_other = table;
        }
    }

    lsvm::hashset::entry* entry = lsvm::hashset::get(table,symbol);
    if(entry == null){
        lsvm::string::string* s = lsvm::string::new_string(symbol);
        lsvm::hashset::put(table,s);
        return s;
    }else{
        return reinterpret_cast<lsvm::symbol::symbol*>(entry->val);
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

    lsvm::hashset::hashset* table;

    if(alpha1(c)){
        table = symbols_alpha[c-0x41];
    }else if(alpha2(c)){
        table = symbols_alpha[c-0x61+0x1A];
    }else{
        table = symbols_other;
    }

    if(table == null)
        return null;

    lsvm::hashset::entry* entry = lsvm::hashset::get(table,symbol);
    if(entry == null)
        return null;
    else
        return reinterpret_cast<lsvm::symbol::symbol*>(entry->val);
}

void clear(){
    for(uint16_t i = 0; i < 52; ++i){
        lsvm::hashset::hashset* table = symbols_alpha[i];
        if(table != null){

            lsvm::hashset::iterator it = lsvm::hashset::get_iterator(table);
            lsvm::hashset::entry* he = lsvm::hashset::next(&it);
            while(he != null){
                lsvm::string::free(reinterpret_cast<lsvm::string::string*>(he->val));            
                he = lsvm::hashset::next(&it);
            }
        
            lsvm::hashset::free(table);
            symbols_alpha[i] = null;
        }
    }

    if(symbols_other != null){

        lsvm::hashset::iterator it = lsvm::hashset::get_iterator(symbols_other);
        lsvm::hashset::entry* he = lsvm::hashset::next(&it);
        while(he != null){
            lsvm::string::free(reinterpret_cast<lsvm::string::string*>(he->val));            
            he = lsvm::hashset::next(&it);
        }

        lsvm::hashset::free(symbols_other);
        symbols_other = null;
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
