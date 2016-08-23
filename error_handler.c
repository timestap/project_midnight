#include "error_handler.h"

#define ERROR_CHAR_LEN 256
#define MSG_LEN 256

/* Create a new number type lval */
lval* lval_num(long x) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_NUM;
  v->num = x;

  add_to_gc(lisp_gc, v);

  return v;
}

/* Create a new error type lval */
lval* lval_err(char* fmt, ...) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_ERR;

  /* Create a va list and initialize it */
  va_list va;
  va_start(va, fmt);

  /* Allocate 256 bytes of space */
  v->err = malloc(ERROR_CHAR_LEN);

  /* printf the error string with a maximum of 255 characters */
  vsnprintf(v->err, ERROR_CHAR_LEN - 1, fmt, va);

  /* Reallocate to number of bytes actually used */
  v->err = realloc(v->err, strlen(v->err)+1);

  /* Cleanup our va list */
  va_end(va);

  add_to_gc(lisp_gc, v);

  return v;
}

lval* lval_symb(char* symbol) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_SYM;
  v->sym = malloc(strlen(symbol) + 1);
  strcpy(v->sym, symbol);

  add_to_gc(lisp_gc, v);

  return v;
}

/* A pointer to a new empty Sexpr lval */
lval* lval_s_expr(void) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_S_EXPR;
  v->count = 0;
  v->cell = NULL;

  add_to_gc(lisp_gc, v);

  return v;
}
//enum { LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR};  
void free_lval(lval* v){
	switch (v->type){
		case LVAL_NUM:
			break;

		case LVAL_ERR:
			free(v->err);
			break;

		case LVAL_SYM:
			free(v->sym);
			break;

    case LVAL_FUN: 
      if (!v->builtin) {
        lenv_del(v->env);
        free_lval(v->formals);
        free_lval(v->body);
      }
      break;

    case LVAL_STR: 
      free(v->str); 
      break;

    case LVAL_Q_EXPR:
		case LVAL_S_EXPR:
			for (int i = 0; i < v->count; i++){
				free_lval(v->cell[i]); //recursive deletino
			}
			free(v->cell);
			break;
	}

  v->meta_data = NULL;
	free(v);
}

char* get_lval_type(int t) {
  switch(t) {
    case LVAL_FUN: return "Function";
    case LVAL_NUM: return "Number";
    case LVAL_ERR: return "Error";
    case LVAL_SYM: return "Symbol";
    case LVAL_S_EXPR: return "S-Expression";
    case LVAL_Q_EXPR: return "Q-Expression";
    case LVAL_STR: return "String";
    default: return "Unknown";
  }
}

lval* lval_lambda(lval* formals, lval* body) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_FUN;

  /* Set Builtin to Null */
  v->builtin = NULL;

  /* Build new environment */
  v->env = lenv_create();

  /* Set Formals and Body */
  v->formals = formals;
  v->body = body;

  add_to_gc(lisp_gc, v);

  return v;  
}

lval* lval_str(char* s) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_STR;
  v->str = malloc(strlen(s) + 1);
  strcpy(v->str, s);

  add_to_gc(lisp_gc, v);

  return v;
}