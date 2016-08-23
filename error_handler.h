#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H
#include "data_structs.h"
#endif

#ifndef VARS_H
#define VARS_H
#include "vars.h"
#endif


#ifndef GC_H
#define GC_H
#include "gc.h"
#endif

lval* lval_num(long x);
lval* lval_err(char* fmt, ...);
lval* lval_symb(char* symbol);
lval* lval_lambda(lval* formals, lval* body); //create user defined function
lval* builtin_lambda(lenv* e, lval* a);
lval* lval_s_expr(void);
void free_lval(lval* v);
char* get_lval_type(int t);
lval* lval_str(char* s);