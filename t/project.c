#include <unistd.h>
#include "hexastore.h"
#include "nodemap.h"
#include "node.h"
#include "storage.h"
#include "project.h"
#include "tap.h"

void _add_data ( hx_hexastore* hx, hx_storage_manager* s );
void _debug_node ( char* h, hx_node* node );
hx_variablebindings_iter* _get_triples ( hx_hexastore* hx, hx_storage_manager* s, int sort );

hx_node* p1;
hx_node* p2;
hx_node* r1;
hx_node* r2;
hx_node* l1;
hx_node* l2;

void project_test1 ( void );

int main ( void ) {
	plan_tests(13);
	p1	= hx_new_node_resource( "p1" );
	p2	= hx_new_node_resource( "p2" );
	r1	= hx_new_node_resource( "r1" );
	r2	= hx_new_node_resource( "r2" );
	l1	= hx_new_node_literal( "l1" );
	l2	= hx_new_node_literal( "l2" );
	
	project_test1();
	
	return exit_status();
}

void project_test1 ( void ) {
	hx_storage_manager* s	= hx_new_memory_storage_manager();
	hx_hexastore* hx		= hx_new_hexastore( s );
	hx_nodemap* map			= hx_get_nodemap( hx );
	_add_data( hx, s );
// <r1> :p1 <r2>
// <r2> :p1 <r1>
// <r2> :p2 "l2"
// <r1> :p2 "l1"
	
	{
		char* name;
		char* string;
		hx_node_id nid;
		hx_variablebindings* b;
		char* pnames[]	= { "pred" };
		// get ?subj ?pred ?obj ordered by object
		hx_variablebindings_iter* iter	= hx_new_project_iter(
											_get_triples( hx, s, HX_OBJECT ),
											1,
											pnames
										);
		{
			int size	= hx_variablebindings_iter_size( iter );
			ok1( size == 1 );
		}
		
		ok1( !hx_variablebindings_iter_finished( iter ) );
		
		hx_variablebindings_iter_current( iter, &b );
		if (0) {
			// XXX
			hx_variablebindings_string( b, map, &string );
			fprintf( stdout, "[1] bindings: %s\n", string );
			free( string );
		}
		{
			// expect 1 variable binding for the projected result
			int size	= hx_variablebindings_size( b );
			ok1( size == 1 );
		}
		
		{
			// expect the first variable binding to be "subj"
			name	= hx_variablebindings_name_for_binding( b, 0 );
			ok1( strcmp( name, "pred" ) == 0);
		}
	
	
		{
			hx_node_id nid	= hx_variablebindings_node_id_for_binding( b, 0 );
			hx_node* node	= hx_nodemap_get_node( map, nid );
	//		_debug_node( "[1] node: ", node );
			
			// expect the first result has "pred" of p1
			ok1( hx_node_cmp( node, p2 ) != 0 );
			ok1( hx_node_cmp( node, p1 ) == 0 );
		}
	
		hx_variablebindings_iter_next( iter );
		if (0) {
			// XXX
			hx_variablebindings_string( b, map, &string );
			fprintf( stdout, "[2] bindings: %s\n", string );
			free( string );
		}
		{
			// expect that the iterator isn't finished
			ok1( !hx_variablebindings_iter_finished( iter ) );
			hx_node* node	= hx_variablebindings_node_for_binding( b, map, 0 );
//			_debug_node( "[2] node: ", node );
			
			// expect the second result has "obj" of r2
			ok1( hx_node_cmp( node, p1 ) == 0 );
		}
		
		hx_variablebindings_iter_next( iter );
		if (0) {
			// XXX
			hx_variablebindings_string( b, map, &string );
			fprintf( stdout, "[3] bindings: %s\n", string );
			free( string );
		}
		{
			// expect that the iterator isn't finished
			ok1( !hx_variablebindings_iter_finished( iter ) );
			
			hx_variablebindings_iter_current( iter, &b );
			hx_node* node	= hx_variablebindings_node_for_binding( b, map, 0 );
//			_debug_node( "[3] node: ", node );
			
			// expect the second result has "obj" of l2
			ok1( hx_node_cmp( node, p2 ) == 0 );
		}
		
		hx_variablebindings_iter_next( iter );
		if (0) {
			// XXX
			hx_variablebindings_string( b, map, &string );
			fprintf( stdout, "[4] bindings: %s\n", string );
			free( string );
		}
		{
			// expect that the iterator isn't finished
			ok1( !hx_variablebindings_iter_finished( iter ) );
			hx_node* node	= hx_variablebindings_node_for_binding( b, map, 0 );
//			_debug_node( "[4] node: ", node );
			
			// expect the second result has "obj" of r2
			ok1( hx_node_cmp( node, p2 ) == 0 );
		}
	
		hx_variablebindings_iter_next( iter );
		ok1( hx_variablebindings_iter_finished( iter ) );
	
// 	while (!hx_variablebindings_iter_finished( iter )) {
// 		hx_variablebindings* b;
// 		hx_node_id s, p, o;
// 		hx_variablebindings_iter_current( iter, &b );
// 		char* string;
// 		hx_variablebindings_string( b, map, &string );
// 		fprintf( stdout, "%s\n", string );
// 		free( string );
// 		
// 		hx_free_variablebindings( b, 0 );
// 		hx_variablebindings_iter_next( iter );
// 	}
	
		hx_free_variablebindings_iter( iter, 1 );
	}
	
	hx_free_hexastore( hx, s );
	hx_free_storage_manager( s );
}

hx_variablebindings_iter* _get_triples ( hx_hexastore* hx, hx_storage_manager* s, int sort ) {
	hx_node* v1	= hx_new_node_variable( -1 );
	hx_node* v2	= hx_new_node_variable( -2 );
	hx_node* v3	= hx_new_node_variable( -3 );
	
	hx_index_iter* titer	= hx_get_statements( hx, s, v1, v2, v3, HX_OBJECT );
	hx_variablebindings_iter* iter	= hx_new_iter_variablebindings( titer, s, "subj", "pred", "obj", 0 );
	return iter;
}

void _add_data ( hx_hexastore* hx, hx_storage_manager* s ) {
	hx_add_triple( hx, s, r1, p1, r2 );
	hx_add_triple( hx, s, r2, p1, r1 );
	hx_add_triple( hx, s, r2, p2, l2 );
	hx_add_triple( hx, s, r1, p2, l1 );
}

void _debug_node ( char* h, hx_node* node ) {
	char* string;
	hx_node_string( node, &string );
	fprintf( stderr, "%s %s\n", h, string );
}
