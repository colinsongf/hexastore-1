#ifndef _HASHJOIN_H
#define _HASHJOIN_H

#ifdef __cplusplus
extern "C" {
#endif

#define NODE_LIST_ALLOC_SIZE	10

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

#include "mentok/mentok_types.h"
#include "mentok/engine/variablebindings_iter.h"
#include "mentok/misc/util.h"

typedef struct {
	int size;
	char** names;
	hx_variablebindings_iter* lhs;
	hx_variablebindings_iter* rhs;
	int finished;
	int started;
	hx_variablebindings* current;
	hx_variablebindings* current_lhs;
	hx_hash_t* hash;
	int* rhs_shared_columns;
	int* lhs_shared_columns;
	int shared_columns;
	hx_container_t* rhs_matches;
	int rhs_matches_index;
	int leftjoin;
} _hx_hashjoin_iter_vb_info;

int _hx_hashjoin_iter_vb_finished ( void* iter );
int _hx_hashjoin_iter_vb_current ( void* iter, void* results );
int _hx_hashjoin_iter_vb_next ( void* iter );	
int _hx_hashjoin_iter_vb_free ( void* iter );
int _hx_hashjoin_iter_vb_size ( void* iter );
char** _hx_hashjoin_iter_vb_names ( void* iter );

hx_variablebindings_iter* hx_new_hashjoin_iter ( hx_variablebindings_iter* lhs, hx_variablebindings_iter* rhs );
hx_variablebindings_iter* hx_new_hashjoin_iter2 ( hx_variablebindings_iter* lhs, hx_variablebindings_iter* rhs, int leftjoin );

#ifdef __cplusplus
}
#endif

#endif
