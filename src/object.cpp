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
    pkg->classes = lsvm::symbol::new_symbolmap();
    *parent = pkg;
    return pkg;    
}

package* get_package(symbol* name){
    package* pkg = packages;
    do{
        if(lsvm::symbol::symbol_equals(pkg->name,name)){
            return pkg;
        }
        pkg = pkg->next;
    }while(pkg != null);

    return null;
}

void free_package(package* pkg){
    lsvm::hashmap::iterator it = lsvm::hashmap::get_iterator(pkg->classes);
    lsvm::hashmap::entry* e;
    while((e = lsvm::hashmap::next(&it)) != null){
        lsvm::object::free_class((object_class*)e->val);    
    }
    lsvm::symbol::free_symbolmap(pkg->classes);
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
    cls->methods = lsvm::symbol::new_symbolmap();

    for(uint16_t i = 0; i<fields_count; ++i){
        cls->fields[i] = null;
    }

    return cls;
}

object_class* get_class(symbol* package_name, symbol* name){
    package* pkg = get_package(package_name);
    if(pkg == null)
        return null;

    lsvm::hashmap::entry* e = lsvm::hashmap::get(pkg->classes,name);
    if(e == null)
        return null;
    else
        return (object_class*)e->val;
}

void free_class(object_class* cls){
    lsvm::symbol::free_symbolmap(cls->methods);
    lsvm::memory::retain(cls);
}

void define_method(object_class* cls, symbol* name, block* block){
    lsvm::hashmap::put(cls->methods,name,block);
}

block* find_method(object_class* cls, symbol* name){
    do {
        lsvm::hashmap::entry* e = lsvm::hashmap::get(cls->methods,name);
        if(e == null){
            cls = cls->super;
        }else{
            return (block*)e->val;
        }
    }while(cls != null);
    
    return null;    
}

}
}
