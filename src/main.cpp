#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "ast.hpp"
#include "symbol.hpp"
#include "memory.hpp"
#include "hashmap.hpp"
#include "string.hpp"

using namespace std;
using namespace lsvm::ast;

int main(int argc, char* argv[]){

    lsvm::memory::initialize();

    void* p = lsvm::memory::allocate(124);
    p = lsvm::memory::reallocate(p, 128);
    lsvm::memory::retain(p);


    //lsvm::hashmap::hashmap* h = lsvm::hashmap::new_hashmap(lsvm::symbol::equals,lsvm::hashmap::hash);

    //lsvm::hashmap::free_hashmap(h);

    lsvm::string::string* s = lsvm::string::new_string();

    printf("%d\n",strlen(lsvm::string::c_str(s)));

    lsvm::string::free(s);

    s = lsvm::string::new_string("Hello World!");

    printf("%d\n",lsvm::string::size(s));
    printf("%d\n",strlen(lsvm::string::c_str(s)));

    int size = strtol(argv[1],NULL,10);
    int range = strtol(argv[2],NULL,10);

    lsvm::hashmap::hashmap* h = lsvm::hashmap::new_hashmap(&lsvm::hashmap::string_equals,&lsvm::hashmap::string_hash,1,3,3,range);
   
    //lsvm::hashmap::put(h,s,s);
    printf("%d\n",lsvm::hashmap::count(h));
    //printf("%d\n",lsvm::string::size((lsvm::string::string*)lsvm::hashmap::get(h,s)));

    printf("hashmap %d\n",size);
    char buf[33];
    uint32_t sum = 0;
    int i = 0;
    while(i < size){
        snprintf(buf,33,"%d",i);
        lsvm::hashmap::put(h,lsvm::string::new_string(buf),lsvm::string::new_string(buf));
        ++i;
        sum += i;
    }

    /*
    uint32_t sum2 = 0;
    int i2 = 0;
    lsvm::hashmap::hashmap_iterator* it = lsvm::hashmap::new_iterator(h);
    while(lsvm::hashmap::next(it) != null){
        ++i2;   
        sum2 += i2;
        //printf("next %s\n",lsvm::string::c_str((lsvm::string::string*)lsvm::hashmap::current(it)->key));
    }
    lsvm::hashmap::free_iterator(it);
    if(sum != sum2)
    printf("Is not Equal %d %d %d %d\n",sum, i, sum2, i2);
    else
    printf("Equal\n");
    */

    lsvm::hashmap::print(h);


    //lsvm::hashmap::remove(h, lsvm::string::new_string("1314"));

    i = 0;
    while(i < size){
        snprintf(buf,33,"%d",i);
        lsvm::string::string* gs = (lsvm::string::string*)lsvm::hashmap::get(h,lsvm::string::new_string(buf));
        if(gs == null || !lsvm::string::equals(gs,lsvm::string::new_string(buf))){
            printf("key is missing %s\n",buf);
            return -1;
        }
        ++i;
    }
    
    lsvm::hashmap::free(h);
    lsvm::string::free(s);

    //ASTMessage m(ASTNumber(4),ASTLiteral("+"),ASTNumber(4));
    Integer a(12);
    printf("%p\n",lsvm::symbol::new_symbol("hello"));
    printf("%p\n",lsvm::symbol::new_symbol("hello"));

    cout << "Hello World!" << endl;

    lsvm::memory::deinitialize();

    return 0;
}
