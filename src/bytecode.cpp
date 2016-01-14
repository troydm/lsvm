#include "bytecode.hpp"
#include "object.hpp"
#include "system.hpp"

namespace lsvm {
namespace bytecode {

using lsvm::object::block;
using lsvm::object::message_frame;
using lsvm::object::variable;

typedef void (*bytecode_op_f)(message_frame* m, variable v[]);

#define v(a) v[a]
#define mv(a) m->v[v[a].i]
#define next_op() m->bytecode_op = m->bytecode_op->next;

// FOR DEBUG USE ONLY
void print_var(variable* v){
    printf("v[%p|%lld]\n",v->cls,v->i);
}

// COPY = 0, [int] from_var, [int] to_var
void copy_op(message_frame* m, variable v[]){
    mv(0) = mv(1);
    next_op();
}

// CALL = 1, [int] ret, [object] reciever, [symbol] msg, [int] var_args_count, [int] var_args...
void call_op(message_frame* m, variable v[]){
    block* b = lsvm::object::find_method(mv(1).cls,v(2).sy);
    if(b != null){
        if(b->on_frame){
            b->f(m);        
        }else{
            // set return variable
            m->ret_var_indx = v(0).i;
            lsvm::object::message_frame* nm = lsvm::object::new_message_frame(b);
            // set self receiver object
            nm->v[1] = v(1);
            // copy argument variables to new message frame
            for(uint32_t i = 0; i < v(3).i; ++i)
                nm->v[2+i] = mv(4+i);
            // replace current process with new message frame
            lsvm::system::replace_current_process(nm);
        }
    }else{
        // TODO: method not found
    }
    next_op();
}

void set_bool_op(message_frame* m, variable v[]){
    //set_bool(&mv(0), v(1).b);
    next_op();
}

void set_int_op(message_frame* m, variable v[]){
    set_int(&mv(0), v(1).i);
    next_op();
}

void set_double_op(message_frame* m, variable v[]){
    //set_double(&mv(0), v(1).d);
    next_op();
}

bytecode_op_f bytecode_jump_table[256] = { &copy_op, &call_op, &set_bool_op, &set_int_op, &set_double_op };

void bytecode_f(message_frame* m){
    bytecode_op* op = m->bytecode_op;
    (bytecode_jump_table[op->op_code])(m,op->v);
}

}
}
