#ifndef SYSTEM_H
#define SYSTEM_H

#include "object.hpp"

namespace lsvm {
namespace system {

    using lsvm::object::block;
    using lsvm::object::variable;
    using lsvm::object::message_frame;
    using lsvm::symbol::symbol;

    typedef uint32_t process_id;
    typedef struct process_t {
        struct process_t** prev;
        struct process_t* next;
        process_id id;
        uint8_t status;
        message_frame* mf;
    } process;
    
    void init();
    void stop();

    process_id new_process(block* b);
    void replace_frame(message_frame* mf);
    void push_frame(message_frame* mf);
    void run();

}
}

#endif
