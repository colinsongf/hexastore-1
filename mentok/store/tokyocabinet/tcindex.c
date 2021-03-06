#include "tcindex.h"

int _hx_store_tokyocabinet_check_next_triple (hx_store_tokyocabinet_index_iter* iter);
int _hx_store_tokyocabinet_index_iter_vb_finished ( void* iter );
int _hx_store_tokyocabinet_index_iter_vb_current ( void* iter, void* results );
int _hx_store_tokyocabinet_index_iter_vb_next ( void* iter );	
int _hx_store_tokyocabinet_index_iter_vb_free ( void* iter );
int _hx_store_tokyocabinet_index_iter_vb_size ( void* iter );
int _hx_store_tokyocabinet_index_iter_vb_sorted_by (void* iter, int index );
char** _hx_store_tokyocabinet_index_iter_vb_names ( void* iter );
int _hx_store_tokyocabinet_index_iter_debug ( void* info, char* header, int indent );

/**

	Arguments tokyocabinet_index_order = {a,b,c} map the positions of a triple to the
	ordering of the tokyocabinet_index.
	
	(a,b,c) = (0,1,2)	==> (s,p,o) tokyocabinet_index
	(a,b,c) = (1,0,2)	==> (p,s,o) tokyocabinet_index
	(a,b,c) = (2,1,0)	==> (o,p,s) tokyocabinet_index

**/

hx_store_tokyocabinet_index* hx_new_tokyocabinet_index ( void* world, int* index_order, const char* directory, const char* filename ) {
//	fprintf( stderr, "hx_index is %d bytes in size\n", (int) sizeof( hx_index ) );
	hx_store_tokyocabinet_index* i	= (hx_store_tokyocabinet_index*) calloc( 1, sizeof( hx_store_tokyocabinet_index ) );
	int j;
	for (j = 0; j < 3; j++) {
		i->order[j]	= index_order[j];
	}
	i->bdb = tcbdbnew();
	tcbdbtune(i->bdb, 0, 0, 0, -1, -1, BDBTLARGE|BDBTDEFLATE);
//	tcbdbtune(i->bdb, 0, 0, 0, -1, -1, BDBTLARGE|BDBTBZIP);
	tcbdbsetcache(i->bdb, 8192, 2048);
	
	char* file	= malloc( strlen(directory) + strlen(filename) + 2 );
	sprintf( file, "%s/%s", directory, filename );
	
	if(!tcbdbopen(i->bdb, file, BDBOWRITER | BDBOCREAT)){
		int ecode;
		ecode = tcbdbecode(i->bdb);
		fprintf(stderr, "tokyocabinet open error on %s: %s\n", filename, tcbdberrmsg(ecode));
		free( i );
		free(file);
		return NULL;
	}
	
	free(file);
	return i;
}

int hx_free_tokyocabinet_index ( hx_store_tokyocabinet_index* i ) {
	tcbdbdel(i->bdb);
	free( i );
	return 0;
}

int hx_store_tokyocabinet_index_debug ( hx_store_tokyocabinet_index* index ) {
	BDBCUR *cur	= tcbdbcurnew(index->bdb);
	tcbdbcurfirst(cur);

	char* name	= hx_store_tokyocabinet_index_name( index );
	fprintf( stderr, "Index (%s):\n", name );
	free(name);
	do {
		int size;
		hx_node_id* k	= (hx_node_id*) tcbdbcurkey(cur, &size);
		fprintf( stderr, "\t{ %"PRIdHXID", %"PRIdHXID", %"PRIdHXID" }\n", k[0], k[1], k[2] );
		free(k);
	} while (tcbdbcurnext(cur));
	
	tcbdbcurdel(cur);
	
	return 0;
}

uint64_t hx_store_tokyocabinet_index_count( hx_store_tokyocabinet_index* index, hx_node_id* index_ordered ) {
	BDBCUR *cur	= tcbdbcurnew(index->bdb);
	tcbdbcurjump(cur, index_ordered, 3*sizeof(hx_node_id));
	
	uint64_t counter	= 0;
	fprintf( stderr, "Index:\n" );
	do {
		int size;
		hx_node_id* k	= (hx_node_id*) tcbdbcurkey(cur, &size);
		fprintf( stderr, "\t{ %"PRIdHXID", %"PRIdHXID", %"PRIdHXID" }\n", k[0], k[1], k[2] );
		
		int match	= 1;
		int i;
		for (i = 0; i < 3; i++) {
			if (index_ordered[i] != 0) {
				if (k[i] != index_ordered[i]) {
					match	= 0;
					break;
				}
			}
		}
		if (!match) {
			break;
		}
		
		counter++;
		free(k);
	} while (tcbdbcurnext(cur));
	
	tcbdbcurdel(cur);
	
	return counter;
}

int hx_store_tokyocabinet_index_add_triple ( hx_store_tokyocabinet_index* index, hx_node_id s, hx_node_id p, hx_node_id o ) {
	hx_node_id triple_ordered[3];
	
	triple_ordered[0]	= s;
	triple_ordered[1]	= p;
	triple_ordered[2]	= o;
	hx_node_id index_ordered[3];
	for (int i = 0; i < 3; i++) {
		index_ordered[ i ]	= triple_ordered[ index->order[ i ] ];
	}
	
	int exists	= tcbdbvnum(index->bdb, index_ordered, 3*sizeof(hx_node_id));
	if (exists > 0) {
		return 1;
	} else {
		char v	= (char) 0;
		if (!tcbdbput(index->bdb, index_ordered, 3*sizeof(hx_node_id), &v, 1)) {
			int ecode	= tcbdbecode(index->bdb);
			fprintf( stderr, "tokyocabinet put error: %s\n", tcbdberrmsg(ecode));
			return 1;
		}
		
	//	fprintf( stderr, "add_triple index order: { %d, %d, %d }\n", (int) index_ordered[0], (int) index_ordered[1], (int) index_ordered[2] );
		return 0;
	}
}

int hx_store_tokyocabinet_index_remove_triple ( hx_store_tokyocabinet_index* index, hx_node_id s, hx_node_id p, hx_node_id o ) {
	hx_node_id triple_ordered[3];
	triple_ordered[0]	= s;
	triple_ordered[1]	= p;
	triple_ordered[2]	= o;
	hx_node_id index_ordered[3];
	for (int i = 0; i < 3; i++) {
		index_ordered[ i ]	= triple_ordered[ index->order[ i ] ];
	}
	
	if (tcbdbout(index->bdb, index_ordered, 3 * sizeof(hx_node_id))) {
		return 0;
	} else {
		return 1;
	}
}

uint64_t hx_store_tokyocabinet_index_triples_count ( hx_store_tokyocabinet_index* index ) {
	return (uint64_t) tcbdbrnum( index->bdb );
}

char* hx_store_tokyocabinet_index_name ( hx_store_tokyocabinet_index* idx ) {
	int size	= 3;
	int* order	= idx->order;
	char* name		= (char*) calloc( 5, sizeof( char ) );
	char* p			= name;
	int i;
	for (i = 0; i < size; i++) {
		int o	= order[i];
		switch (o) {
			case HX_SUBJECT:
				*(p++)	= 'S';
				break;
			case HX_PREDICATE:
				*(p++)	= 'P';
				break;
			case HX_OBJECT:
				*(p++)	= 'O';
				break;
			default:
				fprintf( stderr, "Unrecognized index order (%d) in _hx_optimizer_index_name\n", o );
		};
	}
	return name;
}

hx_store_tokyocabinet_index_iter* hx_store_tokyocabinet_index_new_iter ( hx_store_tokyocabinet_index* index ) {
	hx_store_tokyocabinet_index_iter* iter	= (hx_store_tokyocabinet_index_iter*) calloc( 1, sizeof( hx_store_tokyocabinet_index_iter ) );
	iter->flags			= 0;
	iter->finished		= 0;
	iter->node_mask_a	= (hx_node_id) -1;
	iter->node_mask_b	= (hx_node_id) -2;
	iter->node_mask_c	= (hx_node_id) -3;
	iter->index			= index;
	iter->cursor		= tcbdbcurnew( index->bdb );
	if (!tcbdbcurfirst( iter->cursor )) {
		iter->finished	= 1;
	}
	return iter;
}

hx_store_tokyocabinet_index_iter* hx_store_tokyocabinet_index_new_iter1 ( hx_store_tokyocabinet_index* index, hx_node_id s, hx_node_id p, hx_node_id o ) {
	hx_store_tokyocabinet_index_iter* iter	= hx_store_tokyocabinet_index_new_iter( index );
	hx_node_id masks[3]	= { s, p, o };
	iter->node_mask_a	= masks[ index->order[0] ];
	iter->node_mask_b	= masks[ index->order[1] ];
	iter->node_mask_c	= masks[ index->order[2] ];
	iter->node_dup_b	= 0;
	iter->node_dup_c	= 0;
	
// 	fprintf( stderr, "*** index using node masks (in index-order): %d %d %d\n", (int) iter->node_mask_a, (int) iter->node_mask_b, (int) iter->node_mask_c );
	
	if (iter->node_mask_b == iter->node_mask_a && iter->node_mask_a != (hx_node_id) 0) {
// 		fprintf( stderr, "*** Looking for duplicated subj/pred triples\n" );
		iter->node_dup_b	= HX_STORE_TCINDEX_ITER_DUP_A;
	}
	
	if (iter->node_mask_c == iter->node_mask_a && iter->node_mask_a != (hx_node_id) 0) {
// 		fprintf( stderr, "*** Looking for duplicated subj/obj triples\n" );
		iter->node_dup_c	= HX_STORE_TCINDEX_ITER_DUP_A;
	} else if (iter->node_mask_c == iter->node_mask_b && iter->node_mask_b != (hx_node_id) 0) {
// 		fprintf( stderr, "*** Looking for duplicated pred/obj triples\n" );
		iter->node_dup_c	= HX_STORE_TCINDEX_ITER_DUP_B;
	}
	
	iter->cursor		= tcbdbcurnew( index->bdb );
	hx_node_id index_ordered[3];
	for (int i = 0; i < 3; i++) {
		hx_node_id id	= masks[ index->order[i] ];
		if (id > 0) {
			index_ordered[i]	= id;
		} else {
			index_ordered[i]	= 0;
		}
	}
	tcbdbcurjump( iter->cursor, index_ordered, 3*sizeof( hx_node_id ) );
	return iter;
}

int hx_free_tokyocabinet_index_iter ( hx_store_tokyocabinet_index_iter* iter ) {
	tcbdbcurdel( iter->cursor );
	free( iter );
	return 0;
}

int hx_store_tokyocabinet_index_iter_finished ( hx_store_tokyocabinet_index_iter* iter ) {
	int size;
	hx_node_id* current	= tcbdbcurkey(iter->cursor, &size);
	if (current == NULL) {
		iter->finished	= 1;
	} else {
		int i;
		int matches	= 1;
		if (iter->node_mask_a > 0 && iter->node_mask_a != current[0] ) iter->finished	= 1;
		if (iter->node_mask_b > 0 && iter->node_mask_b != current[1] ) iter->finished	= 1;
		if (iter->node_mask_c > 0 && iter->node_mask_c != current[2] ) iter->finished	= 1;
	}
	return iter->finished;
}

int hx_store_tokyocabinet_index_iter_current ( hx_store_tokyocabinet_index_iter* iter, hx_node_id* s, hx_node_id* p, hx_node_id* o ) {
	int size;
	hx_node_id* current	= tcbdbcurkey(iter->cursor, &size);
	if (current == NULL) {
		iter->finished	= 1;
		return 1;
	}
	
	_hx_store_tokyocabinet_check_next_triple( iter );
	
	hx_node_id triple_ordered[3];
	hx_store_tokyocabinet_index* index	= iter->index;
	for (int i = 0; i < 3; i++) {
		triple_ordered[ index->order[i] ]	= current[i];
	}
	
	*s	= triple_ordered[0];
	*p	= triple_ordered[1];
	*o	= triple_ordered[2];
	return 0;
}

int _hx_store_tokyocabinet_check_next_triple (hx_store_tokyocabinet_index_iter* iter) {
	int size;
	hx_node_id* current	= tcbdbcurkey(iter->cursor, &size);
	if (iter->node_mask_a > 0 && current[0] != iter->node_mask_a) {
		iter->finished	= 1;
		return 1;
	}
	if (iter->node_mask_b > 0 && current[1] != iter->node_mask_b) {
		iter->finished	= 1;
		return 1;
	}
	if (iter->node_mask_c > 0 && current[3] != iter->node_mask_c) {
		iter->finished	= 1;
		return 1;
	}
	
	if (HX_STORE_TCINDEX_ITER_DUP_A == iter->node_dup_b && current[0] != current[1]) {
		iter->finished	= 1;
		return 1;
	}
	
	if (HX_STORE_TCINDEX_ITER_DUP_A == iter->node_dup_c && current[0] != current[2]) {
		iter->finished	= 1;
		return 1;
	}

	if (HX_STORE_TCINDEX_ITER_DUP_B == iter->node_dup_c && current[1] != current[2]) {
		iter->finished	= 1;
		return 1;
	}
	return 0;
}

int hx_store_tokyocabinet_index_iter_next ( hx_store_tokyocabinet_index_iter* iter ) {
	if (tcbdbcurnext( iter->cursor )) {
		_hx_store_tokyocabinet_check_next_triple( iter );
		return 0;
	} else {
		iter->finished	= 1;
		return 1;
	}
}


int hx_store_tokyocabinet_index_iter_is_sorted_by_index ( hx_store_tokyocabinet_index_iter* iter, int index ) {
	hx_node_id masks[3]	= { iter->node_mask_a, iter->node_mask_b, iter->node_mask_c };
// 	fprintf( stderr, ">>> %d\n", index );
// 	fprintf( stderr, "*** masks: { %d, %d, %d }\n", (int) masks[0], (int) masks[1], (int) masks[2] );
// 	fprintf( stderr, "*** order: { %d, %d, %d }\n", iter->index->order[0], iter->index->order[1], iter->index->order[2] );
	int* order	= iter->index->order;
	if (index == order[0]) {
		return 1;
	} else if (index == order[1]) {
		return (masks[0] > 0);
	} else if (index == order[2]) {
		return (masks[0] > 0 && masks[1] > 0);
	} else {
		fprintf( stderr, "*** not a valid triple position index in call to hx_store_tokyocabinet_index_iter_is_sorted_by_index\n" );
		return -1;
	}
}

hx_variablebindings_iter* hx_new_tokyocabinet_index_iter_variablebindings ( hx_store_tokyocabinet_index_iter* i, char* subj_name, char* pred_name, char* obj_name ) {
	hx_variablebindings_iter_vtable* vtable	= (hx_variablebindings_iter_vtable*) calloc( 1, sizeof( hx_variablebindings_iter_vtable ) );
	vtable->finished		= _hx_store_tokyocabinet_index_iter_vb_finished;
	vtable->current			= _hx_store_tokyocabinet_index_iter_vb_current;
	vtable->next			= _hx_store_tokyocabinet_index_iter_vb_next;
	vtable->free			= _hx_store_tokyocabinet_index_iter_vb_free;
	vtable->names			= _hx_store_tokyocabinet_index_iter_vb_names;
	vtable->size			= _hx_store_tokyocabinet_index_iter_vb_size;
	vtable->sorted_by_index	= _hx_store_tokyocabinet_index_iter_vb_sorted_by;
	vtable->debug			= _hx_store_tokyocabinet_index_iter_debug;
	
	int size	= 0;
	if (subj_name != NULL)
		size++;
	if (pred_name != NULL)
		size++;
	if (obj_name != NULL)
		size++;
	
	_hx_store_tokyocabinet_index_iter_vb_info* info	= (_hx_store_tokyocabinet_index_iter_vb_info*) calloc( 1, sizeof( _hx_store_tokyocabinet_index_iter_vb_info ) );
	info->size						= size;
	info->subject					= hx_copy_string( subj_name );
	info->predicate					= hx_copy_string( pred_name );
	info->object					= hx_copy_string( obj_name );
	info->iter						= i;
	info->names						= (char**) calloc( 3, sizeof( char* ) );
	info->triple_pos_to_index		= (int*) calloc( 3, sizeof( int ) );
	info->index_to_triple_pos		= (int*) calloc( 3, sizeof( int ) );
	info->current					= NULL;
	
	int j	= 0;
	if (subj_name != NULL) {
		int idx	= j++;
		info->names[ idx ]		= subj_name;
		info->triple_pos_to_index[ idx ]	= 0;
		info->index_to_triple_pos[ 0 ]		= idx;
	}
	
	if (pred_name != NULL) {
		int idx	= j++;
		info->names[ idx ]		= pred_name;
		info->triple_pos_to_index[ idx ]	= 1;
		info->index_to_triple_pos[ 1 ]		= idx;
	}
	if (obj_name != NULL) {
		int idx	= j++;
		info->names[ idx ]		= obj_name;
		info->triple_pos_to_index[ idx ]	= 2;
		info->index_to_triple_pos[ 2 ]		= idx;
	}
	
	hx_variablebindings_iter* iter	= hx_variablebindings_new_iter( vtable, (void*) info );
	return iter;
}

int _hx_store_tokyocabinet_index_iter_vb_finished ( void* data ) {
	_hx_store_tokyocabinet_index_iter_vb_info* info	= (_hx_store_tokyocabinet_index_iter_vb_info*) data;
	hx_store_tokyocabinet_index_iter* iter	= (hx_store_tokyocabinet_index_iter*) info->iter;
	return hx_store_tokyocabinet_index_iter_finished( iter );
}

int _hx_store_tokyocabinet_index_iter_vb_current ( void* data, void* results ) {
	_hx_store_tokyocabinet_index_iter_vb_info* info	= (_hx_store_tokyocabinet_index_iter_vb_info*) data;
	hx_variablebindings** bindings	= (hx_variablebindings**) results;
	if (info->current == NULL) {
		hx_store_tokyocabinet_index_iter* iter		= (hx_store_tokyocabinet_index_iter*) info->iter;
		hx_node_id triple[3];
		hx_store_tokyocabinet_index_iter_current ( iter, &(triple[0]), &(triple[1]), &(triple[2]) );
		hx_node_id* values	= (hx_node_id*) calloc( info->size, sizeof( hx_node_id ) );
		for (int i = 0; i < info->size; i++) {
			values[ i ]	= triple[ info->triple_pos_to_index[ i ] ];
		}
		info->current	= hx_model_new_variablebindings( info->size, info->names, values );
	}
	*bindings	= info->current;
	return 0;
}

int _hx_store_tokyocabinet_index_iter_vb_next ( void* data ) {
	_hx_store_tokyocabinet_index_iter_vb_info* info	= (_hx_store_tokyocabinet_index_iter_vb_info*) data;
	hx_store_tokyocabinet_index_iter* iter		= (hx_store_tokyocabinet_index_iter*) info->iter;
	info->current			= NULL;
	return hx_store_tokyocabinet_index_iter_next( iter );
}

int _hx_store_tokyocabinet_index_iter_vb_free ( void* data ) {
	_hx_store_tokyocabinet_index_iter_vb_info* info	= (_hx_store_tokyocabinet_index_iter_vb_info*) data;
	hx_store_tokyocabinet_index_iter* iter		= (hx_store_tokyocabinet_index_iter*) info->iter;
	hx_free_tokyocabinet_index_iter( iter );
	free( info->names );
	free( info->triple_pos_to_index );
	free( info->index_to_triple_pos );
	free( info->subject );
	free( info->predicate );
	free( info->object );
	free( info );
	return 0;
}

int _hx_store_tokyocabinet_index_iter_vb_size ( void* data ) {
	_hx_store_tokyocabinet_index_iter_vb_info* info	= (_hx_store_tokyocabinet_index_iter_vb_info*) data;
	return info->size;
}

char** _hx_store_tokyocabinet_index_iter_vb_names ( void* data ) {
	_hx_store_tokyocabinet_index_iter_vb_info* info	= (_hx_store_tokyocabinet_index_iter_vb_info*) data;
	return info->names;
}

int _hx_store_tokyocabinet_index_iter_vb_sorted_by (void* data, int index ) {
	_hx_store_tokyocabinet_index_iter_vb_info* info	= (_hx_store_tokyocabinet_index_iter_vb_info*) data;
	int triple_pos	= info->index_to_triple_pos[ index ];
// 	fprintf( stderr, "*** checking if index iterator is sorted by %d (triple %s)\n", index, HX_POSITION_NAMES[triple_pos] );
	return hx_store_tokyocabinet_index_iter_is_sorted_by_index( info->iter, triple_pos );
}

int _hx_store_tokyocabinet_index_iter_debug ( void* data, char* header, int indent ) {
	_hx_store_tokyocabinet_index_iter_vb_info* info	= (_hx_store_tokyocabinet_index_iter_vb_info*) data;
	for (int i = 0; i < indent; i++) fwrite( " ", sizeof( char ), 1, stderr );
	fprintf( stderr, "%s hexastore triples iterator\n", header );
	return 0;
}
