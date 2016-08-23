#include "gc.h"

//If the current number of variables is greater than the safety factor then we do a sweep, 
//which cleans out variables are in use but not reference counted
void sweep(gc* my_gc){

	if (DEBUG_FLAG_BASIC){
		printf("Begining GC Sweep...\n");
	}

	int empty_counter = 0;

	if (my_gc->cur_num_vars >= SAFETY_FACTOR){
		for (int i = 0; i < NUM_VARS; i++){
			if (my_gc->md_array[i]->in_use == IN_USE && my_gc->md_array[i]->ref_count == 0){

				if (DEBUG_FLAG_ADVANCED){
					printf("Emptying out md_array at index: %i\n", i);
				}
				if (DEBUG_FLAG_BASIC) empty_counter++;
				
				free_lval(my_gc->md_array[i]->lval_pointee);
				my_gc->md_array[i]->in_use = NOT_IN_USE;
				my_gc->cur_num_vars--;
			}
		}
	}

	if (DEBUG_FLAG_BASIC) printf("Number of elements cleaned: %i\n", empty_counter);

}

lval_metadata* create_lval_metadata(){
	lval_metadata* md = (lval_metadata*)malloc(sizeof(lval_metadata));
	md->ref_count = 0;
	md->in_use = NOT_IN_USE;
	md->lval_pointee = NULL;

	return md;
}

//creates the gc data structure
gc* create_gc(){
	gc* my_gc = (gc*) malloc(sizeof(gc));
	my_gc->cur_num_vars = 0;

	for (int i = 0; i < NUM_VARS; i++){
		my_gc->md_array[i] = create_lval_metadata();
	}

	return my_gc;
}

//strictly for adding new variables to our global garbage collector 
//datastructure 
void add_to_gc(gc* my_gc, lval* new_lval){

	assert(my_gc->cur_num_vars < NUM_VARS);
	for (int i = 0; i < NUM_VARS; i++){

		if (my_gc->md_array[i]->in_use == NOT_IN_USE){
			my_gc->md_array[i]->ref_count = 1;
			my_gc->md_array[i]->in_use = IN_USE;
			my_gc->md_array[i]->lval_pointee = new_lval;
			new_lval->meta_data = my_gc->md_array[i];
			my_gc->cur_num_vars++;
			return;
		}
	}

	printf("Current ref count is: %d", my_gc->cur_num_vars);
	printf("Adding to garbage collector datastructure failed!\n");
}

//this allows us to skip the lval_copy for a lot of operations 
//which is super helpful for large objects
void assign_lval(lval** new_pointer, lval* new_lval){
	
	//assign the contents of new_lval to new_pointer
	*new_pointer = new_lval;

	//increase the counter
	new_lval->meta_data->ref_count++;
}

//instead of freeval, we basically mark for deletion instead 
//once we hit the SAFETY_FACTOR, then we do the actual deletion 
void decrement_counter(lval* cur_lval){
	cur_lval->meta_data->ref_count--;
	//cur_lval->meta_data = NULL;
}

