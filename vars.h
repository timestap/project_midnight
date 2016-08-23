//variable suppport
#include "parser.h"
#include "error_handler.h"

#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H
#include "data_structs.h"
#endif


//create function-type lvals 
lval* lval_fun(lbuiltin func, short function_opcode); //returns lval of type function
lval* lval_copy(lval* v);

//environemnt constructor & destructor 
lenv* lenv_create(void);
void lenv_del(lenv* e);
lenv* lenv_copy(lenv* e);

//getters and setter for lvals 
//slow linked-list implementation 

lval* lenv_get(lenv* e, lval* k);
void lenv_put(lenv* e, lval* k, lval* v);


