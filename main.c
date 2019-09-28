#include "list.h"
#include "hash.h"
#include "bitmap.h"
#include "debug.h"
#include "limits.h"
#include "round.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#define CREATE 0
#define DELETE 1
#define DUMP 2
#define OTHERS 3

void list_swap(struct list_elem *a, struct list_elem *b);
void list_shuffle(struct list *list);
unsigned hash_int_2(int i);
struct bitmap *bitmap_expand(struct bitmap *bitmap, int size);

bool execute(char *);
void create(char *);
void delete(char *);
void dumpdata(char *);
void list_handler(char *, int flag);
void bitmap_handler(char *, int flag);
void hash_handler(char *, int flag);
int str2int(char *);

struct list *L[10];

int main (void){

	char command[100];
	bool exit = false;

	while(!exit){
		fgets(command, sizeof(command), stdin);
		exit = execute(command);
	}

	return 0;
}

bool execute(char *command){
	bool exit = false;

	if( !strncmp( command, "create", 6) ){
		command += 7;
		create(command);
	}
	else if( !strncmp( command, "delete", 6)){
		command += 7;
		delete(command);
	}
	else if( !strncmp( command, "dumpdata", 8) ){
		command += 9;
		dumpdata(command);
	}
	else if( !strncmp( command, "quit", 4)){
		exit = true;
	}
	else if( !strncmp( command, "list", 4)){
		command += 5;
		list_handler(command, OTHERS);
	}
	else if( !strncmp( command, "bitmap", 6) ){
		command += 7;
		//bitmap_handler(command);
	}
	else if( !strncmp( command, "hash", 4) ){
		command += 5;
		//hash_handler(command);
	}
	return exit;
}

void create(char *command){

	//create list
	if( !strncmp( command, "list", 4) ){
		list_handler( command + 9, CREATE );
	}
}

void delete(char *command){

	// delete list
	if( !strncmp( command, "list", 4) ){
		list_handler( command + 4, DELETE );
	}
}	

void dumpdata( char *command ){

	//dump list
	if( !strncmp( command, "list", 4) ){
		list_handler( command + 4, DUMP );
	}
}

int str2int(char *str){
	int ret = 0;
	int idx = 0;

	while(1){
		if(str[idx] == '\0' || str[idx] == '\n'){
			break;
		}
		ret *= 10;
		ret += ( str[idx] - '0' );
		idx++;
	}
	return ret;
}

void list_handler(char *command, int flag){
	int idx;
	int i;
	struct list_elem *cur, *next;
	char *param[6];

	// split command
	param[0] = strtok( command, " ");
		
	i = 1;
	while(1){
		param[i] = strtok( NULL, " ");
		if( param[i] == NULL )
			break;
		i++;
	}


	switch(flag){
		case CREATE:{
						idx = command[0] - '0';
						L[idx] = (struct list*)malloc(sizeof(struct list));
						list_init(L[idx]);
						break;
					}
		case DELETE:{
						idx = command[0] - '0';
						for( cur = list_begin(L[idx]) ; cur != list_end(L[idx]) ; cur = next){
							next = list_next(cur);
							struct list_item *ditem = list_entry( cur, struct list_item, elem);
							free(ditem);
							ditem = NULL;
						}
						free( L[idx] );
						L[idx] = NULL;
						break;
					}
		case DUMP:{
					  idx = command[0] - '0';
					  for( cur = list_begin(L[idx]) ; cur != list_end( L[idx] ); cur = list_next(cur)){
						  struct list_item *temp = list_entry( cur, struct list_item, elem);
						  int temp_data = temp->data;
						  printf("%d ",temp_data);
					  }
					  printf("\n");
				  }
	}

	if(flag == OTHERS){
		
		if( !strncmp( param[0], "front", 5 ) ){
			// list_front list0: print value
			idx = param[1][4] - '0';
			cur = list_front( L[idx] );
			struct list_item *temp = list_entry( cur, struct list_item, elem);
			int temp_data = temp->data;
		 	printf("%d\n",temp_data);
		}

		else if( !strncmp( param[0], "back", 4 ) ){
			// list_back list0: print value
			idx = param[1][4] - '0';
			cur = list_back( L[idx] );
			struct list_item *temp = list_entry( cur, struct list_item, elem);
			int temp_data = temp->data;
			printf("%d\n",temp_data);
		}

		else if( !strncmp( param[0], "pop_back", 8 ) ){
			// list_pop_back list0
			idx = param[1][4] - '0';
			list_pop_back( L[idx] );
		}

		else if( !strncmp( param[0], "pop_front", 9)){
			// list_pop_front list0
			idx = param[1][4] - '0';
			list_pop_front( L[idx] );
		}
		
		else if( !strncmp( param[0], "push_front", 10)){
			// list_push_front list0 3
			idx = param[1][4] - '0';
			int val = str2int(param[2]);
			struct list_item* new_item = (struct list_item*)malloc(sizeof(struct list_item));
			new_item->data = val;
			list_push_front( L[idx], &(new_item->elem));
		}

		else if( !strncmp( param[0], "push_back", 9)){
			// list_push_back list0 1
			idx = param[1][4] - '0';
			int val = str2int(param[2]);
			struct list_item* new_item = (struct list_item*)malloc(sizeof(struct list_item));
			new_item->data = val;
			list_push_back( L[idx], &(new_item->elem));
		}

		else if( !strncmp( param[0], "insert_ordered", 14)){
			// list_insert_ordered list0 5: insert and then sort
			idx = param[1][4] - '0';
			int val = str2int(param[2]);
			void *aux;
			struct list_item* new_item = (struct list_item*)malloc(sizeof(struct list_item));
			new_item->data = val;
			list_push_front( L[idx], &(new_item->elem));
			list_sort( L[idx], list_less_function, aux);
		}


		else if( !strncmp( param[0], "insert", 6)){
			// list_insert list0 0 1: index, value
			idx = param[1][4] - '0';
			int before = param[2][0] - '0';
			int val = str2int(param[3]);

			for( i = 0, cur = list_begin(L[idx]) ; ; cur = list_next(cur)){
				if( i == before ){
					struct list_item* new_item = (struct list_item*)malloc(sizeof(struct list_item));
					new_item->data = val;
					list_insert(cur, &(new_item->elem));
				}
				if( cur == list_end(L[idx])){
					break;
				}
				i++;
			}

		}
/*
		else if( !strncmp( command, "empty", 5)){
			// list_empty list0: true or false
		}

		else if( !strncmp( command, "size", 4)){
			// list_size list0: print element size
		}

		else if( !strncmp( command, "max", 3)){
			// list_max list0: print max element
		}

		else if( !strncmp( command, "min", 3)){
			// list_min list0: print min element
		}

		else if( !strncmp( command, "remove", 6)){
			// list_remove list0 0
		}

		else if( !strncmp( command, "sort", 4)){
			// list_sort list0
		}
		
		else if( !strncmp( command, "reverse", 7)){
			// list_reverse list0
		}

		else if( !strncmp( command, "splice", 6)){
			// list_splice list0 2 list1 1 4: list0[2] list1[1]~list1[1+3] list0[3]
		}

		else if( !strncmp( command, "swap", 4)){
			// list_swap list0 1 3: [1] and [3] swap	
		}

		else if( !strncmp( command, "unique", 6)){
			// list_unique list0 list1: unique in list0, duplicate in list1
			// list_unique list1: unique in list1
		}

		*/
	}

	return;
}
