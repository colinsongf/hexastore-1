// PREFIX : <http://www.lehigh.edu/~zhp2/2004/0401/univ-bench.owl#>
// SELECT DISTINCT * WHERE {
// 	?X rdf:type http://www.lehigh.edu/~zhp2/2004/0401/univ-bench.owl#UndergraduateStudent .
// 	?Y rdf:type http://www.lehigh.edu/~zhp2/2004/0401/univ-bench.owl#FullProfessor .
// 	?Z rdf:type http://www.lehigh.edu/~zhp2/2004/0401/univ-bench.owl#Course .
// 	?X http://www.lehigh.edu/~zhp2/2004/0401/univ-bench.owl#advisor ?Y .
// 	?Y http://www.lehigh.edu/~zhp2/2004/0401/univ-bench.owl#teacherOf ?Z .
// 	?X http://www.lehigh.edu/~zhp2/2004/0401/univ-bench.owl#takesCourse ?Z .
// }

#include <time.h>
#include <stdio.h>
#include "mentok/mentok.h"
#include "mentok/algebra/variablebindings.h"
#include "mentok/engine/mergejoin.h"
#include "mentok/rdf/node.h"
#include "mentok/algebra/bgp.h"
#include "mentok/engine/bgp.h"
#include "mentok/store/store.h"
#include "mentok/store/hexastore/hexastore.h"

void _fill_triple ( hx_triple* t, hx_node* s, hx_node* p, hx_node* o ) {
	t->subject		= s;
	t->predicate	= p;
	t->object		= o;
}

int main ( int argc, char** argv ) {
	const char* filename	= argv[1];
	FILE* f	= fopen( filename, "r" );
	if (f == NULL) {
		perror( "Failed to open hexastore file for reading: " );
		return 1;
	}
	
	hx_store* store			= hx_store_hexastore_read( NULL, f, 0 );
	hx_model* hx		= hx_new_model_with_store( NULL, store );
	fprintf( stderr, "Finished loading hexastore...\n" );
	
	hx_node* x			= hx_model_new_named_variable( hx, "x" );
	hx_node* y			= hx_model_new_named_variable( hx, "y" );
	hx_node* z			= hx_model_new_named_variable( hx, "z" );
	hx_node* type			= hx_new_node_resource("http://www.w3.org/1999/02/22-rdf-syntax-ns#type");
	hx_node* subOrgOf		= hx_new_node_resource("http://www.lehigh.edu/~zhp2/2004/0401/univ-bench.owl#subOrganizationOf");
	hx_node* univ			= hx_new_node_resource("http://www.University0.edu");
	hx_node* dept			= hx_new_node_resource("http://www.lehigh.edu/~zhp2/2004/0401/univ-bench.owl#Department");
	hx_node* memberOf		= hx_new_node_resource("http://www.lehigh.edu/~zhp2/2004/0401/univ-bench.owl#memberOf");
	hx_node* student		= hx_new_node_resource("http://www.lehigh.edu/~zhp2/2004/0401/univ-bench.owl#Student");
	hx_node* email			= hx_new_node_resource("http://www.lehigh.edu/~zhp2/2004/0401/univ-bench.owl#emailAddress");
	
	hx_triple* triples[5];
	{
		hx_triple t0, t1, t2, t3, t4;
		_fill_triple( &t0, y, subOrgOf, univ );
		_fill_triple( &t1, y, type, dept );
		_fill_triple( &t2, x, memberOf, y );
		_fill_triple( &t3, x, type, student );
		_fill_triple( &t4, x, email, z );
		triples[0]	= &t0;
		triples[1]	= &t1;
		triples[2]	= &t2;
		triples[3]	= &t3;
		triples[4]	= &t4;
	}
	
	hx_bgp* b	= hx_new_bgp( 5, triples );
	hx_execution_context* ctx	= hx_new_execution_context( NULL, hx );
	hx_variablebindings_iter* iter	= hx_bgp_execute( ctx, b );
	uint64_t counter	= 0;
	if (iter != NULL) {
	//	hx_variablebindings_iter_debug( iter, "lubm8> ", 0 );
		
		int size		= hx_variablebindings_iter_size( iter );
		char** names	= hx_variablebindings_iter_names( iter );
		
		int xi, yi, zi;
		int i;
		for (i = 0; i < size; i++) {
			if (strcmp(names[i], "x") == 0) {
				xi	= i;
			} else if (strcmp(names[i], "y") == 0) {
				yi	= i;
			} else if (strcmp(names[i], "z") == 0) {
				zi	= i;
			}
		}
		
		while (!hx_variablebindings_iter_finished( iter )) {
			counter++;
			hx_variablebindings* b;
			hx_variablebindings_iter_current( iter, &b );
			hx_node_id xid	= hx_variablebindings_node_id_for_binding ( b, xi );
			hx_node_id yid	= hx_variablebindings_node_id_for_binding ( b, yi );
			hx_node_id zid	= hx_variablebindings_node_id_for_binding ( b, zi );
			hx_node* x		= hx_store_get_node( hx->store, xid );
			hx_node* y		= hx_store_get_node( hx->store, yid );
			hx_node* z		= hx_store_get_node( hx->store, zid );
			
			char *xs, *ys, *zs;
			hx_node_string( x, &xs );
			hx_node_string( y, &ys );
			hx_node_string( z, &zs );
			printf( "%s\t%s\t%s\n", xs, ys, zs );
			free( xs );
			free( ys );
			free( zs );
			
			hx_free_variablebindings(b);
			hx_variablebindings_iter_next( iter );
		}
		
		hx_free_variablebindings_iter( iter );
	}
	
	if (counter == 0) {
		fprintf( stderr, "No results found.\n" );
	}
	hx_free_bgp( b );
	hx_free_node( x );
	hx_free_node( y );
	hx_free_node( z );
	hx_free_node( type );
	hx_free_node( subOrgOf );
	hx_free_node( univ );
	hx_free_node( dept );
	hx_free_node( memberOf );
	hx_free_node( student );
	hx_free_node( email );

	hx_free_execution_context( ctx );
	hx_free_model( hx );
	
	return 0;
}
