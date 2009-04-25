#ifndef _INDEX_H
#define _INDEX_H

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

#include "hexastore_types.h"
#include "head.h"

typedef struct {
	int order[3];
	hx_storage_id_t head;
} hx_index;

enum {
	HX_INDEX_ITER_DUP_NONE	= 0,
	HX_INDEX_ITER_DUP_A		= 1,
	HX_INDEX_ITER_DUP_B		= 2,
	HX_INDEX_ITER_DUP_C		= 3
};

typedef struct {
	hx_storage_manager* storage;
	hx_index* index;
	int flags;
	hx_node_id node_mask_a, node_mask_b, node_mask_c;
	char node_dup_b;
	char node_dup_c;
	hx_head_iter* head_iter;
	hx_vector_iter* vector_iter;
	hx_terminal_iter* terminal_iter;
	int started;
	int finished;
} hx_index_iter;

static int HX_INDEX_ORDER_SPO[3]	= { HX_SUBJECT, HX_PREDICATE, HX_OBJECT };
static int HX_INDEX_ORDER_SOP[3]	= { HX_SUBJECT, HX_OBJECT, HX_PREDICATE };
static int HX_INDEX_ORDER_PSO[3]	= { HX_PREDICATE, HX_SUBJECT, HX_OBJECT };
static int HX_INDEX_ORDER_POS[3]	= { HX_PREDICATE, HX_OBJECT, HX_SUBJECT };
static int HX_INDEX_ORDER_OSP[3]	= { HX_OBJECT, HX_SUBJECT, HX_PREDICATE };
static int HX_INDEX_ORDER_OPS[3]	= { HX_OBJECT, HX_PREDICATE, HX_SUBJECT };

/* hx_index* hx_new_index ( int a, int b, int c ); */
hx_index* hx_new_index ( hx_storage_manager* s, int* index_order );
int hx_free_index ( hx_index* index, hx_storage_manager* s );
int hx_index_debug ( hx_index* index, hx_storage_manager* s );
int hx_index_add_triple ( hx_index* index, hx_storage_manager* st, hx_node_id s, hx_node_id p, hx_node_id o );
int hx_index_add_triple_terminal ( hx_index* index, hx_storage_manager* st, hx_node_id s, hx_node_id p, hx_node_id o, hx_terminal** t );
int hx_index_add_triple_with_terminal ( hx_index* index, hx_storage_manager* st, hx_terminal* t, hx_node_id s, hx_node_id p, hx_node_id o, int _new );
int hx_index_remove_triple ( hx_index* i, hx_storage_manager* st, hx_node_id s, hx_node_id p, hx_node_id o );
hx_storage_id_t hx_index_triples_count ( hx_index* index, hx_storage_manager* st );
hx_head* hx_index_head ( hx_index* index, hx_storage_manager* st );

int hx_index_write( hx_index* t, hx_storage_manager* s, FILE* f );
hx_index* hx_index_read( hx_storage_manager* w, FILE* f, int buffer );

hx_index_iter* hx_index_new_iter ( hx_index* index, hx_storage_manager* st );
hx_index_iter* hx_index_new_iter1 ( hx_index* index, hx_storage_manager* st, hx_node_id s, hx_node_id p, hx_node_id o );
int hx_free_index_iter ( hx_index_iter* iter );

int hx_index_iter_finished ( hx_index_iter* iter );
int hx_index_iter_current ( hx_index_iter* iter, hx_node_id* s, hx_node_id* p, hx_node_id* o );
int hx_index_iter_next ( hx_index_iter* iter );

int hx_index_iter_is_sorted_by_index ( hx_index_iter* iter, int index );

#ifdef __cplusplus
}
#endif

#endif
