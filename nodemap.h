#ifndef _NODEMAP_H
#define _NODEMAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <tcutil.h>
#include <tcbdb.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "hexastore_types.h"
#include "node.h"
#include "storage.h"

/* type is 'M' for memory, 'B' for B+-tree disk-based */
typedef struct {
	char type;
	hx_node_id next_id;
	void* id2node;
	void* node2id;
	char* directory;
} hx_nodemap;

typedef struct {
	hx_node_id id;
	hx_node* node;
} hx_nodemap_item;

hx_nodemap* hx_new_nodemap( void );
hx_nodemap* hx_new_file_nodemap( const char* directory );
int hx_remove_nodemap ( hx_nodemap* m );
int hx_free_nodemap ( hx_nodemap* m );

hx_node_id hx_nodemap_add_node ( hx_nodemap* m, hx_node* n );
int hx_nodemap_remove_node_id ( hx_nodemap* m, hx_node_id id );
int hx_nodemap_remove_node ( hx_nodemap* m, hx_node* n );
hx_node_id hx_nodemap_get_node_id ( hx_nodemap* m, hx_node* n );
hx_node* hx_nodemap_get_node ( hx_nodemap* m, hx_node_id id );
hx_nodemap* hx_nodemap_sparql_order_nodes ( hx_nodemap* map );
int hx_nodemap_debug ( hx_nodemap* map );

int hx_nodemap_write( hx_nodemap* t, FILE* f );
hx_nodemap* hx_nodemap_read( hx_storage_manager* s, FILE* f, int buffer );

#ifdef __cplusplus
}
#endif

#endif
