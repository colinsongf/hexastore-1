#include <time.h>
#include <stdio.h>
#include <raptor.h>
#include <inttypes.h>
#include "hexastore.h"
#include "node.h"
#include "parser.h"

#define DIFFTIME(a,b) ((b-a)/(double)CLOCKS_PER_SEC)

void help (int argc, char** argv);
int main (int argc, char** argv);

static int count	= 0;

void help (int argc, char** argv) {
	fprintf( stderr, "Usage: %s data.rdf hexastore_dir\n\n", argv[0] );
}

void logger ( uint64_t _count ) {
	fprintf( stderr, "\rParsed %lu triples...", (unsigned long) _count );
}

int main (int argc, char** argv) {
	const char* rdf_filename	= NULL;
	const char* output_dir		= NULL;
	
	if (argc < 3) {
		help(argc, argv);
		exit(1);
	}
	
	rdf_filename	= argv[1];
	output_dir		= argv[2];
	
	hx_storage_manager* s	= hx_new_memory_storage_manager();
	hx_hexastore* hx		= hx_new_tchexastore( s, output_dir );
	
	hx_parser* parser	= hx_new_parser();
	hx_parser_set_logger( parser, logger );
	
	clock_t st_time	= clock();
	uint64_t total	= hx_parser_parse_file_into_hexastore( parser, hx, s, rdf_filename );
	clock_t end_time	= clock();
	
	double elapsed	= DIFFTIME(st_time, end_time);
	double tps	= ((double) total / elapsed);
	fprintf( stderr, "\rParsed %lu triples in %.1lf seconds (%.1lf triples/second)\n", (unsigned long) total, elapsed, tps );
	
	hx_free_parser( parser );
	hx_free_hexastore( hx, s );
	hx_free_storage_manager( s );
	return 0;
}

