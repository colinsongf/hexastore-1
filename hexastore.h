#ifndef _HEXASTORE_H
#define _HEXASTORE_H

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

#ifdef HAVE_LIBPTHREAD
#include <pthread.h>
#endif

#include "hexastore_types.h"
#include "rdf/triple.h"
#include "algebra/variablebindings.h"
#include "misc/nodemap.h"
#include "misc/util.h"
#include "engine/variablebindings_iter.h"
#include "store/hexastore/head.h"
#include "store/hexastore/terminal.h"
#include "store/hexastore/vector.h"
#include "store/store.h"

typedef enum {
	RDF_ITER_FLAGS_BOUND_A	= 1,
	RDF_ITER_FLAGS_BOUND_B	= 2,
	RDF_ITER_FLAGS_BOUND_C	= 4
} hx_iter_flag_t;

#define THREADED_BATCH_SIZE	5000

typedef struct {
	hx_store* store;
	int next_var;
	hx_container_t* indexes;
} hx_hexastore;

typedef struct {
	void* world;
	hx_hexastore* hx;
	int64_t nestedloopjoin_penalty;
	int64_t hashjoin_penalty;
	int64_t unsorted_mergejoin_penalty;
} hx_execution_context;

hx_execution_context* hx_new_execution_context ( void* world, hx_hexastore* hx );
int hx_free_execution_context ( hx_execution_context* c );

hx_hexastore* hx_new_hexastore ( void* world );
hx_hexastore* hx_new_hexastore_with_store ( void* world, hx_store* store );
int hx_free_hexastore ( hx_hexastore* hx );

int hx_add_triple( hx_hexastore* hx, hx_node* s, hx_node* p, hx_node* o );

int hx_remove_triple( hx_hexastore* hx, hx_node* s, hx_node* p, hx_node* o );
int hx_debug ( hx_hexastore* hx );

uint64_t hx_triples_count( hx_hexastore* hx );
uint64_t hx_count_statements( hx_hexastore* hx, hx_node* s, hx_node* p, hx_node* o );

hx_node* hx_new_variable ( hx_hexastore* hx );
hx_node* hx_new_named_variable ( hx_hexastore* hx, char* name );
hx_nodemap* hx_get_nodemap ( hx_hexastore* hx );
hx_container_t* hx_get_indexes ( hx_hexastore* hx );

hx_variablebindings_iter* hx_new_variablebindings_iter_for_triple ( hx_hexastore* hx, hx_triple* t, hx_node_position_t sort_position );

#ifdef __cplusplus
}
#endif

#endif
