#include <stdio.h>
#include <stdlib.h>
#include <editline/readline.h>
#include "parser.h"
#include "vars.h"
#include "func.h"
#include "gc.h"

#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H
#include "data_structs.h"
#endif

//Compile Instructions cc -std=c99 -Wall main.c -ledit -o main

int main(int argc, char** argv) {

  /* Print Version and Exit Information */
  puts("Welcome to Midnight");
  puts("Version: 0.0.1");
  puts("Press Ctrl+c to Exit\n");

  //set up the garbage collector 
  lisp_gc = create_gc();

  //some setup for our polish grammer system
  setup_polish_grammer();

  //continue execution until Ctrl-C or the exit command 
  to_execute = true;

  //setup environment
  lenv* e = lenv_create();
  lenv_add_builtins(e);

  //load libraray
  load_std_lib(e);


  /* Supplied with list of files */
  if (argc >= 2) {

    /* loop over each supplied filename (starting from 1) */
    for (int i = 1; i < argc; i++) {

      /* Argument list with a single argument, the filename */
      lval* args = lval_add(lval_s_expr(), lval_str(argv[i]));

      /* Pass to builtin load and get the result */
      lval* x = builtin_load(e, args);

      /* If the result is an error be sure to print it */
      if (x->type == LVAL_ERR) { lval_println(x); }
      //free_lval(x);
      decrement_counter(x);
    }
  }
  else {
    /* In a never ending loop */
    while (to_execute) {

      /* Output our prompt and get input */
      char* input = readline("midnight> ");

      add_history(input);
      handle_input(e, input);

      free(input);

      //free garbage if required
      sweep(lisp_gc);
    }
  }

  lenv_del(e);
  polish_grammer_cleanup();
  return 0;
}