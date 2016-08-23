#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"
#include "error_handler.h"
#include "func.h"

#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H
#include "data_structs.h"
#endif

mpc_parser_t* Number;
mpc_parser_t* Symbol;
mpc_parser_t* S_expr;
mpc_parser_t* Expr;
mpc_parser_t* Lispy;
mpc_parser_t* Q_expr;
mpc_parser_t* Lisp_String;
mpc_parser_t* Comment;

void setup_polish_grammer();
void polish_grammer_cleanup();
void handle_input(lenv* e, char* input);
void process_command(lenv* e, mpc_result_t* my_result);
int number_of_nodes(mpc_ast_t* t);
lval* lval_read_num(mpc_ast_t* t); //when the value at s-expression is not a symbol/s expresion
lval* lval_read(mpc_ast_t* t);
lval* lval_add(lval* v, lval* x);
void lval_expr_print(lval* v, char open, char close);
void lval_print(lval* v);
lval* lval_eval_s_expr(lenv* e, lval* v);
lval* lval_eval(lenv* e, lval* v);
lval* lval_pop(lval* v, int i);  //pop the first element from lval
lval* lval_take(lval* v, int i); //take value at i and remove the entire expression 
lval* lval_q_expr(void);
lval* builtin_head(lenv* e, lval* a); //Takes a Q-Expression and returns a Q-Expression with only of the first element
lval* builtin_tail(lenv* e, lval* a); //Takes a Q-Expression and returns a Q-Expression with the first element removed
lval* builtin_list(lenv* e, lval* a); //converts S expression to q expression
lval* eval(lenv* e, lval* a); //converts q expression and returns as s expression
lval* builtin_join(lenv* e, lval* a);
lval* builtin(lenv* e, lval* a, char* func);
lval* lval_join(lval* x, lval* y);
lval* cons(lenv* e, lval* x); //takes a value and adds it to the front of an existing Q-Expression
void print_lval_debug(lval* x); //printing lval for debugging purposes
lval* lval_len(lenv* e, lval* a); //returns number of elements in Q-expression
lval* init(lenv* e, lval* a);
void lval_print_str(lval* v);
lval* lval_read_str(mpc_ast_t* t);
void lval_println(lval* v);

void load_std_lib();