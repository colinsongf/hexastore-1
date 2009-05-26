#ifndef _VARIABLEBINDINGS_H
#define _VARIABLEBINDINGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "hexastore_types.h"
#include "nodemap.h"
#include "node.h"

enum {
	HX_VARIABLEBINDINGS_FREE_NAMES		= 1,
	HX_VARIABLEBINDINGS_NO_FREE_NAMES	= 0
};

typedef struct {
	int size;
	int free_names;
	char** names;
	hx_node_id* nodes;
} hx_variablebindings;

typedef struct {
	int (*finished) ( void* iter );
	int (*current) ( void* iter, void* results );
	int (*next) ( void* iter );	
	int (*free) ( void* iter );
	int (*size) ( void* iter );
	char** (*names) ( void* iter );
	int (*sorted_by) ( void* iter, int index );
	int (*debug) ( void* iter, char* header, int indent );
} hx_variablebindings_iter_vtable;

typedef struct {
	int size;
	char** names;
	hx_variablebindings_iter_vtable* vtable;
	void* ptr;
} hx_variablebindings_iter;

#include "materialize.h"

hx_variablebindings* hx_new_variablebindings ( int size, char** names, hx_node_id* nodes, int free_names );
hx_variablebindings* hx_copy_variablebindings ( hx_variablebindings* b );
int hx_free_variablebindings ( hx_variablebindings* b, int free_names );

hx_variablebindings* hx_variablebindings_project ( hx_variablebindings* b, int newsize, int* columns );
hx_variablebindings* hx_variablebindings_project_names ( hx_variablebindings* b, int newsize, char** names );

int hx_variablebindings_string ( hx_variablebindings* b, hx_nodemap* m, char** string );
void hx_variablebindings_debug ( hx_variablebindings* b, hx_nodemap* m );

int hx_variablebindings_set_names ( hx_variablebindings* b, char** names, int free_names );
int hx_variablebindings_size ( hx_variablebindings* b );
char* hx_variablebindings_name_for_binding ( hx_variablebindings* b, int column );
hx_node_id hx_variablebindings_node_id_for_binding ( hx_variablebindings* b, int column );
hx_node_id hx_variablebindings_node_id_for_binding_name ( hx_variablebindings* b, char* name );
hx_node* hx_variablebindings_node_for_binding ( hx_variablebindings* b, hx_nodemap* map, int column );
hx_node* hx_variablebindings_node_for_binding_name ( hx_variablebindings* b, hx_nodemap* map, char* name );
char** hx_variablebindings_names ( hx_variablebindings* b );
int hx_variablebindings_cmp ( void* a, void* b );

hx_variablebindings* hx_variablebindings_natural_join( hx_variablebindings* left, hx_variablebindings* right );

hx_variablebindings_iter* hx_variablebindings_new_empty_iter ( void );
hx_variablebindings_iter* hx_variablebindings_new_iter ( hx_variablebindings_iter_vtable* vtable, void* ptr );
int hx_free_variablebindings_iter ( hx_variablebindings_iter* iter, int free_vtable );
int hx_variablebindings_iter_finished ( hx_variablebindings_iter* iter );
int hx_variablebindings_iter_current ( hx_variablebindings_iter* iter, hx_variablebindings** b );
int hx_variablebindings_iter_next ( hx_variablebindings_iter* iter );
int hx_variablebindings_iter_size ( hx_variablebindings_iter* iter );
char** hx_variablebindings_iter_names ( hx_variablebindings_iter* iter );
int hx_variablebindings_column_index ( hx_variablebindings_iter* iter, char* column );
int hx_variablebindings_iter_is_sorted_by_index ( hx_variablebindings_iter* iter, int index );
int hx_variablebindings_iter_debug ( hx_variablebindings_iter* iter, char* header, int indent );

hx_variablebindings_iter* hx_variablebindings_sort_iter( hx_variablebindings_iter* iter, int index );

#ifdef __cplusplus
}
#endif

#endif
