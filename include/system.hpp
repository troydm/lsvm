#ifndef SYSTEM_H
#define SYSTEM_H

#include "object.hpp"

namespace lsvm {
namespace system {

    using lsvm::object::block;
    using lsvm::object::variable;
    using lsvm::object::message_frame;
    using lsvm::symbol::symbol;

    typedef message_frame process;
    typedef uint32_t process_id;
    
    void init();
    void stop();

    process_id new_process(block* b);
    void free_process(process_id pid);
    void replace_current_process(process* p);
    void return_current_process();
    void run();

}
}

#endif
