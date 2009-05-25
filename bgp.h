#ifndef _BGP_H
#define _BGP_H

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
#include "hexastore.h"
#include "node.h"

typedef struct {
	int size;
	int variables;
	char** variable_names;
	hx_triple** triples;
} hx_bgp;

hx_bgp* hx_new_bgp ( int size, hx_triple** triples );
hx_bgp* hx_new_bgp1 ( hx_triple* t1 );
hx_bgp* hx_new_bgp2 ( hx_triple* t1, hx_triple* t2 );
int hx_free_bgp ( hx_bgp* b );

int hx_bgp_size ( hx_bgp* b );
int hx_bgp_variables ( hx_bgp* b, hx_node*** vars );
hx_triple* hx_bgp_triple ( hx_bgp* b, int index );
int hx_bgp_reorder ( hx_bgp* , hx_hexastore* , hx_storage_manager* );
hx_variablebindings_iter* hx_bgp_execute ( hx_bgp*, hx_hexastore*, hx_storage_manager* );

int hx_bgp_debug ( hx_bgp* b );
int hx_bgp_string ( hx_bgp* b, char** string );
int hx_bgp_sse ( hx_bgp* b, char** string, char* indent, int level );

#ifdef __cplusplus
}
#endif

#endif
