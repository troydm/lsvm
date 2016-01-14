#include "system.hpp"
#include "memory.hpp"
#include "symbol.hpp"
#include "object.hpp"
#include "bytecode.hpp"

namespace lsvm {
namespace system {

using lsvm::object::package;
using lsvm::object::object_class;
using lsvm::object::variable;
using lsvm::object::block;
using lsvm::object::message_frame;
using lsvm::bytecode::bytecode_op;

uint32_t processes_count;
process_id process_curr_id;
process_id process_free_id;
message_frame** processes;
#define total_processes process_free_id
#define current_process processes[process_curr_id] 
#define has_processes process_free_id > 0

object_class* Class, *Object, *Nil, *Integer;

#define arg(i) (m->v[i])
#define op(i) (op->v[i])
#define def_system_block(n,f,a) block n = { &f, true, null, a, 0, 0}

void hash_block_f(message_frame* m){
    bytecode_op* op = m->bytecode_op;
    variable* v0 = &op(0);
    variable* v1 = &op(1);
    if(arg(v1->i).cls == Integer)
        set_int(&arg(v0->i),arg(v1->i).i);
} 
def_system_block(hash_block,hash_block_f,1);

void init(){
    lsvm::memory::initialize();
    // initialize processes
    processes_count = 1024;
    process_curr_id = 0;
    process_free_id = 0;
    processes = (message_frame**)lsvm::memory::allocate(processes_count*sizeof(message_frame*));

    // initialize global package
    package* system = lsvm::object::new_package(sym("system"));

    // initialize Class
    Class = lsvm::object::new_class(system,sym("Class"),null,0);
    lsvm::object::define_class(system,Class);

    Object = lsvm::object::new_class(system,sym("Object"),null,0);
    lsvm::object::define_class(system,Object);

    Nil = lsvm::object::new_class(system,sym("Nil"),Object,0);
    lsvm::object::define_class(system,Nil);

    Integer = lsvm::object::new_class(system,sym("Integer"),Object,0);
    lsvm::object::define_class(system,Integer);

    // defining methods for Object
    lsvm::object::define_method(Object,sym("hash"),&hash_block);
}

void stop(){
    lsvm::object::free_packages();
    lsvm::symbol::clear();
    lsvm::memory::retain(processes);
    lsvm::memory::deinitialize();
}

process_id new_process(block* b){
    if(process_free_id == processes_count){
        processes_count *= 2;
        processes = (message_frame**)lsvm::memory::reallocate(processes,processes_count*sizeof(message_frame*));
    }
    
    processes[process_free_id] = lsvm::object::new_message_frame(b);
    return process_free_id++;
}

void free_process(process_id process_id){
    lsvm::object::free_message_frame(processes[process_id]);
    if(process_free_id-1 != process_id){
        processes[process_id] = processes[process_free_id-1];
    }else{
        processes[process_id] = null;
    }
    --process_free_id;
}

void replace_current_process(process* p){
    p->next = processes[process_curr_id];
    processes[process_curr_id] = p;
}

void return_current_process(){
    process* p = processes[process_curr_id];
    if(p->next == null){
        free_process(process_curr_id);
    }else{
        process* pr = p->next;
        // copying return value back to caller message frame
        pr->v[pr->ret_var_indx] = p->v[0];
        processes[process_curr_id] = pr;
        lsvm::object::free_message_frame(p);
    }
}

uint32_t run_n_ops(uint32_t ops);

void run(){
    run_n_ops(10);
}

uint32_t run_n_ops(uint32_t ops){
    while(ops > 0 && has_processes){
        process* p = current_process;
        if(p->bytecode_op != null){
            p->f(p);
            --ops;
        }else{
            return_current_process();
        }
    }

    return ops;
}


}

namespace object {

message_frame* new_message_frame(block* b){
    message_frame* m = (message_frame*)lsvm::memory::allocate(
            sizeof(message_frame)+
            ((1+b->v_args+b->v_copy+b->v_temp)*sizeof(variable))
            );
    m->next = null;
    m->f = b->f; 
    m->b = b;
    m->bytecode_op = b->bytecode_op;
    set_nil(&arg(0));
    // copy block enviroment variables
    int32_t i = b->v_copy-1;
    while(i >= 0){
        arg(1+b->v_args+i) = b->v[i]; 
    }    
    return m;
}

void free_message_frame(message_frame* f){
    lsvm::memory::retain(f);
}

void set_nil(variable* v){
    v->cls = lsvm::system::Nil;
}

void set_int(variable* v, int64_t val){
    v->cls = lsvm::system::Integer;
    v->i = val;
}

}

namespace bytecode {

block* new_bytecode_block(uint32_t v_copy, bytecode* bytecode){
    block* b = (block*)lsvm::memory::allocate(
            sizeof(block)+(v_copy*sizeof(variable))
            );
    b->v_args = bytecode->v_args;
    b->v_copy = v_copy;
    b->v_temp = bytecode->v_temp;
    b->f = &lsvm::bytecode::bytecode_f;
    b->bytecode_op = bytecode->op;
    return b;
}

}

}
