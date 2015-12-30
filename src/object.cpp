#include "object.hpp"
#include "memory.hpp"
#include "hashmap.hpp"


namespace lsvm {
namespace object {

package* packages = null;

package* new_package(symbol* name){

    package** parent = &packages;
    if(*parent != null){
        do{
            parent = &((*parent)->next);
        }while(*parent != null);
    }

    package* pkg = (package*)lsvm::memory::allocate(sizeof(package));
    pkg->name = name;
    pkg->classes = lsvm::hashmap::new_hashmap(&lsvm::symbol::symbol_equals,&lsvm::symbol::symbol_hash);   
    *parent = pkg;
    return pkg;    
}

void free_package(package* pkg){
    lsvm::hashmap::iterator it = lsvm::hashmap::get_iterator(pkg->classes);
    lsvm::hashmap::entry* e;
    while((e = lsvm::hashmap::next(&it)) != null){
        lsvm::object::free_class((object_class*)e->val);    
    }
    lsvm::hashmap::free(pkg->classes);
    lsvm::memory::retain(pkg);
}

void free_packages(){
    package* pkg = packages;
    while(pkg != null){
        package* to_free = pkg;
        pkg = pkg->next;
        free_package(to_free);
    }
    packages = null;
}

void define_class(package* pkg, object_class* cls){
    lsvm::hashmap::put(pkg->classes,cls->name,cls);
}

object_class* new_class(package* pkg, symbol* name, object_class* super, uint16_t fields_count){
    object_class* cls = (object_class*)lsvm::memory::allocate(sizeof(object_class)+(fields_count*sizeof(symbol*)));
    cls->pkg = pkg;
    cls->name = name;
    cls->super = super;
    cls->fields_count = fields_count;

    for(uint16_t i = 0; i<fields_count; ++i){
        cls->fields[i] = null;
    }

    return cls;
}

void free_class(object_class* cls){
    lsvm::memory::retain(cls);
}

}
}
