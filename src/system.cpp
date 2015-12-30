#include "system.hpp"
#include "memory.hpp"
#include "symbol.hpp"
#include "object.hpp"

namespace lsvm {
namespace system {

using lsvm::object::package;

void init(){
    lsvm::memory::initialize();
    // initialize global package
    package* system = lsvm::object::new_package(lsvm::symbol::new_symbol("system"));

    // initialize Class
    lsvm::object::object_class* Class = lsvm::object::new_class(system,lsvm::symbol::new_symbol("Class"),null,0);
    Class->super = Class;
    lsvm::object::define_class(system,Class);

    lsvm::object::object_class* Object = lsvm::object::new_class(system,lsvm::symbol::new_symbol("Object"),null,0);
    Object->super = Object;
    lsvm::object::define_class(system,Object);
}

void stop(){
    lsvm::object::free_packages();
    lsvm::symbol::clear();
    lsvm::memory::deinitialize();
}

}
}
