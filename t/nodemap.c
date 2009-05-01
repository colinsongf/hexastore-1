#include <unistd.h>
#include "nodemap.h"
#include "tap.h"

void test_nodemap ( void );
void test_file_nodemap ( void );
void test_with_nodemap ( hx_nodemap* map );

int main ( void ) {
	plan_tests(14);
	
	test_nodemap();
	test_file_nodemap();
	return exit_status();
}

void test_nodemap ( void ) {
	hx_nodemap* map	= hx_new_nodemap();
	ok1( map != NULL );
	test_with_nodemap( map );
	hx_free_nodemap( map );
}

void test_file_nodemap ( void ) {
	const char* filename	= "/tmp";
	hx_nodemap* map	= hx_new_file_nodemap(filename);
	ok1( map != NULL );
	test_with_nodemap( map );
	hx_remove_nodemap( map );
}


void test_with_nodemap ( hx_nodemap* map ) {
	hx_node* v1	= hx_new_node_variable( -1 );
	hx_node* v2	= hx_new_node_variable( -2 );
	hx_node* l1	= hx_new_node_literal("foo");
	hx_node* l2	= (hx_node*) hx_new_node_lang_literal("bar", "en-us");
	hx_node* l3	= (hx_node*) hx_new_node_dt_literal("7", "http://www.w3.org/2001/XMLSchema#integer");
	hx_node* r1	= (hx_node*) hx_new_node_resource("http://www.w3.org/2001/XMLSchema#integer");
	hx_node* b1	= (hx_node*) hx_new_node_blank("r1");
	
	hx_node_id id;
	
	{
		ok1( hx_nodemap_get_node_id( map, l1 ) == 0 );
		id	= hx_nodemap_add_node( map, l1 );
		
		ok1( id != 0 );
		ok1( hx_nodemap_get_node_id( map, l1 ) == id );
		
		ok1( hx_nodemap_remove_node( map, l1 ) == 0 );
		
		ok1( hx_nodemap_get_node_id( map, l1 ) == 0 );
	}
	
	{
		hx_node_id rid, bid;
		rid	= hx_nodemap_add_node( map, r1 );
		bid	= hx_nodemap_add_node( map, b1 );
		ok1( rid != bid );
	}
}
