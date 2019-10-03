#include "list.h"
#include "hash.h"
#include "bitmap.h"
#include "bitmap.c"
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

// LIST
void list_swap(struct list_elem *a, struct list_elem *b);
void list_shuffle(struct list *list);

// HASH
unsigned hash_int_2(int i);
unsigned hash_hash_function( const struct hash_elem *e, void *aux );
bool hash_less_function( const struct hash_elem *a, const struct hash_elem *b, void *aux );
void hash_action_on_element( struct hash_elem *e, void *aux );
void hash_action_destructor( struct hash_elem *e, void *aux );

// BITMAP
struct bitmap *bitmap_expand(struct bitmap *b, int size);

// MAIN
bool execute(char *);
void create(char *);
void delete(char *);
void dumpdata(char *);
void list_handler(char *, int flag);
void bitmap_handler(char *, int flag);
void hash_handler(char *, int flag);
int str2int(char *);

char *param[6];
struct list *L[10];
struct hash *H[10];
struct hash_iterator *Hash_Iterator;
struct bitmap *B[10];

int main (void){

	char command[100];
	bool exit = false;
	Hash_Iterator = (struct hash_iterator *)malloc(sizeof( struct hash_iterator));

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
		bitmap_handler(command, OTHERS);
	}
	else if( !strncmp( command, "hash", 4) ){
		command += 5;
		hash_handler(command, OTHERS);
	}

	return exit;
}

void create(char *command){

	//create list
	if( !strncmp( command, "list", 4) ){
		list_handler( command + 9, CREATE );
	}

	//create hashtable
	else if( !strncmp( command, "hashtable", 9) ){
		hash_handler( command + 14, CREATE );
	}

	//create bitmap
	else if( !strncmp( command, "bitmap", 6)){
		bitmap_handler( command + 7, CREATE);
	}
}

void delete(char *command){

	// delete list
	if( !strncmp( command, "list", 4) ){
		list_handler( command + 4, DELETE );
	}

	//delete hashtable
	else if( !strncmp( command, "hash", 4) ){
		hash_handler( command + 4, DELETE );
	}

	//delete bitmap
	else if( !strncmp( command, "bm", 2 )){
		bitmap_handler( command + 2, DELETE );
	}

}	

void dumpdata( char *command ){

	//dump list
	if( !strncmp( command, "list", 4) ){
		list_handler( command + 4, DUMP );
	}

	//dump hashtable
	else if( !strncmp( command, "hash", 4) ){
		hash_handler( command + 4, DUMP );
	}

	//dump bitmap
	else if( !strncmp( command, "bm", 2)){
		bitmap_handler( command + 2, DUMP);
	}

}

int str2int(char *str){
	int ret = 0;
	int idx = 0;
	bool neg = false;
	
	if(str[idx] == '-'){
		neg = true;
		idx++;
	}

	while(1){
		if(str[idx] == '\0' || str[idx] == '\n'){
			break;
		}
		ret *= 10;
		ret += ( str[idx] - '0' );
		idx++;
	}

	if(neg){
		ret *= (-1);
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
			struct list_item *ditem = list_entry( cur, struct list_item, elem);
			free(ditem);
			ditem = NULL;
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

#define FNV_32_PRIME 16777619u
#define FNV_32_BASIS 2166136261u

unsigned hash_int_2(int i){
	
	unsigned hash = FNV_32_BASIS;

	while(i>0){
		if(i%2 == 1){
			hash = hash ^ FNV_32_PRIME;
		}
		i/=2;
	}

	return hash;
}

unsigned hash_hash_function( const struct hash_elem *e, void *aux ){
	struct hash_item *hitem = hash_entry( e, struct hash_item, elem);
	int val = hitem->data;
	return hash_int(val);
}

bool hash_less_function( const struct hash_elem *a, const struct hash_elem *b, void *aux ){
	struct hash_item *aitem = hash_entry( a, struct hash_item, elem);
	int aval = aitem->data;

	struct hash_item *bitem = hash_entry( b, struct hash_item, elem);
	int bval = bitem->data;

	return (aval < bval);
}

#define SQUARE 0
#define TRIPLE 1
void hash_action_on_element( struct hash_elem *e, void *aux ){
	int flag = *(int *)aux;

	struct hash_item *hitem = hash_entry( e, struct hash_item, elem);
	int val = hitem->data;

	switch(flag){
		case SQUARE:
			{
				hitem->data = val * val;
				break;
			}
		case TRIPLE:
			{
				hitem->data = val * val * val;
				break;
			}
	}
}

void hash_action_destructor( struct hash_elem *e, void *aux ){

	struct hash_item *hitem = hash_entry( e, struct hash_item, elem);
	free( hitem );
	hitem = NULL;

}

void hash_handler( char *command, int flag){
	int idx;

	// split command
	param[0] = strtok( command, " ");
		
	int i = 1;
	while(1){
		param[i] = strtok( NULL, " ");
		if( param[i] == NULL )
			break;
		i++;
	}


	switch(flag){
		case CREATE:{
						idx = command[0] - '0';
						H[idx] = (struct hash*)malloc(sizeof(struct hash));
						void *aux;
						hash_init( H[idx], hash_hash_function, hash_less_function, aux);
						break;
					}
		case DELETE:{
						idx = command[0] - '0';
						hash_destroy( H[idx], hash_action_destructor);
						break;
					}
		case DUMP:{
					  idx = command[0] - '0';
					  hash_first( Hash_Iterator, H[idx] );
					  struct hash_elem *cur;

					  while(1){
						  cur = hash_next( Hash_Iterator );
						  if( cur == NULL ){
							  break;
						  }
						  struct hash_item *hitem = hash_entry( cur, struct hash_item, elem);
						  int val = hitem->data;
						  printf("%d ", val);
					  }
					  printf("\n");
				  }
	}

	if(flag == OTHERS){
		idx = param[1][4] - '0';
		
		if( !strncmp( param[0], "insert", 6) ){
			// hash_insert hash0 -1: not duplicate
			int val = str2int(param[2]);			
			struct hash_item *new_item = (struct hash_item *)malloc( sizeof( struct hash_item) );
			new_item->data = val;
			hash_insert( H[idx], &(new_item->elem));
		}

		else if( !strncmp( param[0], "apply", 5) ){
			// hash_apply hash0 square,triple: elements become square, triple
			void *aux;
			int flag;

			if( !strncmp( param[2], "square", 6)){
				flag = SQUARE;
			}
			else if( !strncmp( param[2], "triple", 6)){
				flag = TRIPLE;
			}
			
			aux = &flag;
			H[idx]->aux = aux;
			hash_apply( H[idx], hash_action_on_element);
		}

		else if( !strncmp( param[0], "delete", 6) ){
			// hash_delete hash0 10: delete element whose value is 10
			int target = str2int(param[2]);

			struct hash_item *new_item = (struct hash_item *)malloc( sizeof( struct hash_item) );
			new_item->data = target;
			hash_delete( H[idx], &(new_item->elem));
			free(new_item);
			new_item = NULL;
		}

		else if( !strncmp( param[0], "empty", 5) ){
			// hash_empty hash0: print true/false
			if( hash_empty( H[idx] ) ){
				printf("true\n");
			}
			else{
				printf("false\n");
			}
		}

		else if( !strncmp( param[0], "size", 4) ){
			// hash_size hash0: print hash element number
			printf("%d\n", (int)hash_size( H[idx]));
		}

		else if( !strncmp( param[0], "clear", 5 ) ){
			// hash_clear hash0
			hash_clear( H[idx], hash_action_destructor );
		}

		else if( !strncmp( param[0], "find", 4) ){
			// hash_find hash0 10: if find, print value(10)/ else, print nothing
			int target = str2int(param[2]);
			struct hash_elem *found;

			struct hash_item *new_item = (struct hash_item *)malloc( sizeof( struct hash_item) );
			new_item->data = target;
			found = hash_find( H[idx], &(new_item->elem));
			
			if( found != NULL ){
				printf("%d\n", target);
			}
		}

		else if( !strncmp( param[0], "replace", 7) ){
			// hash_replace hash0 10: what difference between insert?
			int val = str2int( param[2] );
			struct hash_item *new_item = (struct hash_item *)malloc( sizeof( struct hash_item) );
			new_item->data = val;

			hash_replace( H[idx], &(new_item->elem) );
		}

	}
}

struct bitmap *bitmap_expand(struct bitmap *b, int size){
	int orgsize = bitmap_size( b );
	b->bit_cnt = orgsize + size;
	b->bits = (elem_type *)realloc( b->bits, (orgsize + size) * sizeof(elem_type));
	bitmap_set_multiple( b, orgsize, size, false);
	return b;
}

void bitmap_handler( char *command, int flag){
	int idx;
	int i;

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
						// create bitmap bm0 16
						idx = param[0][2] - '0';
						int bsize = str2int( param[1] );
						B[idx] = bitmap_create( bsize );
						break;
					}
		case DELETE:{
						idx = command[0] - '0';
						bitmap_destroy( B[idx] );
						break;
					}
		case DUMP:{
					  idx = command[0] - '0';
					  int bsize = (int)( bitmap_size( B[idx] ));

					  for( i = 0; i < bsize ; i++ ){
						  printf("%d",(int)(bitmap_test(B[idx], i)));
					  }
					  printf("\n");
				  }
	}

	if(flag == OTHERS){
		idx = param[1][2] - '0';

		int index1, num;

		if( !strncmp( param[0], "mark", 4)) {
			// bitmap_mark bm0 0: [0] <- 1
			index1 = str2int( param[2] );
			bitmap_mark( B[idx], index1 );
		}

		else if( !strncmp( param[0], "all", 3) ){
			// bitmap_all bm0 0 1: [0] - [0 + 1 - 1] all set true? => print true/false
			index1 = str2int( param[2] );
			num = str2int( param[3] );
			if( bitmap_all( B[idx],index1, num ) ){
				printf("true\n");
			}
			else{
				printf("false\n");
			}
		}

		else if( !strncmp( param[0], "any", 3) ){
			// bitmap_any bm0 0 1: [0] - [0 + 1 - 1] any set true? => print true/false
			index1 = str2int( param[2] );
			num = str2int( param[3] );
			if( bitmap_any( B[idx], index1, num) ){
				printf("true\n");
			}
			else{
				printf("false\n");
			}
		}
		
		else if( !strncmp( param[0], "contains", 8) ){
			// bitmap_contains bm0 0 2 true: [0] - [0 + 2 - 1] contain true? => print true/ false
			index1 = str2int( param[2] );
			num = str2int( param[3] );
			bool val = false;
			if( !strncmp( param[4], "true", 4 ) ){
				val = true;
			}
			
			if( bitmap_contains( B[idx], index1, num, val ) ){
				printf("true\n");
			}
			else{
				printf("false\n");
			}

		}

		else if( !strncmp( param[0], "count", 5 )){
			// bitmap_count bm0 0 8 true: [0] - [0 + 8 - 1] how many true?
			index1 = str2int( param[2] );
			num = str2int( param[3] );
			bool val = false;
			if( !strncmp( param[4], "true", 4 )){
				val = true;
			}

			printf( "%d\n", (int)bitmap_count( B[idx], index1, num, val));
		}

		else if( !strncmp( param[0], "dump", 4 )){
			// bitmap_dump bm0
			bitmap_dump( B[idx] );
		}

		else if( !strncmp( param[0], "expand", 6 )){
			// bitmap_expand bm0 4
			num = str2int( param[2] );
			bitmap_expand( B[idx], num);
		}
		
		else if( !strncmp( param[0], "set_multiple", 12 )){
			// bitmap_set_multiple bm0 0 4 true
			index1 = str2int( param[2] );
			num = str2int( param[3] );
			bool val = false;
			if( !strncmp( param[4], "true", 4 )){
				val = true;
			}
			bitmap_set_multiple( B[idx], index1, num, val);
		}

		else if( !strncmp( param[0], "set_all", 7 )){
			// bitmap_set_all bm0 false
			bool val = false;
			if( !strncmp( param[2], "true", 4 )){
				val = true;
			}
			bitmap_set_all( B[idx], val);
		}

		else if( !strncmp( param[0], "set", 3 )){
			// bitmap_set bm0 0 true
			index1 = str2int( param[2] );
			bool val = false;
			if( !strncmp( param[3], "true", 4 )){
				val = true;
			}
			
			bitmap_set( B[idx], index1, val);
		}

		
		else if( !strncmp( param[0], "flip", 4 )){
			// bitmap_flip bm0 4
			index1 = str2int( param[2] );
			bitmap_flip( B[idx], index1 );
		}

		else if( !strncmp( param[0], "none", 4 )){
			// bitmap_none bm0 0 1
			index1 = str2int( param[2] );
			num = str2int( param[3] );
			if( bitmap_none( B[idx], index1, num )){
				printf("true\n");
			}
			else{
				printf("false\n");
			}
		}
		
		else if( !strncmp( param[0], "reset", 5 )){
			//bitmap_reset bm0 0
			num = str2int( param[2] );
			bitmap_reset( B[idx], num);
		}
		
		else if( !strncmp( param[0], "scan_and_flip", 13 )){
			//bitmap_scan_and_flip bm0 0 1 true
			index1 = str2int( param[2] );
			num = str2int( param[3] );
			bool val = false;
			if( !strncmp( param[4], "true", 4 )){
				val = true;
			}
			
			size_t ret = bitmap_scan_and_flip( B[idx], index1, num, val);
			if( ret == BITMAP_ERROR ){
				printf("4294967295\n");
			}
			else{
				printf("%d\n",(int)ret);
			}
		}

		else if( !strncmp( param[0], "scan", 4 )){
			//bitmap_scan bm0 0 1 true
			index1 = str2int( param[2] );
			num = str2int( param[3] );
			bool val = false;
			if( !strncmp( param[4], "true", 4 )){
				val = true;
			}
			size_t ret = bitmap_scan(B[idx], index1, num, val);
			if( ret == BITMAP_ERROR ){
				printf("4294967295\n");
			}
			else{
				printf("%d\n",(int)ret);
			}

		}

		else if( !strncmp( param[0], "size", 4 )){
			// bitamp_size bm0
			printf("%d\n", (int)(bitmap_size(B[idx])));
		}
		
		else if( !strncmp( param[0], "test", 4 )){
			// bitmap_test bm0 4
			index1 = str2int(param[2]);
			if( bitmap_test(B[idx], index1) ){
				printf("true\n");
			}
			else{
				printf("false\n");
			}
		}
	}
}
