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

void list_swap(struct list_elem *a, struct list_elem *b){

	struct list_item *aitem = list_entry( a, struct list_item, elem);
	int adata = aitem->data;

	struct list_item *bitem = list_entry( b, struct list_item, elem);
	int bdata = bitem->data;

	aitem->data = bdata;
	bitem->data = adata;
}


void list_shuffle(struct list *list){
	
	int shuffle_num = ( rand()% 5 ) + 5;
	int lsize = list_size( list );
	int a, b, idx;

	struct list_elem *aelem, *belem, *cur;

	for( int i = 0 ; i < shuffle_num ; i++ ){

		b = a = rand() % lsize;
		while( a == b ){
			b = rand() %lsize;
		}

		for( cur = list_begin(list), idx = 0 ; cur != list_end( list ); cur = list_next(cur), idx++){
			if( idx == a ){
				aelem = cur;
			}
			else if( idx == b ){
				belem = cur;
			}
		}
		list_swap( aelem, belem );
	}
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
		idx = param[1][4] - '0';

		if( !strncmp( param[0], "front", 5 ) ){
			// list_front list0: print value
			cur = list_front( L[idx] );
			struct list_item *temp = list_entry( cur, struct list_item, elem);
			int temp_data = temp->data;
		 	printf("%d\n",temp_data);
		}

		else if( !strncmp( param[0], "back", 4 ) ){
			// list_back list0: print value
			cur = list_back( L[idx] );
			struct list_item *temp = list_entry( cur, struct list_item, elem);
			int temp_data = temp->data;
			printf("%d\n",temp_data);
		}

		else if( !strncmp( param[0], "pop_back", 8 ) ){
			// list_pop_back list0
			list_pop_back( L[idx] );
		}

		else if( !strncmp( param[0], "pop_front", 9)){
			// list_pop_front list0
			list_pop_front( L[idx] );
		}
		
		else if( !strncmp( param[0], "push_front", 10)){
			// list_push_front list0 3
			int val = str2int(param[2]);
			struct list_item* new_item = (struct list_item*)malloc(sizeof(struct list_item));
			new_item->data = val;
			list_push_front( L[idx], &(new_item->elem));
		}

		else if( !strncmp( param[0], "push_back", 9)){
			// list_push_back list0 1
			int val = str2int(param[2]);
			struct list_item* new_item = (struct list_item*)malloc(sizeof(struct list_item));
			new_item->data = val;
			list_push_back( L[idx], &(new_item->elem));
		}

		else if( !strncmp( param[0], "insert_ordered", 14)){
			// list_insert_ordered list0 5: insert and then sort
			int val = str2int(param[2]);
			void *aux;
			struct list_item* new_item = (struct list_item*)malloc(sizeof(struct list_item));
			new_item->data = val;
			list_insert_ordered(L[idx], &(new_item->elem), list_less_function, aux);
		}


		else if( !strncmp( param[0], "insert", 6)){
			// list_insert list0 0 1: index, value
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

		else if( !strncmp( param[0], "empty", 5)){
			// list_empty list0: true or false
			if( list_empty(L[idx]) ){
				printf("true\n");
			}
			else{
				printf("false\n");
			}
		}

		else if( !strncmp( param[0], "size", 4)){
			// list_size list0: print element size
			int elemsize = 0;
			for( cur = list_begin(L[idx]) ; cur != list_end(L[idx]) ; cur = list_next(cur)){
				elemsize++;
			}
			printf("%d\n", elemsize);
		}

		else if( !strncmp( param[0], "max", 3)){
			// list_max list0: print max element
			void *aux;
			cur = list_max(L[idx], list_less_function, aux);
			struct list_item* eitem = list_entry( cur, struct list_item, elem);
			int val = eitem->data;
			printf("%d\n", val);
		}

		else if( !strncmp( param[0], "min", 3)){
			// list_min list0: print min element
			void *aux;
			cur = list_min(L[idx], list_less_function, aux);
			struct list_item* eitem = list_entry( cur, struct list_item, elem);
			int val = eitem->data;
			printf("%d\n", val);

		}

		else if( !strncmp( param[0], "remove", 6)){
			// list_remove list0 0
			i = 0;
			int target = str2int(param[2]);

			for( cur = list_begin(L[idx]) ; cur != list_end(L[idx]) ; cur = list_next(cur), i++){
				if(i == target){
					break;
				}
			}
			list_remove(cur);
			/*
			struct list_item *ditem = list_entry( cur, struct list_item, elem);
			free(ditem);
			ditem = NULL;
			*/
		}

		else if( !strncmp( param[0], "sort", 4)){
			// list_sort list0
			void *aux;
			list_sort( L[idx], list_less_function, aux);
		}
		
		else if( !strncmp( param[0], "reverse", 7)){
			// list_reverse list0
			list_reverse(L[idx]);
		}

		else if( !strncmp( param[0], "splice", 6)){
			// list_splice list0 2 list1 1 4: list0[2] list1[1]~list1[1+3] list0[3]
			int num = str2int( param[5] );
			int start1 = str2int( param[2] );
			int idx2 = param[3][4] - '0';
			int start2 = str2int( param[4] );
			int end2 = start2 + num - 1;
			struct list_elem *first;
			struct list_elem *before;
			struct list_elem *last;

			i = 0;
			for( cur = list_begin(L[idx2]) ; ; cur = list_next(cur), i++){
				if( i == start2 ){
					first = cur; 
				}
				else if( i == end2 ){
					last = cur;
					break;
				}
			}

			i = 0;
			for( cur = list_begin(L[idx]) ; ; cur = list_next(cur), i++){
				if(i == start1){
					before = cur;
					break;
				}
			}
			
			list_splice( before, first, last);
		}

		else if( !strncmp( param[0], "swap", 4)){
			// list_swap list0 1 3: [1] and [3] swap

			int a = str2int( param[2] );
			int b = str2int( param[3] );
			struct list_elem *aelem, *belem;

			i = 0;
			for( cur = list_begin(L[idx]) ; cur != list_end(L[idx]) ; cur = list_next(cur), i++){
				if(i == a){
					aelem = cur;
				}
				if( i == b ){
					belem = cur;
				}
			}
			list_swap( aelem, belem );
		}

		else if( !strncmp( param[0], "unique", 6)){
			// list_unique list0 list1: unique in list0, duplicate in list1
			// list_unique list1: unique in list1
			void *aux;
			int idx2 = -1;

			if(param[2] != NULL){
				idx2 = param[2][4] - '0';
				list_unique( L[idx], L[idx2], list_less_function, aux);
			}

			else{
				list_unique( L[idx], NULL, list_less_function, aux);
			}
		}

		else if( !strncmp( param[0], "shuffle", 7)){
			
			list_shuffle( L[idx] );
		}

	}

	return;
}
