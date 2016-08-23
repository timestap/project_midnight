#include "parser.h"
#include <math.h>

#ifndef VARS_H
#define VARS_H
#include "vars.h"
#endif

void setup_polish_grammer(){

	/* Create Some Parsers */
	Number   = mpc_new("number");
	Symbol	 = mpc_new("symbol");
	S_expr 	 = mpc_new("s_expr");
	Expr     = mpc_new("expr");
	Q_expr    = mpc_new("q_expr");
	Lispy    = mpc_new("lispy");
  Lisp_String = mpc_new("string");
  Comment = mpc_new("comment");

	/* Define them with the following Language */
	//'add' | 'sub' | 'mul' | 'div' | 'mod' | 
	mpca_lang(MPCA_LANG_DEFAULT,
	  "                                                     \
	    number   : /-?[0-9]+/ ;                             \
      comment : /;[^\\r\\n]*/ ;                           \
      string   : /\"(\\\\.|[^\"])*\"/ ;                    \
	    symbol   : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&||&&]+/ |  \
	    		   /exp/  | '^'    | '%' |  '+' | '-' | '*' | '/' \
	    		   /join/ | /head/ | /list/ | /eval/ | /tail/ | \
	    		   /cons/ | /len/  | /init/ | /def/ | /exit/; \
	    s_expr   : '(' <expr>* ')' ;               \
	    q_expr  : '{' <expr>* '}' ;                         \
	    expr     : <number> | <symbol> | <s_expr> | <q_expr> | <string> | <comment>;  \
	    lispy    : /^/ <expr>* /$/ ;             \
	  ",
	  Number, Comment, Lisp_String, Symbol, S_expr, Q_expr, Expr, Lispy);
}


void polish_grammer_cleanup(){
	/* Undefine and Delete our Parsers */
	mpc_cleanup(8, Number, Comment, Lisp_String, Symbol, S_expr, Q_expr, Expr, Lispy);
}

void process_command(lenv* e, mpc_result_t* my_result){
	lval* result = lval_eval(e, lval_read(my_result->output));
	lval_print(result);
	putchar('\n');
  //free_lval(result);
  decrement_counter(result);
	mpc_ast_delete(my_result->output);
}

void handle_input(lenv* e, char* input){
	mpc_result_t r;

	/* Attempt to Parse the user Input */
	if (mpc_parse("<stdin>", input, Lispy, &r)) {
	  /* On Success Print the AST */
	  //mpc_ast_print(r.output);
	  process_command(e, &r);

	} else {
	  /* Otherwise Print the Error */
	  mpc_err_print(r.error);
	  mpc_err_delete(r.error);
	}
}

int number_of_nodes(mpc_ast_t* t) {
  if (t->children_num == 0) { return 1; }
  if (t->children_num >= 1) {
    int total = 1;
    for (int i = 0; i < t->children_num; i++) {
      total = total + number_of_nodes(t->children[i]);
    }
    return total;
  }
  return 0;
}

lval* lval_read_num(mpc_ast_t* t) {
  errno = 0;
  long x = strtol(t->contents, NULL, 10);
  return errno != ERANGE ?
    lval_num(x) : lval_err("invalid number");
}

lval* lval_read(mpc_ast_t* t) {

  /* If Symbol or Number return conversion to that type */
  if (strstr(t->tag, "number")) { return lval_read_num(t); }
  if (strstr(t->tag, "symbol")) { return lval_symb(t->contents); }

  /* If root (>) or s_expr then create empty list */
  lval* x = NULL;
  if (strcmp(t->tag, ">") == 0) { x = lval_s_expr(); } 
  if (strstr(t->tag, "s_expr"))  { x = lval_s_expr(); }
  if (strstr(t->tag, "q_expr")) {x = lval_q_expr();}
  if (strstr(t->tag, "string")) { return lval_read_str(t); }

  /* Fill this list with any valid expression contained within */
  for (int i = 0; i < t->children_num; i++) {
    if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
    if (strcmp(t->children[i]->contents, ")") == 0) { continue; }
    if (strcmp(t->children[i]->contents, "}") == 0) { continue; }
    if (strcmp(t->children[i]->contents, "{") == 0) { continue; }
    if (strcmp(t->children[i]->tag,  "regex") == 0) { continue; }
    if (strstr(t->children[i]->tag, "comment")) { continue; }
    x = lval_add(x, lval_read(t->children[i]));
  }

  return x;
}

lval* lval_add(lval* v, lval* x) {
  v->count++;
  v->cell = realloc(v->cell, sizeof(lval*) * v->count);
  v->cell[v->count-1] = x;
  return v;
}

void lval_expr_print(lval* v, char open, char close) {
  putchar(open);
  for (int i = 0; i < v->count; i++) {

    /* Print Value contained within */
    lval_print(v->cell[i]);

    /* Don't print trailing space if last element */
    if (i != (v->count-1)) {
      putchar(' ');
    }
  }
  putchar(close);
}

void lval_print(lval* v) {
  switch (v->type) {
    case LVAL_NUM:   printf("%li", v->num); break;
    case LVAL_ERR:   printf("Error: %s", v->err); break;
    case LVAL_SYM:   printf("%s", v->sym); break;
    case LVAL_S_EXPR: lval_expr_print(v, '(', ')'); break;
    case LVAL_Q_EXPR: lval_expr_print(v, '{', '}'); break;
    case LVAL_FUN:   
      if (v->builtin){
        printf("<function>: %s", get_function_name(v->function_opcode));
      }
      else{
        printf("(\\ "); lval_print(v->formals);
        putchar(' '); lval_print(v->body); putchar(')');
      }
      break;
    case LVAL_STR: lval_print_str(v); break;
  }
}

lval* lval_eval_s_expr(lenv* e, lval* v) {
	/* Evaluate Children */
	for (int i = 0; i < v->count; i++) {
		v->cell[i] = lval_eval(e, v->cell[i]);
		//error checking
		if (v->cell[i]->type == LVAL_ERR) { 
			return lval_take(v, i); 
		}
	}

	if (v->count == 0) { 
		return v; /* Empty Expression */
	}
	else if (v->count == 1){
    if ((v->cell[0]->type != LVAL_FUN) || (v->cell[0]->type == LVAL_FUN \
      && v->cell[0]->function_opcode != OP_EXIT))
    {
      return lval_take(v, 0);  /* Single Expression e.g. (5) */
    }
	}

	/* Ensure First Element is Function */
	lval* f = lval_pop(v, 0);
	if (f->type != LVAL_FUN) {
		//free_lval(f); 
		//free_lval(v);
    decrement_counter(f);
    decrement_counter(v);
		return lval_err("The first element in lval_eval_s_expr is not a function!");
	}

	  /* Call builtin with operator */
	  lval* result = lval_call(e, f, v);
	  //free_lval(f);
    decrement_counter(f);
	  return result;
}

lval* lval_eval(lenv* e, lval* v) {

  //retrieves the correct function based on the symbol name
  if (v->type == LVAL_SYM) {
    lval* x = lenv_get(e, v);
    //free_lval(v);
    decrement_counter(v);
    return x;
  }

  /* Evaluate Sexpressions */
  if (v->type == LVAL_S_EXPR) { return lval_eval_s_expr(e, v); }
  /* All other lval types remain the same */
  return v;
}

lval* lval_pop(lval* v, int i) {
  /* Find the item at "i" */
  lval* x = v->cell[i];

  /* Shift memory after the item at "i" over the top */
  memmove(&v->cell[i], &v->cell[i+1], sizeof(lval*) * (v->count-i-1));

  /* Decrease the count of items in the list */
  v->count--;

  /* Reallocate the memory used */
  v->cell = realloc(v->cell, sizeof(lval*) * v->count);
  return x;
}

lval* lval_take(lval* v, int i) {
  lval* x = lval_pop(v, i);
  //free_lval(v); 
  decrement_counter(v);
  return x;
}

/* A pointer to a new empty Qexpr lval */
lval* lval_q_expr(void) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_Q_EXPR;
  v->count = 0;
  v->cell = NULL;

  add_to_gc(lisp_gc, v);

  return v;
}

lval* builtin_head(lenv* e, lval* a) {
  /* Check Error Conditions */

  CHECK_COUNT(a, 1, a->count, "head");
  CHECK_EMPTY(a, a->cell[0]->count, "head");

  if (a->cell[0]->type == LVAL_Q_EXPR){
      CHECK_TYPE(a, LVAL_Q_EXPR, a->cell[0]->type, "head", \
      get_lval_type(LVAL_Q_EXPR), get_lval_type(a->cell[0]->type));

    /* Otherwise take first argument */
    lval* v = lval_take(a, 0);

    /* Delete all elements that are not head and return */
    while (v->count > 1) { decrement_counter(lval_pop(v, 1)); }
    return v;
  }
  else {//string type
    CHECK_TYPE(a, LVAL_STR, a->cell[0]->type, "head", \
    get_lval_type(LVAL_STR), get_lval_type(a->cell[0]->type));

    if (strlen(a->cell[0]->str) == 0){
      //free_lval(a);
      decrement_counter(a);
      return lval_err("String is empty");
    }

    char* head_val = (char*)malloc(2);
    strncpy(head_val, a->cell[0]->str, 1);
    head_val[1] = '\0';

    //free_lval(a);
    decrement_counter(a);
    return lval_str(head_val);
  }

}

lval* builtin_tail(lenv* e, lval* a) {
  /* Check Error Conditions */

  CHECK_COUNT(a, 1, a->count, "tail");
  CHECK_EMPTY(a, a->cell[0]->count, "tail");

  if (a->cell[0]->type == LVAL_Q_EXPR){
    CHECK_TYPE(a, LVAL_Q_EXPR, a->cell[0]->type, "tail", \
    get_lval_type(LVAL_Q_EXPR), get_lval_type(a->cell[0]->type));;

    /* Take first argument */
    lval* v = lval_take(a, 0);

    /* Delete first element and return */
    //free_lval(lval_pop(v, 0));
    decrement_counter(lval_pop(v, 0));
    return v;
  }
  else{
    CHECK_TYPE(a, LVAL_STR, a->cell[0]->type, "tail", \
    get_lval_type(LVAL_STR), get_lval_type(a->cell[0]->type));

    if (strlen(a->cell[0]->str) == 0){
      return lval_err("String is empty");
    }

    char* new_string = (char*) malloc(strlen(a->cell[0]->str)); 
    strncpy(new_string, a->cell[0]->str + 1, strlen(a->cell[0]->str) - 1);
    new_string[strlen(a->cell[0]->str)] = '\0';

    //free_lval(a);
    decrement_counter(a);
    return lval_str(new_string);
  }
}

lval* builtin_list(lenv* e, lval* a) {
  a->type = LVAL_Q_EXPR;
  return a;
}


lval* eval(lenv* e, lval* a) {

  CHECK_COUNT(a, 1, a->count, "eval");
  CHECK_TYPE(a, LVAL_Q_EXPR, a->cell[0]->type, "eval", \
    get_lval_type(LVAL_Q_EXPR), get_lval_type(a->cell[0]->type));;

  // LASSERT(a, a->count == 1,
  //   "Function 'eval' passed too many arguments!");
  // LASSERT(a, a->cell[0]->type == LVAL_Q_EXPR,
  //   "Function 'eval' passed incorrect type!");

  lval* x = lval_take(a, 0);
  x->type = LVAL_S_EXPR;
  return lval_eval(e, x);
}

lval* builtin_join(lenv* e, lval* a) {

  if (a->count > 0 && a->cell[0]->type == LVAL_Q_EXPR){
    for (int i = 0; i < a->count; i++) {
      CHECK_TYPE(a, LVAL_Q_EXPR, a->cell[i]->type, "builtin_join", \
        get_lval_type(LVAL_Q_EXPR), get_lval_type(a->cell[i]->type));
    }

    lval* x = lval_pop(a, 0);

    while (a->count) {
      x = lval_join(x, lval_pop(a, 0));
    }

    //free_lval(a);
    decrement_counter(a);
    return x;
  }
  else { //string join
    //CHECK_COUNT(args, expected_val, actual_val, function_name)
    CHECK_COUNT(a, 2, a->count, "builin_join"); 
    CHECK_TYPE(a, LVAL_STR, a->cell[0]->type, "builtin_join", \
      get_lval_type(LVAL_STR), get_lval_type(a->cell[0]->type));
    CHECK_TYPE(a, LVAL_STR, a->cell[1]->type, "builtin_join", \
      get_lval_type(LVAL_STR), get_lval_type(a->cell[1]->type));

    //enforce that both strings need to be non-empty
    int string1_len = strlen(a->cell[0]->str);
    int string2_len = strlen(a->cell[1]->str);
    int total_len = string1_len + string2_len;

    if (string1_len == 0 || string2_len == 0){
      return lval_err("Strings cannot be empty in join operation!");
    }

    char* new_string = (char*) malloc(strlen(a->cell[0]->str) + strlen(a->cell[1]->str) + 1);

    strncpy(new_string, a->cell[0]->str, string1_len);
    strncpy(new_string + string1_len, a->cell[1]->str, string2_len);
    new_string[total_len] = '\0';

    //free_lval(a);
    decrement_counter(a);
    return lval_str(new_string);
  }

}

lval* lval_join(lval* x, lval* y) {

  /* For each cell in 'y' add it to 'x' */
  while (y->count) {
    x = lval_add(x, lval_pop(y, 0));
  }

  /* Delete the empty 'y' and return 'x' */
  //free_lval(y);  
  decrement_counter(y);
  return x;
}

lval* cons(lenv* e, lval* x){
	//check that it has 2 children
  CHECK_COUNT(x, 2, x->count, "cons");
	//LASSERT(x, x->count == 2, "Function \"cons\" did not provide 2 arguements");
	
  //check second child is of the type Q expression
  CHECK_TYPE(x, LVAL_Q_EXPR, x->cell[1]->type, "cons", \
    get_lval_type(LVAL_Q_EXPR), get_lval_type(x->cell[0]->type));
	//LASSERT(x, x->cell[1]->type == LVAL_Q_EXPR, "Function \"cons\" does not contain Q expression as second argument");

	//create an empty Q-expression and add first child to it
	lval* result = lval_q_expr();
	result = lval_add(result, lval_pop(x, 0));

	//join the contents of second q expression to first 
	result = lval_join(result, lval_pop(x, 0));
	//free_lval(x);

  decrement_counter(x);

	return result;
}

//len {tail tail} --> S expression -> q expression -> number
lval* lval_len(lenv* e, lval* a){
	if (a->count == 0 || a->cell[0]->count == 0){
		return lval_num((long)0);
	}

  CHECK_TYPE(a, LVAL_Q_EXPR, a->cell[0]->type, "len", \
    get_lval_type(LVAL_Q_EXPR), get_lval_type(a->cell[0]->type));
	//LASSERT(a, a->cell[0]->type == LVAL_Q_EXPR, "Function 'len' passed incorrect type.");

	int count = a->cell[0]->count;

	//free_lval(a);
  decrement_counter(a);

	return lval_num((long)count);
}

//return 'a' without the last element
lval* init(lenv* e, lval* a){

  CHECK_COUNT(a, 1, a->count, "init");
  CHECK_TYPE(a, LVAL_Q_EXPR, a->cell[0]->type, "init", \
    get_lval_type(LVAL_Q_EXPR), get_lval_type(a->cell[0]->type));;
  CHECK_EMPTY(a, a->cell[0]->count, "init");


	//LASSERT(a, a->count == 1, "Function 'init' is empty");
  //LASSERT(a, a->cell[0]->type == LVAL_Q_EXPR, "Function 'init' is not contain a Q-expression");
  //LASSERT(a, a->cell[0]->count > 0, "Q-expression is empty");

  lval* result = lval_q_expr();
  int count = a->cell[0]->count - 1;
  for (int i = 0; i < count; i++){
    result = lval_add(result, lval_pop(a->cell[0], 0));
  }
  //free_lval(a);
  decrement_counter(a);

  return result;
}

lval* builtin(lenv* e, lval* a, char* func) {
  if (strcmp("list", func) == 0) { return builtin_list(e, a); }
  if (strcmp("head", func) == 0) { return builtin_head(e, a); }
  if (strcmp("tail", func) == 0) { return builtin_tail(e, a); }
  if (strcmp("join", func) == 0) { return builtin_join(e, a); }
  if (strcmp("eval", func) == 0) { return eval(e, a); }
  if (strcmp("cons", func) == 0) { return cons(e, a); }
  if (strcmp("len", func) == 0)  { return lval_len(e, a); }
  if (strcmp("init", func) == 0)  { return init(e, a); }
  if (strcmp("def", func) == 0)  { return builtin_def(e, a); }
  if (strcmp("exit", func) == 0)  { return exit_lisp(e, a); }
  if (strcmp("printall", func) == 0){return print_all_vars(e, a);}

  return builtin_op(e, a, func); 
 
}

void print_lval_debug(lval* x){
	lval_print(x);
	printf("\n");
	printf("Type          : %d\n", x->type);
	printf("Contents      : %li\n", x->num);
	printf("Children count: %d\n", x->count);

	if (x->err != NULL){
		printf("Error         : %s\n", x->err);
	}
	if (x->sym != NULL){
		printf("Symbol        : %s\n", x->sym);
	}
}

void lval_print_str(lval* v) {
  /* Make a Copy of the string */
  char* escaped = malloc(strlen(v->str)+1);
  strcpy(escaped, v->str);
  /* Pass it through the escape function */
  escaped = mpcf_escape(escaped);
  /* Print it between " characters */
  printf("\"%s\"", escaped);
  /* free the copied string */
  free(escaped);
}

lval* lval_read_str(mpc_ast_t* t) {
  /* Cut off the final quote character */
  t->contents[strlen(t->contents)-1] = '\0';
  /* Copy the string missing out the first quote character */
  char* unescaped = malloc(strlen(t->contents+1)+1);
  strcpy(unescaped, t->contents+1);
  /* Pass through the unescape function */
  unescaped = mpcf_unescape(unescaped);
  /* Construct a new lval using the string */
  lval* str = lval_str(unescaped);
  /* Free the string and return */
  free(unescaped);
  return str;
}

void lval_println(lval* v) { 
  lval_print(v); putchar('\n'); 
}

//will need to improve this if we want to load multiple standard libraries
void load_std_lib(lenv* e){
  
  //create q-expression lval
  lval* q_list = lval_s_expr(); //create s-expression
  q_list     = builtin_list(e, q_list); //conver to q-expression

  lval* std_lib = lval_str(STANDARD_LIB);
  q_list = lval_add(q_list, std_lib);
  builtin_load(e, q_list);

}
