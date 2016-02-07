#ifndef BYTECODE_H
#define BYTECODE_H

#include "object.hpp"

namespace lsvm {
namespace bytecode {

    using lsvm::object::message_frame;
    using lsvm::object::variable;
    using lsvm::object::block;

    typedef enum { COPY = 0, // [int] from_var, [int] to_var
                   CALL = 1, // [int] ret, [object] reciever, [symbol] msg, [int] var_args_count, [int] var_args...
                   SET_BOOL = 2, // [int] var, [bool] val
                   SET_INT = 3, // [int] var, [int] val
                   SET_DOUBLE = 4, // [int] var, [double] val
    } 
    bytecode_op_type;

    typedef void (*bytecode_op_f)(message_frame* m);

    typedef struct bytecode_op_t {
        struct bytecode_op_t* next;
        bytecode_op_f op;
        variable v[];
    } bytecode_op;

    typedef struct {
        uint32_t v_args;    
        uint32_t v_temp;    
        bytecode_op* op;
    } bytecode;
    
    bytecode_op_f get_bytecode_op(bytecode_op_type op_code);

    block* new_bytecode_block(uint32_t v_copy, bytecode* bytecode);

}
}

#endif
