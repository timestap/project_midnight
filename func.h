//handles the function calls

#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H
#include "data_structs.h"
#endif

#ifndef VARS_H
#define VARS_H
#include "vars.h"
#endif

//note that it checks the !NOT expression
//the ##__VA_ARGS__ ensures that the comma after fmt is deleted 
#define LASSERT(args, cond, fmt, ...) \
  if (!(cond)) { \
    lval* err = lval_err(fmt, ##__VA_ARGS__); \
    free_lval(args); \
    return err; \
  }

#define CHECK_COUNT(args, expected_val, actual_val, function_name) \
  LASSERT(args, expected_val == actual_val, \
  	"Function \"%s\" has incorrect count. Expected %d, got %d", \
  	function_name, expected_val, actual_val);

#define CHECK_TYPE(args, expected_type, actual_type, function_name, expected_string, actual_string) \
  LASSERT(args, expected_type == actual_type, "Function \"%s\" has incorrect type. Expected %s, got %s");

#define CHECK_EMPTY(args, count, function_name) \
  LASSERT(args, count > 0, "Function \"%s\" is empty.", function_name);

lval* builtin_op(lenv* e, lval* a, char* op);
void lenv_add_builtin(lenv* e, char* name, lbuiltin func, int op_code);
void lenv_add_builtins(lenv* e);

lval* builtin_def(lenv* e, lval* a); //add to global environment
lval* builtin_put(lenv* e, lval* a); //add to local environment
lval* lval_call(lenv* e, lval* f, lval* a); 
lval* builtin_var(lenv* e, lval* a, char* func);

lval* builtin_op_add(lenv* e, lval* a);
lval* builtin_op_sub(lenv* e, lval* a);
lval* builtin_op_mul(lenv* e, lval* a);
lval* builtin_op_div(lenv* e, lval* a);
lval* builtin_op_pow(lenv* e, lval* a);
lval* builtin_op_mod(lenv* e, lval* a);

//builting for lambda function-->function for user defined functions
lval* builtin_lambda(lenv* e, lval* a); 
void lenv_def(lenv* e, lval* k, lval* v); //define symbol in global variable 


lval* exit_lisp(lenv* e, lval* a);

char* get_function_name(int op_code);

//prints the number of variables, as well as all variables that the user has defined
lval* print_all_vars(lenv* e, lval* a); 

//Equality operators
lval* builtin_gt(lenv* e, lval* a);
lval* builtin_lt(lenv* e, lval* a);
lval* builtin_ge(lenv* e, lval* a);
lval* builtin_le(lenv* e, lval* a);
int lval_eq(lval* x, lval* y);
lval* builtin_cmp(lenv* e, lval* a, char* op);
lval* builtin_ne(lenv* e, lval* a);
lval* builtin_eq(lenv* e, lval* a);
lval* builtin_if(lenv* e, lval* a);
lval* builtin_ord(lenv* e, lval* a, char* op);

lval* builtin_and(lenv*e, lval* a);
lval* builtin_or(lenv*e, lval* a);
lval* builtin_not(lenv*e, lval* a);
lval* builtin_load(lenv* e, lval* a);
lval* builtin_print(lenv* e, lval* a);
lval* builtin_error(lenv* e, lval* a);
lval* builtin_read(lenv* e, lval* a); //turn a string into a single q-expression

lval* print_num_gc_vars(lenv* e, lval* a); //for debugging