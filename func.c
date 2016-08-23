#include "func.h"

lval* builtin_add(lenv* e, lval* a){
	return builtin(e, a, "add");
}

lval* builtin_sub(lenv* e, lval* a){
	return builtin(e, a, "sub");
}

lval* builtin_mul(lenv* e, lval* a){
	return builtin(e, a, "mul");
}

lval* builtin_div(lenv* e, lval* a){
	return builtin(e, a, "div");
}

lval* builtin_pow(lenv* e, lval* a){
	return builtin(e, a, "pow");
}

lval* builtin_mod(lenv* e, lval* a){
	return builtin(e, a, "mod");
}

lval* builtin_exit(lenv* e, lval* a){
	return builtin(e, a, "exit");
}

lval* builtin_op(lenv* e, lval* a, char* op) {
  
  /* Ensure all arguments are numbers */
  for (int i = 0; i < a->count; i++) {
    if (a->cell[i]->type != LVAL_NUM) {
      //free_lval(a);
      decrement_counter(a);
      return lval_err("Cannot operate on non-number!");
    }
  }
  
  /* Pop the first element */
  lval* x = lval_pop(a, 0);

  /* If no arguments and sub then perform unary negation */
  if ((strcmp(op, "-") == 0 || strcmp(op, "sub") == 0) && a->count == 0) {
    x->num = -x->num;
  }

  /* While there are still elements remaining */
	while (a->count > 0) {

    /* Pop the next element */
    lval* y = lval_pop(a, 0);

	  if (strcmp(op, "+") == 0 || strcmp(op, "add") == 0) { x->num += y->num; }
		if (strcmp(op, "-") == 0 || strcmp(op, "sub") == 0) { x->num -= y->num; }
		if (strcmp(op, "*") == 0 || strcmp(op, "mul") == 0) { x->num *= y->num; }
		if (strcmp(op, "/") == 0 || strcmp(op, "div") == 0) { 
			if (y->num == 0){
				//free_lval(x);
				//free_lval(y);
				decrement_counter(x);
				decrement_counter(y);
				x = lval_err("Division By Zero!");
			}else{
				x->num /= y->num;
			}
		}
		if (strcmp(op, "%") == 0 || strcmp(op, "mod") == 0) { 
			if (y->num == 0){
				//free_lval(x);
				//free_lval(y);
				decrement_counter(x);
				decrement_counter(y);
				x = lval_err("Can't Mod By Zero!");
			}else{
				x->num = x->num % y->num;
			}
		}
		if (strcmp(op, "^") == 0 || strcmp(op, "exp") == 0) { 
			x->num = pow(x->num, y->num); 
	    	//free_lval(y);
	    	decrement_counter(y);
		}
	
	}
	//free_lval(a); 
	decrement_counter(a);
	return x;	
}

void lenv_add_builtin(lenv* e, char* name, lbuiltin func, int op_code) {
  lval* k = lval_symb(name);
  lval* v = lval_fun(func, op_code);
  lenv_put(e, k, v);
  //free_lval(k); free_lval(v);
  decrement_counter(k);
  decrement_counter(v);
}


//builtin_add(cur_lenv, cur_lval, op_code)
void lenv_add_builtins(lenv* e) {  
	/* List Functions */
	lenv_add_builtin(e, "list", builtin_list, OP_LIST); //builtin_add(op_code)
	lenv_add_builtin(e, "head", builtin_head, OP_HEAD);
	lenv_add_builtin(e, "tail", builtin_tail, OP_TAIL);
	lenv_add_builtin(e, "eval", eval, OP_EVAL);
	lenv_add_builtin(e, "join", builtin_join, OP_JOIN);
	lenv_add_builtin(e, "cons", cons, OP_CONS);
	lenv_add_builtin(e, "len",  lval_len, OP_LEN);
	lenv_add_builtin(e, "init", init, OP_INIT);
	lenv_add_builtin(e, "exit", builtin_exit, OP_EXIT);

	/* Mathematical Functions */
	lenv_add_builtin(e, "+", builtin_add, OP_ADD);
	lenv_add_builtin(e, "-", builtin_sub, OP_SUB);
	lenv_add_builtin(e, "*", builtin_mul, OP_MUL);
	lenv_add_builtin(e, "/", builtin_div, OP_DIV);
	lenv_add_builtin(e, "^", builtin_pow, OP_POW);
	lenv_add_builtin(e, "%", builtin_mod, OP_MOD);

	//using english words instead of symbols
	lenv_add_builtin(e, "add", builtin_add, OP_ADD);
	lenv_add_builtin(e, "sub", builtin_sub, OP_SUB);
	lenv_add_builtin(e, "mul", builtin_mul, OP_MUL);
	lenv_add_builtin(e, "div", builtin_div, OP_DIV);
	lenv_add_builtin(e, "pow", builtin_pow, OP_POW);
	lenv_add_builtin(e, "mod", builtin_mod, OP_MOD);

	lenv_add_builtin(e, "printall", print_all_vars, OP_PRINTALL);

	//add builtin for lambda-related stuff
	lenv_add_builtin(e, "def",  builtin_def, OP_DEF);
	lenv_add_builtin(e, "=",   builtin_put, OP_PUT);
	lenv_add_builtin(e, "\\", builtin_lambda, OP_LAMBDA);

	/* Comparison Functions */
	lenv_add_builtin(e, "if", builtin_if, OP_IF);
	lenv_add_builtin(e, "==", builtin_eq, OP_EQUAL);
	lenv_add_builtin(e, "!=", builtin_ne, OP_NOT_EQUAL);
	lenv_add_builtin(e, ">",  builtin_gt, OP_GREATER_THAN);
	lenv_add_builtin(e, "<",  builtin_lt, OP_LESS_THAN);
	lenv_add_builtin(e, ">=", builtin_ge, OP_GREATER_EQUAL);
	lenv_add_builtin(e, "<=", builtin_le, OP_LESS_EQUAL);
	lenv_add_builtin(e, "&&", builtin_and, OP_AND);
	lenv_add_builtin(e, "||", builtin_or, OP_OR);
	lenv_add_builtin(e, "!", builtin_not, OP_NOT);
	lenv_add_builtin(e, "and", builtin_and, OP_AND);
	lenv_add_builtin(e, "or", builtin_or, OP_OR);
	lenv_add_builtin(e, "not", builtin_not, OP_NOT);

	lenv_add_builtin(e, "load",  builtin_load, OP_LOAD); 
	lenv_add_builtin(e, "error", builtin_error, OP_ERR);
	lenv_add_builtin(e, "print", builtin_print, OP_PRINT);
	lenv_add_builtin(e, "read", builtin_read, OP_READ);

	lenv_add_builtin(e, "print_gc", print_num_gc_vars, OP_GC); 

}

lval* builtin_def(lenv* e, lval* a) {
  return builtin_var(e, a, "def");
}

lval* builtin_put(lenv* e, lval* a) {
  return builtin_var(e, a, "=");
}

//takes a list of variables and assigns values to those variables 
//e.g. def {x y z } 5 6 7
lval* builtin_var(lenv* e, lval* a, char* func) {
	CHECK_TYPE(a, LVAL_Q_EXPR, a->cell[0]->type, func, get_lval_type(LVAL_Q_EXPR), get_lval_type(a->cell[0]->type));

	lval* syms = a->cell[0];
	for (int i = 0; i < syms->count; i++) {
		LASSERT(a, (syms->cell[i]->type == LVAL_SYM),
		  "Function '%s' cannot define non-symbol. "
		  "Got %s, Expected %s.", func, 
		  get_lval_type(syms->cell[i]->type),
		  get_lval_type(LVAL_SYM));
	}

	LASSERT(a, (syms->count == a->count-1),
		"Function '%s' passed too many arguments for symbols. "
		"Got %i, Expected %i.", func, syms->count, a->count-1);

	for (int i = 0; i < syms->count; i++) {
		/* If 'def' define in globally. If 'put' define in locally */
		if (strcmp(func, "def") == 0) {
		  lenv_def(e, syms->cell[i], a->cell[i+1]);
		}

		if (strcmp(func, "=")   == 0) {
		  lenv_put(e, syms->cell[i], a->cell[i+1]);
		} 
	}

	//free_lval(a);
	decrement_counter(a);
	return lval_s_expr();
}

lval* exit_lisp(lenv* e, lval* a){
	to_execute = false;
	//free_lval(a);
	decrement_counter(a);
	//printf("Exiting...%s\n", get_lval_type(a->type));
	lval* exit_val = lval_symb("Lisp exiting...\n");
	return exit_val;
}

char* get_function_name(int op_code){
	switch(op_code){
		case OP_LIST: return "list"; 
		case OP_HEAD: return "head"; 
		case OP_TAIL: return "tail"; 
		case OP_EVAL: return "eval"; 
		case OP_JOIN: return "join"; 
		case OP_CONS: return "cons";
		case OP_LEN: return "len"; 
		case OP_INIT: return "init"; 
		case OP_EXIT: return "exit"; 
		case OP_ADD: return "add"; 
		case OP_SUB: return "sub"; 
		case OP_MUL: return "mul"; 
		case OP_DIV: return "div"; 
		case OP_MOD: return "mod"; 
		case OP_POW: return "pow"; 
		case OP_DEF: return "def"; 
		case OP_PUT: return "put"; 
		case OP_LAMBDA: return "lambda"; 
		case OP_PRINTALL: return "printall";
		case OP_IF: return "if";
		case OP_EQUAL: return "==";
		case OP_NOT_EQUAL: return "!=";
		case OP_GREATER_THAN: return ">";
		case OP_LESS_THAN: return "<";
		case OP_GREATER_EQUAL: return ">=";
		case OP_LESS_EQUAL: return "<=";
		case OP_AND: return "&&";
		case OP_OR: return "||";
		case OP_NOT: return "!";
		case OP_LOAD: return "load";
		case OP_ERR: return "error";
		case OP_PRINT: return "print";
		case OP_READ: return "read";

		default: return "Unknown";
	}
}

lval* print_all_vars(lenv* e, lval* a){
	printf("The number of variables is: %i\n", e->count);
	for (int i = 0; i < e->count; i++){
		lval_print(e->vals[i]);
		printf("\n");
	}

	//free_lval(a);
	decrement_counter(a);
	return lval_str("Printing Completed.");
}

lval* print_num_gc_vars(lenv* e, lval* a){
	printf("The number of variables is: %i\n", lisp_gc->cur_num_vars);

	decrement_counter(a);
	return lval_s_expr();
}

lval* builtin_lambda(lenv* e, lval* a) {
	/* Check Two arguments, each of which are Q-Expressions */

	CHECK_COUNT(a, 2, a->count, "lambda");
	CHECK_TYPE(a, LVAL_Q_EXPR, a->cell[0]->type, "lambda", get_lval_type(LVAL_Q_EXPR), get_lval_type(a->cell[0]->type));
	CHECK_TYPE(a, LVAL_Q_EXPR, a->cell[1]->type, "lambda", get_lval_type(LVAL_Q_EXPR), get_lval_type(a->cell[1]->type));

	/* Check first Q-Expression contains only Symbols */
	for (int i = 0; i < a->cell[0]->count; i++) {
	LASSERT(a, (a->cell[0]->cell[i]->type == LVAL_SYM),
	  "Cannot define non-symbol. Got %s, Expected %s.",
	  get_lval_type(a->cell[0]->cell[i]->type),get_lval_type(LVAL_SYM));
	}
  
	/* Pop first two arguments and pass them to lval_lambda */
	lval* formals = lval_pop(a, 0);
	lval* body = lval_pop(a, 0);
	//free_lval(a);
	decrement_counter(a);

	return lval_lambda(formals, body);
}

void lenv_def(lenv* e, lval* k, lval* v) {
  /* Iterate till e has no parent */
  while (e->parent) { e = e->parent; }
  /* Put value in e */
  lenv_put(e, k, v);
}

//Way to define functions below
// def {add-mul} (\ {x y} {+ x (* x y)})

//Easier way to define functions 
// 1. def {fun} (\ {args body} {def (head args) (\ (tail args) body)})
// 2. fun {add-together x y} {+ x y}

//Currying ->
//fun {unpack f xs} {eval (join (list f) xs)}
//fun {pack f & xs} {f xs}
//def {uncurry} pack
//def {curry} unpack

//Use cases:
//curry + {5 6 7}
//uncurry head 5 6 7
//def {add-uncurried} +
//def {add-curried} (curry +)
//add-curried {5 6 7}
//add-uncurried 5 6 7
//fun {len l} {if (== l {}) {0} {+ 1 (len (tail l))}}

//if usage: if (== x y) {+ x y} {- x y}

//returns nth element of a list, starts from 0
//fun {nth_elem n my_list} {if (== n 0) {head my_list} {nth_elem (- n 1) (tail(my_list)) }}

//passing contents of "a" into "f"
lval* lval_call(lenv* e, lval* f, lval* a) {

	/* If Builtin then simply apply that */
	if (f->builtin) { return f->builtin(e, a); }

	/* Record Argument Counts */
	int given = a->count;
	int total = f->formals->count;

	/* While arguments still remain to be processed */
	while (a->count) {

		/* If we've ran out of formal arguments to bind */
		if (f->formals->count == 0) {
		  //free_lval(a);
		  decrement_counter(a); 
		  return lval_err(
		    "Function passed too many arguments. "
		    "Got %i, Expected %i.", given, total); 
		}

		/* Pop the first symbol from the formals */
		lval* sym = lval_pop(f->formals, 0);

		if (strcmp(sym->sym, "&") == 0) {

			/* Ensure '&' is followed by another symbol */
			if (f->formals->count != 1) {
				//free_lval(a);
				decrement_counter(a);
				return lval_err("Function format invalid. "
				"Symbol '&' not followed by single symbol.");
			}

			/* Next formal should be bound to remaining arguments */
			lval* nsym = lval_pop(f->formals, 0);
			lenv_put(f->env, nsym, builtin_list(e, a));
			//free_lval(sym); 
			//free_lval(nsym);
			decrement_counter(sym);
			decrement_counter(nsym);
			break;
		}


		/* Pop the next argument from the list */
		lval* val = lval_pop(a, 0);

		/* Bind a copy into the function's environment */
		lenv_put(f->env, sym, val);

		/* Delete symbol and value */
		//free_lval(sym); 
		//free_lval(val);
		decrement_counter(sym);
		decrement_counter(val);
	}


	/* Argument list is now bound so can be cleaned up */
	//free_lval(a);
	decrement_counter(a);

	/* If '&' remains in formal list bind to empty list */
	if (f->formals->count > 0 &&
	  strcmp(f->formals->cell[0]->sym, "&") == 0) {

	  /* Check to ensure that & is not passed invalidly. */
	  if (f->formals->count != 2) {
	    return lval_err("Function format invalid. "
	      "Symbol '&' not followed by single symbol.");
	  }

	  /* Pop and delete '&' symbol */
	  //free_lval(lval_pop(f->formals, 0));
	  decrement_counter(lval_pop(f->formals, 0));

	  /* Pop next symbol and create empty list */
	  lval* sym = lval_pop(f->formals, 0);
	  lval* val = lval_q_expr();

	  /* Bind to environment and delete */
	  lenv_put(f->env, sym, val);
	  //free_lval(sym); 
	  //free_lval(val);
	  decrement_counter(sym);
	  decrement_counter(val);
	}


	/* If all formals have been bound evaluate */
	if (f->formals->count == 0) {

		/* Set environment parent to evaluation environment */
		f->env->parent = e;

		/* Evaluate and return */
		return eval(
		  f->env, lval_add(lval_s_expr(), lval_copy(f->body)));
	} else {
		/* Otherwise return partially evaluated function */
		return lval_copy(f);
	}

}

lval* builtin_gt(lenv* e, lval* a) {
  return builtin_ord(e, a, ">");
}

lval* builtin_lt(lenv* e, lval* a) {
  return builtin_ord(e, a, "<");
}

lval* builtin_ge(lenv* e, lval* a) {
  return builtin_ord(e, a, ">=");
}

lval* builtin_le(lenv* e, lval* a) {
  return builtin_ord(e, a, "<=");
}

lval* builtin_ord(lenv* e, lval* a, char* op) {
  CHECK_COUNT(a, 2, a->count, op);
  CHECK_TYPE(a, LVAL_NUM, a->cell[0]->type, op, get_lval_type(LVAL_NUM), a->cell[0]->type);
  CHECK_TYPE(a, LVAL_NUM, a->cell[1]->type, op, get_lval_type(LVAL_NUM), a->cell[1]->type);
  
  int r;
  if (strcmp(op, ">")  == 0) {
    r = (a->cell[0]->num >  a->cell[1]->num);
  }
  if (strcmp(op, "<")  == 0) {
    r = (a->cell[0]->num <  a->cell[1]->num);
  }
  if (strcmp(op, ">=") == 0) {
    r = (a->cell[0]->num >= a->cell[1]->num);
  }
  if (strcmp(op, "<=") == 0) {
    r = (a->cell[0]->num <= a->cell[1]->num);
  }
  if (strcmp(op, "&&") == 0) {
    r = (a->cell[0]->num && a->cell[1]->num);
  }
  if (strcmp(op, "||") == 0) {
    r = (a->cell[0]->num || a->cell[1]->num);
  }

  //free_lval(a);
  decrement_counter(a);
  return lval_num(r);
}

//comparison for different lval types
int lval_eq(lval* x, lval* y) {

  /* Different Types are always unequal */
  if (x->type != y->type) { return 0; }

  /* Compare Based upon type */
  switch (x->type) {
    /* Compare Number Value */
    case LVAL_NUM: return (x->num == y->num);

    /* Compare String Values */
    case LVAL_ERR: return (strcmp(x->err, y->err) == 0);
    case LVAL_SYM: return (strcmp(x->sym, y->sym) == 0);
    case LVAL_STR: return (strcmp(x->str, y->str) == 0);

    /* If builtin compare, otherwise compare formals and body */
    case LVAL_FUN:
      if (x->builtin || y->builtin) {
        return x->builtin == y->builtin;
      } else {
        return lval_eq(x->formals, y->formals) 
          && lval_eq(x->body, y->body);
      }

    /* If list compare every individual element */
    case LVAL_Q_EXPR:
    case LVAL_S_EXPR:
      if (x->count != y->count) { return 0; }
      for (int i = 0; i < x->count; i++) {
        /* If any element not equal then whole list not equal */
        if (!lval_eq(x->cell[i], y->cell[i])) { return 0; }
      }
      /* Otherwise lists must be equal */
      return 1;
    break;
  }
  return 0;
}

lval* builtin_cmp(lenv* e, lval* a, char* op) {
  CHECK_COUNT(a, 2, a->count, op);
  int r;
  if (strcmp(op, "==") == 0) {
    r =  lval_eq(a->cell[0], a->cell[1]);
  }
  if (strcmp(op, "!=") == 0) {
    r = !lval_eq(a->cell[0], a->cell[1]);
  }
  //free_lval(a);
  decrement_counter(a);
  return lval_num(r);
}

lval* builtin_eq(lenv* e, lval* a) {
  return builtin_cmp(e, a, "==");
}

lval* builtin_ne(lenv* e, lval* a) {
  return builtin_cmp(e, a, "!=");
}

lval* builtin_if(lenv* e, lval* a) {
  CHECK_COUNT(a, 3, a->count, "if");

  //args, expected_type, actual_type, function_name, expected_string, actual_string)
  CHECK_TYPE(a, LVAL_NUM, a->cell[0]->type, "if", get_lval_type(LVAL_NUM), get_lval_type(a->cell[0]->type));
  CHECK_TYPE(a, LVAL_Q_EXPR, a->cell[1]->type, "if", get_lval_type(LVAL_QEXPR), get_lval_type(a->cell[1]->type));
  CHECK_TYPE(a, LVAL_Q_EXPR, a->cell[2]->type, "if", get_lval_type(LVAL_QEXPR), get_lval_type(a->cell[2]->type));

  // LASSERT_TYPE("if", a, 0, LVAL_NUM);
  // LASSERT_TYPE("if", a, 1, LVAL_QEXPR);
  // LASSERT_TYPE("if", a, 2, LVAL_QEXPR);
  
  /* Mark Both Expressions as evaluable */
  lval* x;
  a->cell[1]->type = LVAL_S_EXPR;
  a->cell[2]->type = LVAL_S_EXPR;
  
  if (a->cell[0]->num) {
    /* If condition is true evaluate first expression */
    x = lval_eval(e, lval_pop(a, 1));
  } else {
    /* Otherwise evaluate second expression */
    x = lval_eval(e, lval_pop(a, 2));
  }
  
  /* Delete argument list and return */
  //free_lval(a);
  decrement_counter(a);
  return x;
}

lval* builtin_and(lenv*e, lval* a){
	return builtin_ord(e, a, "&&");
}

lval* builtin_or(lenv*e, lval* a){
	return builtin_ord(e, a, "||");
}

lval* builtin_not(lenv*e, lval* a){
	
	CHECK_COUNT(a, 1, a->count, "!");
	CHECK_TYPE(a, LVAL_NUM, a->cell[0]->type, !, get_lval_type(LVAL_NUM), a->cell[0]->type);

	int res = a->cell[0]->num == 0 ? 1:0; 

	//free_lval(a);
	decrement_counter(a);
	return lval_num(res);
}

lval* builtin_load(lenv* e, lval* a) {
	CHECK_COUNT(a, 1, a->count, "load");
	CHECK_TYPE(a, LVAL_STR, a->cell[0]->type, "load", lval_get_type(LVAL_STR), get_lval_type(a->cell[0]->type));

	/* Parse File given by string name */
	mpc_result_t r;
	if (mpc_parse_contents(a->cell[0]->str, Lispy, &r)) {

		/* Read contents */
		lval* expr = lval_read(r.output);
		mpc_ast_delete(r.output);

		/* Evaluate each Expression */
		while (expr->count) {
		  lval* x = lval_eval(e, lval_pop(expr, 0));
		  /* If Evaluation leads to error print it */
		  if (x->type == LVAL_ERR) { lval_println(x); }
		  //free_lval(x);
		  decrement_counter(x);
		}

		/* Delete expressions and arguments */
		//free_lval(expr);    
		//free_lval(a);
		decrement_counter(expr);
		decrement_counter(a);

		/* Return empty list */
		return lval_s_expr();

	} 
	else {
		/* Get Parse Error as String */
		char* err_msg = mpc_err_string(r.error);
		mpc_err_delete(r.error);

		/* Create new error message using it */
		lval* err = lval_err("Could not load Library %s", err_msg);
		free(err_msg);
		//free_lval(a);
		decrement_counter(a);

		/* Cleanup and return error */
		return err;
	}
}

lval* builtin_print(lenv* e, lval* a) {

  /* Print each argument followed by a space */
  for (int i = 0; i < a->count; i++) {
    lval_print(a->cell[i]); putchar(' ');
  }

  /* Print a newline and delete arguments */
  putchar('\n');
  //free_lval(a);
  decrement_counter(a);

  return lval_s_expr();
}

lval* builtin_error(lenv* e, lval* a) {

	CHECK_COUNT(a, 1, a->count, "error");
	CHECK_TYPE(a, LVAL_STR, a->cell[0]->type, "error", lval_get_type(LVAL_STR), get_lval_type(a->cell[0]->type));

	/* Construct Error from first argument */
	lval* err = lval_err(a->cell[0]->str);

	/* Delete arguments and return */
	//free_lval(a);
	decrement_counter(a);
	return err;
}

lval* builtin_read(lenv* e, lval* a){

	CHECK_COUNT(a, 1, a->count, "read");
	CHECK_TYPE(a, LVAL_STR, a->cell[0]->type, "read", lval_get_type(LVAL_STR), get_lval_type(a->cell[0]->type));

	//create q-expression lval
	lval* q_list = lval_s_expr(); //create s-expression
	q_list 	 	 = builtin_list(e, q_list); //conver to q-expression

	//add string-lval to q-expression
	lval* string_lval = lval_copy(a->cell[0]);
	q_list = lval_add(q_list, string_lval);

	//free_lval(a);
	decrement_counter(a);

	return q_list;
}