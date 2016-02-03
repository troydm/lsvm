#include "system.hpp"
#include "memory.hpp"
#include "symbol.hpp"
#include "object.hpp"
#include "bytecode.hpp"

#include <ctime>

namespace lsvm {
namespace system {

using lsvm::object::package;
using lsvm::object::object_class;
using lsvm::object::variable;
using lsvm::object::block;
using lsvm::object::message_frame;
using lsvm::bytecode::bytecode_op;

process* processes;
uint32_t processes_count;
process* current_process;
#define has_processes processes_count > 0

clock_t max_time_slice = CLOCKS_PER_SEC/100;
uint16_t atleast_n_ops = 2000;

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
    // initialize process
    processes_count = 0;
    processes = null;
    current_process = null;

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
    lsvm::memory::deinitialize();
}

inline void schedule_next_process(){
}

process_id new_process(block* b){
    process* p = (process*)lsvm::memory::allocate(sizeof(process));
    p->id = ++processes_count;
    p->prev = &processes;
    p->next = processes;
    p->mf = lsvm::object::new_message_frame(b);
    processes = p;
    return p->id;
}

void free_process(process* p){
    current_process = p->next;
    if(p->next != null)
        p->next->prev = p->prev;
    *(p->prev) = p->next;
    --processes_count;
    lsvm::memory::retain(p);
}

void replace_frame(message_frame* mf){
    mf->next = current_process->mf->next;
    current_process->mf = mf;
}

void push_frame(message_frame* mf){
    mf->next = current_process->mf;
    current_process->mf = mf;
}

uint16_t run_n_ops(uint16_t ops);

void run(){
    //printf("%p %d\n",processes,processes_count);
    printf("%d\n",run_n_ops(atleast_n_ops));
    //printf("%p %d\n",processes,processes_count);
}

uint16_t run_n_ops(uint16_t ops){
    clock_t until = clock()+max_time_slice;
    process* p = current_process;
    do {
        if(p == null){
            if(has_processes){
                current_process = processes;    
            }else{
                break;
            }
            p = current_process;
        }
        do {
            message_frame* mf = p->mf;
            if(mf->bytecode_op != null){
                mf->f(mf);
            }else{
                if(mf->next == null){
                    lsvm::object::free_message_frame(mf);
                    free_process(p);
                    p = current_process;
                    --ops;
                    break;
                }else{
                    message_frame* r = mf->next;
                    // copying return value back to caller message frame
                    r->v[r->ret_var_indx] = mf->v[0];
                    // pop message frame
                    lsvm::object::free_message_frame(mf);
                    p->mf = r;
                }
            }
            --ops;
        }while(ops > 0);
    }while(clock() < until);

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
