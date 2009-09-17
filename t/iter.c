#include <unistd.h>
#include "mentok.h"
#include "misc/nodemap.h"
#include "rdf/node.h"
#include "store/hexastore/hexastore.h"
#include "test/tap.h"

void _add_data ( hx_model* hx );
void _debug_node ( char* h, hx_node* node );
hx_variablebindings_iter* _get_triples ( hx_model* hx, int sort );

hx_node* p1;
hx_node* p2;
hx_node* r1;
hx_node* r2;
hx_node* l1;
hx_node* l2;

void test_small_iter ( void );

int main ( void ) {
	plan_tests(10);
	p1	= hx_new_node_resource( "p1" );
	p2	= hx_new_node_resource( "p2" );
	r1	= hx_new_node_resource( "r1" );
	r2	= hx_new_node_resource( "r2" );
	l1	= hx_new_node_literal( "l1" );
	l2	= hx_new_node_literal( "l2" );
	
	test_small_iter();
	
	return exit_status();
}

void test_small_iter ( void ) {
	hx_model* hx	= hx_new_model( NULL );
	hx_nodemap* map		= hx_store_hexastore_get_nodemap( hx->store );
	_add_data( hx );
// <r1> :p1 <r2>
// <r2> :p1 <r1>
// <r2> :p2 "l2"
// <r1> :p2 "l1"
	
	int size;
	char* name;
	char* string;
	hx_variablebindings* b;
	
	// get ?subj ?pred ?obj ordered by object
	hx_variablebindings_iter* iter	= _get_triples( hx, HX_OBJECT );
	ok1( !hx_variablebindings_iter_finished( iter ) );
	hx_variablebindings_iter_current( iter, &b );
	
	// expect 3 variable bindings for the three triple nodes
	size	= hx_variablebindings_size( b );
	ok1( size == 3 );
	
//	hx_nodemap_debug( map );
	
	{
		// expect the first variable binding to be "subj"
		name	= hx_variablebindings_name_for_binding( b, 0 );
		hx_store_variablebindings_string( hx->store, b, &string );
//		fprintf( stdout, "[1] bindings: %s\n", string );
		free( string );
		ok1( strcmp( name, "subj" ) == 0);
	}
	{
		// expect the third variable binding to be "obj"
		name	= hx_variablebindings_name_for_binding( b, 2 );
		ok1( strcmp( name, "obj" ) == 0);
	}
	
	
	{
		hx_node_id nid	= hx_variablebindings_node_id_for_binding( b, 2 );
		hx_node* node	= hx_nodemap_get_node( map, nid );
//		_debug_node( "[1] node: ", node );
		
		// expect the first result has "obj" of r1
		ok1( hx_node_cmp( node, r2 ) != 0 );
		ok1( hx_node_cmp( node, r1 ) == 0 );
	}
	
	hx_variablebindings_iter_next( iter );
	{
		// expect that the iterator isn't finished
		ok1( !hx_variablebindings_iter_finished( iter ) );
		
		hx_variablebindings_iter_current( iter, &b );
		hx_store_variablebindings_string( hx->store, b, &string );
//		fprintf( stdout, "[2] bindings: %s\n", string );
		free( string );

		hx_node_id nid	= hx_variablebindings_node_id_for_binding( b, 2 );
		hx_node* node	= hx_nodemap_get_node( map, nid );
//		_debug_node( "[2] node: ", node );
		
		// expect the second result has "obj" of r2
		ok1( hx_node_cmp( node, r2 ) == 0 );
	}
	
	hx_variablebindings_iter_next( iter );
	{
		// expect that the iterator isn't finished
		ok1( !hx_variablebindings_iter_finished( iter ) );
		
		hx_variablebindings_iter_current( iter, &b );
		hx_store_variablebindings_string( hx->store, b, &string );
//		fprintf( stdout, "[3] bindings: %s\n", string );
		free( string );

		hx_node_id nid	= hx_variablebindings_node_id_for_binding( b, 2 );
		hx_node* node	= hx_nodemap_get_node( map, nid );
//		_debug_node( "[3] node: ", node );
		
		// expect the second result has "obj" of l2
		ok1( hx_node_cmp( node, l2 ) == 0 );
	}
	
	
	hx_free_variablebindings_iter( iter );
	hx_free_model( hx );
}

hx_variablebindings_iter* _get_triples ( hx_model* hx, int sort ) {
	hx_node* v1	= hx_new_node_named_variable( -1, "subj" );
	hx_node* v2	= hx_new_node_named_variable( -2, "pred" );
	hx_node* v3	= hx_new_node_named_variable( -3, "obj" );
	hx_triple* t	= hx_new_triple( v1, v2, v3 );
	hx_variablebindings_iter* iter	=  hx_model_new_variablebindings_iter_for_triple( hx, t, HX_OBJECT );
	hx_free_triple(t);
	hx_free_node(v1);
	hx_free_node(v2);
	hx_free_node(v3);
	return iter;
}

void _add_data ( hx_model* hx ) {
	hx_model_add_triple( hx, r1, p1, r2 );
	hx_model_add_triple( hx, r2, p1, r1 );
	hx_model_add_triple( hx, r2, p2, l2 );
	hx_model_add_triple( hx, r1, p2, l1 );
}

void _debug_node ( char* h, hx_node* node ) {
	char* string;
	hx_node_string( node, &string );
	fprintf( stderr, "%s %s\n", h, string );
}

