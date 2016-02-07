#include "bytecode.hpp"
#include "object.hpp"
#include "system.hpp"

namespace lsvm {
namespace bytecode {

using lsvm::object::block;
using lsvm::object::message_frame;
using lsvm::object::variable;

#define v(a) m->op->v[a]
#define mv(a) m->v[m->op->v[a].i]
#define next_op() m->op = m->op->next;

// FOR DEBUG USE ONLY
void print_var(variable* v){
    printf("v[%p|%lld]\n",v->cls,v->i);
}

// COPY = 0, [int] from_var, [int] to_var
void copy_op(message_frame* m){
    mv(0) = mv(1);
    next_op();
}

// CALL = 1, [int] ret, [object] reciever, [symbol] msg, [int] var_args_count, [int] var_args...
void call_op(message_frame* m){
    block* b = lsvm::object::find_method(mv(1).cls,v(2).sy);
    if(b != null){
        if(b->on_frame){
            b->op->op(m);        
            next_op();
        }else{
            lsvm::object::message_frame* nm = lsvm::object::new_message_frame(b);
            // set self receiver object
            nm->v[1] = v(1);
            // copy argument variables to new message frame
            for(uint32_t i = 0; i < v(3).i; ++i)
                nm->v[2+i] = mv(4+i);
            next_op();

            if(m->op == null){
                // TCO
                // replace current process message frame with new message frame
                lsvm::system::replace_frame(nm);
                free_message_frame(m);
            }else{
                // set return variable
                m->ret_var_indx = v(0).i;
                // push new message frame to current process
                lsvm::system::push_frame(nm);
            }
        }
    }else{
        // TODO: method not found
    }
}

void set_bool_op(message_frame* m){
    //set_bool(&mv(0), v(1).b);
    next_op();
}

void set_int_op(message_frame* m){
    set_int(&mv(0), v(1).i);
    next_op();
}

void set_double_op(message_frame* m){
    //set_double(&mv(0), v(1).d);
    next_op();
}

bytecode_op_f bytecode_op_table[256] = { 
    &copy_op, // COPY
    &call_op, // CALL
    &set_bool_op, // SET_BOOL
    &set_int_op,  // SET_INT
    &set_double_op  // SET_DOUBLE
};

bytecode_op_f get_bytecode_op(bytecode_op_type op_code){
    return bytecode_op_table[op_code];
}

}
}
