//garbage collector functions

#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H
#include "data_structs.h"
#endif 

#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H
#include "error_handler.h"
#endif

#include <assert.h>

void sweep(gc* my_gc);
gc* create_gc();

void add_to_gc(gc* my_gc, lval* new_lval);
void assign_lval(lval** new_pointer, lval* new_lval);
void decrement_counter(lval* cur_lval);

lval_metadata* create_lval_metadata();