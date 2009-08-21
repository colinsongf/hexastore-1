#include <unistd.h>
#include "store/hexastore/terminal.h"
#include "test/tap.h"

void terminal_test ( void );
void terminal_iter_test ( void );

int main ( void ) {
	plan_tests(113);
	terminal_test();
	terminal_iter_test();
	return exit_status();
}

void terminal_test ( void ) {
	hx_terminal* t	= hx_new_terminal( NULL );
	ok1( t != NULL );
	
	ok1( hx_terminal_size(t) == 0 );
	ok1( hx_terminal_add_node(t, (hx_node_id) 1 ) == 0 );
	ok1( hx_terminal_size(t) == 1 );
	ok1( hx_terminal_add_node(t, (hx_node_id) 1 ) == 1 );
	ok1( hx_terminal_size(t) == 1 );
	ok1( hx_terminal_contains_node( t, (hx_node_id) 1 ) == 1 );
	ok1( hx_terminal_contains_node( t, (hx_node_id) 2 ) == 0 );
	
	int i;
	for (i = 5000; i > 0; i--) {
		hx_terminal_add_node(t, (hx_node_id) i );
	}
	ok1( hx_terminal_size(t) == 5000 );
	
	ok1( hx_terminal_contains_node( t, (hx_node_id) 5000 ) == 1 );
	ok1( hx_terminal_contains_node( t, (hx_node_id) 5001 ) == 0 );
	
	for (i = 1; i <= 5000; i++) {
		hx_terminal_remove_node(t, (hx_node_id) i );
	}
	
	ok1( hx_terminal_size(t) == 0 );
	
	hx_free_terminal(t);
}

void terminal_iter_test ( void ) {
	hx_terminal* t	= hx_new_terminal( NULL );
	int i;
	for (i = 200; i > 0; i -= 2) {
		hx_terminal_add_node(t, (hx_node_id) i );
	}
	
	hx_terminal_iter* iter	= hx_terminal_new_iter( t );
	ok1( iter != NULL );
	
	int counter	= 0;
	hx_node_id last, cur;
	while (!hx_terminal_iter_finished(iter)) {
		hx_terminal_iter_current( iter, &cur );
		if (counter > 0) {
			ok1( last < cur );
		}
		last	= cur;
		counter++;
		hx_terminal_iter_next(iter);
	}
	ok1( counter == 100 );
	
	hx_free_terminal_iter( iter );
	hx_free_terminal(t);
}
