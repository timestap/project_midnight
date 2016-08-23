#include "vars.h"

lval* lval_fun(lbuiltin func, short op) {
	lval* v = malloc(sizeof(lval));
	v->type = LVAL_FUN;
	v->builtin = func;
	v->function_opcode = op;

  add_to_gc(lisp_gc, v);

	return v;
}

lval* lval_copy(lval* v) {
  
  lval* x = malloc(sizeof(lval));
  x->type = v->type;

  //add the new lval to 
  add_to_gc(lisp_gc, x);
  
  switch (v->type) {
    
    /* Copy Functions and Numbers Directly */
    case LVAL_FUN: 
    	if (v->builtin){
	    	x->builtin = v->builtin; 
	    	x->function_opcode = v->function_opcode;
    	}	
    	else{
    		x->builtin = NULL;
		    x->env = lenv_copy(v->env);
		    x->formals = lval_copy(v->formals);
		    x->body = lval_copy(v->body);
    	}

    	break;
    case LVAL_NUM: x->num = v->num; break;
    
    /* Copy Strings using malloc and strcpy */
    case LVAL_ERR:
      x->err = malloc(strlen(v->err) + 1);
      strcpy(x->err, v->err); break;
    
    case LVAL_SYM:
      x->sym = malloc(strlen(v->sym) + 1);
      strcpy(x->sym, v->sym); break;

    case LVAL_STR: 
    	x->str = malloc(strlen(v->str) + 1);
  		strcpy(x->str, v->str); 
  		break;

    /* Copy Lists by copying each sub-expression */
    case LVAL_S_EXPR:
    case LVAL_Q_EXPR:
      x->count = v->count;
      x->cell = malloc(sizeof(lval*) * x->count);
      for (int i = 0; i < x->count; i++) {
        x->cell[i] = lval_copy(v->cell[i]);
      }
    break;
  }


  return x;
}

lenv* lenv_create(void) {
  lenv* e = malloc(sizeof(lenv));
  e->parent = NULL;
  e->count = 0;
  e->syms = NULL;
  e->vals = NULL;
  return e;
}

void lenv_del(lenv* e) {
  for (int i = 0; i < e->count; i++) {
    free(e->syms[i]);
    //free_lval(e->vals[i]);
    decrement_counter(e->vals[i]);
  }
  free(e->syms);
  free(e->vals);
  free(e);
}

lval* lenv_get(lenv* e, lval* k) {

  /* Iterate over all items in environment */
  for (int i = 0; i < e->count; i++) {
    /* Check if the stored string matches the symbol string */
    /* If it does, return a copy of the value */
    if (strcmp(e->syms[i], k->sym) == 0) {
      return lval_copy(e->vals[i]);
    }
  }

  //look at the symbols in the parent environment
  if (e->parent) {
    return lenv_get(e->parent, k);
  } 

  /* If no symbol found return error */
  return lval_err("unbound symbol during lenv_get, with symbol: %s", k->sym);
}


void lenv_put(lenv* e, lval* k, lval* v) {

  /* Iterate over all items in environment */
  /* This is to see if variable already exists */
  for (int i = 0; i < e->count; i++) {

    /* If variable is found delete item at that position */
    /* And replace with variable supplied by user */
    if (strcmp(e->syms[i], k->sym) == 0) {
      //free_lval(e->vals[i]);
      decrement_counter(e->vals[i]);
      assign_lval(&e->vals[i], v);
      //e->vals[i] = lval_copy(v);
      return;
    }
  }

  /* If no existing entry found allocate space for new entry */
  e->count++;
  e->vals = realloc(e->vals, sizeof(lval*) * e->count);
  e->syms = realloc(e->syms, sizeof(char*) * e->count);

  /* Copy contents of lval and symbol string into new location */
  //e->vals[e->count-1] = lval_copy(v);
  assign_lval(&e->vals[e->count-1], v);
  e->syms[e->count-1] = malloc(strlen(k->sym)+1);
  strcpy(e->syms[e->count-1], k->sym);
}

lenv* lenv_copy(lenv* e) {
	lenv* n = malloc(sizeof(lenv));
	n->parent = e->parent;
	n->count = e->count;
	n->syms = malloc(sizeof(char*) * n->count);
	n->vals = malloc(sizeof(lval*) * n->count);
	
	for (int i = 0; i < e->count; i++) {
		n->syms[i] = malloc(strlen(e->syms[i]) + 1);
		strcpy(n->syms[i], e->syms[i]);
		n->vals[i] = lval_copy(e->vals[i]);
	}
	return n;
}
