//data structures

typedef struct lval lval; 
typedef struct lenv lenv; 
typedef struct lval_metadata lval_metadata;

#define STANDARD_LIB "lib.lspy"
#define NUM_VARS 5000
#define SAFETY_FACTOR 3500

typedef lval*(*lbuiltin)(lenv*, lval*);

struct lenv {
  int count;
  char** syms;
  lval** vals;
  lenv* parent;
};

//get boolean values in C
typedef int bool;
#define true 1
#define false 0

#define DEBUG_FLAG_BASIC 1
#define DEBUG_FLAG_ADVANCED 0 

bool to_execute;

struct lval{
  int type; //error or actual lval
  long num; //contents at this level

  int function_opcode; //we use this to help debug and check what function is running currenting

  //string data for err & symbols 
  char* err;
  char* msg;
  char* sym;
  char* str;

  //function stuff
  lbuiltin builtin;
  lenv* env;
  lval* formals; //arguements to function
  lval* body; //body of function

  //children lvals that it contains 
  int count; //number of children
  struct lval** cell; //contain reference to itself

  //used to go back to the garbage collector 
  lval_metadata* meta_data; 

};

//sym are operators +, -, etc..
//SEXPR are s expressions 
//Q expressions are {}, and are not evaluated by traditional mechanis
//S expressions are (), are are evaluated like thus + 10 10
enum { LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_S_EXPR, LVAL_Q_EXPR, LVAL_FUN, LVAL_STR};  
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM }; //types of errors

//function opcodes
enum {OP_LIST, OP_HEAD, OP_TAIL, OP_EVAL, OP_JOIN, OP_CONS, OP_LEN, OP_INIT, \
  OP_EXIT, OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD, OP_POW, OP_DEF, OP_PRINTALL, \
  OP_PUT, OP_LAMBDA,  OP_IF, OP_EQUAL, OP_NOT_EQUAL, OP_GREATER_THAN, OP_LESS_THAN, \
  OP_GREATER_EQUAL, OP_LESS_EQUAL, OP_AND, OP_OR, OP_NOT, OP_LOAD, OP_ERR, OP_PRINT, OP_READ, \
  OP_GC};

#define IN_USE 1
#define NOT_IN_USE 0

//data structure which contains data for each 
struct lval_metadata{
  int ref_count; //number of variables, functions, data structures which make use of this value
  int in_use; 
  lval* lval_pointee;
};

//data structure for global garbage collector 
typedef struct gc {
  int cur_num_vars; //current number of variables/functions 
  lval_metadata *md_array[NUM_VARS];
} gc;

gc* lisp_gc;
