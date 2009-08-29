#include "engine/variablebindings_iter.h"

hx_variablebindings_iter_sorting* hx_variablebindings_iter_new_node_sorting ( hx_variablebindings_iter_sort_order order, int sparql_order, hx_node* node ) {
	hx_expr* e	= hx_new_node_expr( node );
	hx_variablebindings_iter_sorting* s	= hx_variablebindings_iter_new_sorting( order, sparql_order, e );
	hx_free_expr(e);
	return s;
}

hx_variablebindings_iter_sorting* hx_variablebindings_iter_new_sorting ( hx_variablebindings_iter_sort_order order, int sparql_order, hx_expr* expr ) {
	hx_variablebindings_iter_sorting* s	= (hx_variablebindings_iter_sorting*) calloc( 1, sizeof(hx_variablebindings_iter_sorting) );
	s->order		= order;
	s->sparql_order	= sparql_order;
	s->expr			= hx_copy_expr( expr );
	return s;
}

hx_variablebindings_iter_sorting* hx_copy_variablebindings_iter_sorting ( hx_variablebindings_iter_sorting* s ) {
	hx_variablebindings_iter_sorting* c	= (hx_variablebindings_iter_sorting*) calloc( 1, sizeof(hx_variablebindings_iter_sorting) );
	c->order		= s->order;
	c->sparql_order	= s->sparql_order;
	c->expr			= hx_copy_expr( s->expr );
	return c;
}

int hx_free_variablebindings_iter_sorting ( hx_variablebindings_iter_sorting* sorting ) {
	hx_free_expr( sorting->expr );
	free( sorting );
}

int hx_variablebindings_iter_sorting_string ( hx_variablebindings_iter_sorting* sorting, char** string ) {
	char* expr_string;
	hx_expr_sse( sorting->expr, &expr_string, "", 0 );
	int len	= 7 + strlen(expr_string);
	*string	= (char*) calloc( len, sizeof(char) );
	snprintf( *string, len, "%s(%s)", (sorting->order == HX_VARIABLEBINDINGS_ITER_SORT_ASCENDING ? "ASC" : "DESC"), expr_string );
	free(expr_string);
	return 0;
}

hx_variablebindings_iter* hx_variablebindings_new_empty_iter ( void ) {
	hx_variablebindings_iter* iter	= (hx_variablebindings_iter*) malloc( sizeof( hx_variablebindings_iter ) );
	if (iter == NULL) {
		fprintf( stderr, "*** malloc failed in hx_variablebindings_new_empty_iter\n" );
	}
	iter->vtable		= NULL;
	iter->ptr			= NULL;
	return iter;
}

hx_variablebindings_iter* hx_variablebindings_new_empty_iter_with_names ( int size, char** names ) {
	return hx_new_materialize_iter_with_data( size, names, 0, NULL );
}

hx_variablebindings_iter* hx_variablebindings_new_iter ( hx_variablebindings_iter_vtable* vtable, void* ptr ) {
	hx_variablebindings_iter* iter	= (hx_variablebindings_iter*) malloc( sizeof( hx_variablebindings_iter ) );
	if (iter == NULL) {
		fprintf( stderr, "*** malloc failed in hx_variablebindings_new_iter\n" );
	}
	iter->vtable		= vtable;
	iter->ptr			= ptr;
	return iter;
}

int hx_free_variablebindings_iter ( hx_variablebindings_iter* iter ) {
	if (iter->vtable != NULL) {
		iter->vtable->free( iter->ptr );
		free( iter->vtable );
		iter->vtable	= NULL;
		iter->ptr		= NULL;
	}
	free( iter );
	return 0;
}

int hx_variablebindings_iter_finished ( hx_variablebindings_iter* iter ) {
	if (iter->vtable != NULL) {
		return iter->vtable->finished( iter->ptr );
	} else {
		return 1;
	}
}

int hx_variablebindings_iter_current ( hx_variablebindings_iter* iter, hx_variablebindings** b ) {
	if (iter->vtable != NULL) {
		return iter->vtable->current( iter->ptr, b );
	} else {
		return 1;
	}
}

int hx_variablebindings_iter_next ( hx_variablebindings_iter* iter ) {
	if (iter->vtable != NULL) {
		return iter->vtable->next( iter->ptr );
	} else {
		return 1;
	}
}

int hx_variablebindings_iter_size ( hx_variablebindings_iter* iter ) {
	if (iter->vtable != NULL) {
		return iter->vtable->size( iter->ptr );
	} else {
		return -1;
	}
}

char** hx_variablebindings_iter_names ( hx_variablebindings_iter* iter ) {
	if (iter->vtable != NULL) {
		return iter->vtable->names( iter->ptr );
	} else {
		return NULL;
	}
}

int hx_variablebindings_column_index ( hx_variablebindings_iter* iter, char* column ) {
	int size		= hx_variablebindings_iter_size( iter );
	char** names	= hx_variablebindings_iter_names( iter );
	int i;
	for (i = 0; i < size; i++) {
		if (strcmp(column, names[i]) == 0) {
			return i;
		}
	}
	return -1;
}

int hx_variablebindings_iter_is_sorted_by_index ( hx_variablebindings_iter* iter, int index ) {
	if (iter->vtable != NULL) {
		return iter->vtable->sorted_by_index( iter->ptr, index );
	} else {
		return 0;
	}
}

int hx_variablebindings_iter_debug ( hx_variablebindings_iter* iter, char* header, int indent ) {
	if (iter->vtable != NULL) {
		return iter->vtable->debug( iter->ptr, header, indent );
	} else {
		return 1;
	}
}

hx_variablebindings_iter* hx_variablebindings_sort_iter( hx_variablebindings_iter* iter, int index ) {
	int size		= hx_variablebindings_iter_size( iter );
	char** names	= hx_variablebindings_iter_names( iter );
// 	fprintf( stderr, "requested sorting of iterator on '%s'\n", names[index] );
	
	if (hx_variablebindings_iter_is_sorted_by_index(iter, index)) {
		return iter;
	} else {
		// iterator isn't sorted on the requested column...
		
		// so, materialize the iterator
		hx_variablebindings_iter* sorted	= hx_new_materialize_iter( iter );
		if (sorted == NULL) {
			hx_free_variablebindings_iter( iter );
			return NULL;
		}
		
// 		hx_materialize_iter_debug( iter );
		
		// and sort the materialized bindings by the requested column...
		int r	= hx_materialize_sort_iter_by_column( sorted, index );
		if (r == 0) {
			return sorted;
		} else {
			hx_free_variablebindings_iter( sorted );
			return NULL;
		}
	}
}
