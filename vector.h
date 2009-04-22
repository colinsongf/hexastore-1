#ifndef _VECTOR_H
#define _VECTOR_H

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
#include "btree.h"
#include "storage.h"
#include "terminal.h"

typedef struct {
	hx_storage_id_t triples_count;
	hx_storage_id_t tree;
} hx_vector;

typedef struct {
	hx_storage_manager* storage;
	hx_vector* vector;
	hx_btree_iter* t;
} hx_vector_iter;

hx_vector* hx_new_vector ( hx_storage_manager* s );
int hx_free_vector ( hx_vector* list, hx_storage_manager* st );

int hx_vector_debug ( const char* header, const hx_vector* v, hx_storage_manager* st );
int hx_vector_add_terminal ( hx_vector* v, hx_storage_manager* st, const hx_node_id n, hx_terminal* t );
hx_terminal* hx_vector_get_terminal ( hx_vector* v, hx_storage_manager* st, hx_node_id n );
int hx_vector_remove_terminal ( hx_vector* v, hx_storage_manager* st, hx_node_id n );
list_size_t hx_vector_size ( hx_vector* v, hx_storage_manager* st );
hx_storage_id_t hx_vector_triples_count ( hx_vector* v, hx_storage_manager* st );
void hx_vector_triples_count_add ( hx_vector* v, hx_storage_manager* st, int c );

int hx_vector_write( hx_vector* t, hx_storage_manager* st, FILE* f );
hx_vector* hx_vector_read( hx_storage_manager* s, FILE* f, int buffer );

hx_vector_iter* hx_vector_new_iter ( hx_vector* vector, hx_storage_manager* st );
int hx_free_vector_iter ( hx_vector_iter* iter );
int hx_vector_iter_finished ( hx_vector_iter* iter );
int hx_vector_iter_current ( hx_vector_iter* iter, hx_node_id* n, hx_terminal** t );
int hx_vector_iter_next ( hx_vector_iter* iter );
int hx_vector_iter_seek( hx_vector_iter* iter, hx_node_id n );

#ifdef __cplusplus
}
#endif

#endif
