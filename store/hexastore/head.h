#ifndef _HEAD_H
#define _HEAD_H

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

#include "store/hexastore/btree.h"
#include "mentok_types.h"
#include "store/hexastore/vector.h"

#define HEAD_TREE_BRANCHING_SIZE				252

typedef struct {
	uintptr_t triples_count;
	uintptr_t tree;
} hx_head;

typedef struct {
	hx_head* head;
	hx_btree_iter* t;
} hx_head_iter;

hx_head* hx_new_head ( void* world );
int hx_free_head ( hx_head* head );

int hx_head_debug ( const char* header, hx_head* h );
int hx_head_add_vector ( hx_head* h, hx_node_id n, hx_vector* v );
hx_vector* hx_head_get_vector ( hx_head* h, hx_node_id n );
int hx_head_remove_vector ( hx_head* h, hx_node_id n );
list_size_t hx_head_size ( hx_head* h );
uintptr_t hx_head_triples_count ( hx_head* h );
void hx_head_triples_count_add ( hx_head* h, int c );

int hx_head_write( hx_head* t, FILE* f );
hx_head* hx_head_read( FILE* f, int buffer );

hx_head_iter* hx_head_new_iter ( hx_head* head );
int hx_free_head_iter ( hx_head_iter* iter );
int hx_head_iter_finished ( hx_head_iter* iter );
int hx_head_iter_current ( hx_head_iter* iter, hx_node_id* n, hx_vector** v );
int hx_head_iter_next ( hx_head_iter* iter );
int hx_head_iter_seek( hx_head_iter* iter, hx_node_id n );

#ifdef __cplusplus
}
#endif

#endif
