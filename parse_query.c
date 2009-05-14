#include "SPARQLParser.h"
#include <time.h>
#include "bgp.h"
#define DIFFTIME(a,b) ((b-a)/(double)CLOCKS_PER_SEC)

extern hx_bgp* parse_bgp_query ( void );

void help (int argc, char** argv) {
	fprintf( stderr, "Usage:\n" );
	fprintf( stderr, "\t%s hexastore.dat\n", argv[0] );
	fprintf( stderr, "\t\tReads a SPARQL query on standard input.\n" );
	fprintf( stderr, "\n\n" );
}

int main( int argc, char** argv ) {
	char* filename	= argv[1];
	FILE* f	= fopen( filename, "r" );
	
	if (argc < 2) {
		help( argc, argv );
		exit(1);
	}
	
	if (f == NULL) {
		perror( "Failed to open hexastore file for reading: " );
		return 1;
	}
	
	int dryrun	= 0;
	if (argc > 2) {
		for (int i = 2; i < argc; i++) {
			if (strncmp(argv[i], "-n", 2) == 0) {
				dryrun	= 1;
			}
		}
	}
	
	hx_storage_manager* st	= hx_new_memory_storage_manager();
	hx_hexastore* hx	= hx_read( st, f, 0 );
	hx_nodemap* map		= hx_get_nodemap( hx );
	
	hx_bgp* b	= parse_bgp_query();
	if (b == NULL) {
		fprintf( stderr, "Failed to parse query\n" );
		return 1;
	}
	
	char* sse;
	hx_bgp_sse( b, &sse, "  ", 0 );
	fprintf( stdout, sse );
	free( sse );
	
	if (!dryrun) {
		clock_t st_time	= clock();
		uint64_t count	= 0;
		hx_variablebindings_iter* iter	= hx_bgp_execute( b, hx, st );
		int size		= hx_variablebindings_iter_size( iter );
		char** names	= hx_variablebindings_iter_names( iter );
	// 	for (int i = 0; i < size; i++) {
	// 		fprintf( stderr, "column: %s\n", names[i] );
	// 	}
		
		if (iter != NULL) {
			while (!hx_variablebindings_iter_finished( iter )) {
				count++;
				hx_variablebindings* b;
				hx_variablebindings_iter_current( iter, &b );
				
				fprintf( stdout, "Row %d:\n", (int) count );
				for (int i = 0; i < size; i++) {
					char* string;
					hx_node* node	= hx_variablebindings_node_for_binding( b, map, i );
					hx_node_string( node, &string );
					fprintf( stdout, "\t%s: %s\n", names[i], string );
					free( string );
				}
				
				hx_free_variablebindings( b, 0 );
				hx_variablebindings_iter_next( iter );
			}
		}
		clock_t end_time	= clock();
		
		fprintf( stderr, "%d total results\n", (int) count );
		fprintf( stderr, "query execution time: %lfs\n", DIFFTIME(st_time, end_time) );
	}
	
	hx_free_hexastore( hx, st );
	hx_free_storage_manager( st );
	return 0;
}

