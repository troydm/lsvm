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

#define max_time_slice (CLOCKS_PER_SEC/100)
#define max_time_slice_half (max_time_slice/2)
#define atleast_n_ops 10

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
    p->status = RUNNING;
    processes = p;
    return p->id;
}

void free_process(process* p){
    current_process = p->next;
    if(current_process != null)
        current_process->prev = p->prev;
    *(p->prev) = current_process;
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

// get current time in millis seconds
uint32_t current_time(){
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    uint64_t millis = t.tv_sec*1000LL;
    millis += t.tv_nsec/1000000;
    return millis;
}

void sleep(uint32_t millis){
#if _POSIX_C_SOURCE >= 199309L
    struct timespec t;
    t.tv_sec = millis / 1000;
    t.tv_nsec = (millis % 1000) * 1000000;
    nanosleep(&t, NULL);
#else
    usleep(milliseconds * 1000);
#endif
}

clock_t run_time_slice();

void run(){
    //printf("%p %d\n",processes,processes_count);
    clock_t clocks = run_time_slice();
    if(clocks < max_time_slice_half){
        sleep(10);
        printf("sleep\n");
    }
    printf("%d\n",(int)clocks);
    //printf("%p %d\n",processes,processes_count);
}

clock_t run_time_slice(){
    clock_t start = clock();
    clock_t until = start+max_time_slice;
    uint32_t processes_not_running = 0;
    do {
CHECK_PROCESS:
        // check current process
        if(current_process == null){
            if(has_processes){
                current_process = processes;    
            }else{
                break;
            }
        }else if(current_process->status != RUNNING){
            current_process = current_process->next;
            if(++processes_not_running > 2*processes_count){
                break;            
            }
            goto CHECK_PROCESS;
        }
        // run atleast n ops in current process
        current_process->ops = atleast_n_ops;
        do {
            message_frame* mf = current_process->mf;
            if(mf->bytecode_op != null){
                mf->f(mf);
            }else{
                if(mf->next == null){
                    lsvm::object::free_message_frame(mf);
                    free_process(current_process);
                    goto CHECK_PROCESS;
                }else{
                    message_frame* r = mf->next;
                    // copying return value back to caller message frame
                    r->v[r->ret_var_indx] = mf->v[0];
                    // pop message frame
                    lsvm::object::free_message_frame(mf);
                    current_process->mf = r;
                }
            }
            current_process->ops -= 1;
        }while(current_process->ops > 0);
        // schedule next process
        current_process = current_process->next;
    }while(clock() < until);

    return clock() - start;
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
