### This is the generic Makefile for the hexastore code. It should work on most
### unix-like systems (development and testing happens on both linux and OS X).
### `make all` is what you want to run the first time through (which importantly
### drags in the `make sparql` target for running flex and bison). On subsequent
### makes, just `make` should do the trick without needing to re-generate the
### flex/bison stuff.

CDEFINES	= -DTIMING_CPU_FREQUENCY=2400000000.0 # -DDEBUG
CFLAGS		= -arch i386 -I. -L. -I/usr/local/include -L/usr/local/lib -I/ext/local/include -L/ext/local/lib -std=gnu99 -pedantic -ggdb $(CDEFINES)
CC			= gcc $(CFLAGS)

LIBS	=	-ltokyocabinet -lpthread -lraptor

HEXASTORE_OBJECTS	= store/hexastore/hexastore.o store/hexastore/index.o store/hexastore/terminal.o store/hexastore/vector.o store/hexastore/head.o store/hexastore/btree.o
TCSTORE_OBJECTS		= store/tokyocabinet/tokyocabinet.o store/tokyocabinet/tcindex.o
STORE_OBJECTS		= store/store.o $(HEXASTORE_OBJECTS) $(TCSTORE_OBJECTS)
MISC_OBJECTS		= misc/avl.o misc/nodemap.o misc/util.o misc/idmap.c
RDF_OBJECTS			= rdf/node.o rdf/triple.o
ENGINE_OBJECTS		= engine/expr.o engine/variablebindings_iter.o engine/variablebindings_iter_sorting.o engine/nestedloopjoin.o engine/mergejoin.o engine/materialize.o engine/filter.o engine/project.o engine/hashjoin.o engine/bgp.o engine/graphpattern.o
ALGEBRA_OBJECTS		= algebra/variablebindings.o algebra/bgp.o algebra/expr.o algebra/graphpattern.o
PARSER_OBJECTS		= parser/SPARQLParser.o parser/SPARQLScanner.o parser/parser.o
OPT_OBJECTS			= optimizer/optimizer.o optimizer/plan.o
OBJECTS				= hexastore.o $(STORE_OBJECTS) $(MISC_OBJECTS) $(RDF_OBJECTS) $(ENGINE_OBJECTS) $(ALGEBRA_OBJECTS) $(PARSER_OBJECTS) $(OPT_OBJECTS)

default: parse print optimize tests examples parse_query

all: sparql parse print optimize tests examples parse_query dumpmap assign_ids

server: cli/server.c $(OBJECTS)
	$(CC) $(INC) $(LIBS) -ldrizzle -o server cli/server.c $(OBJECTS)

assign_ids: cli/assign_ids.c $(OBJECTS)
	$(CC) $(INC) $(LIBS) -ltokyocabinet -o assign_ids cli/assign_ids.c $(OBJECTS)

parse: cli/parse.c $(OBJECTS)
	$(CC) $(INC) $(LIBS) -o parse cli/parse.c $(OBJECTS)

optimize: cli/optimize.c $(OBJECTS)
	$(CC) $(INC) $(LIBS) -o optimize cli/optimize.c $(OBJECTS)

print: cli/print.c $(OBJECTS)
	$(CC) $(INC) $(LIBS) -o print cli/print.c $(OBJECTS)

parse_query: cli/parse_query.c $(OBJECTS)
	$(CC) $(INC) $(LIBS) -o parse_query cli/parse_query.c $(OBJECTS)

dumpmap: cli/dumpmap.c $(OBJECTS)
	$(CC) $(INC) $(LIBS) -o dumpmap cli/dumpmap.c $(OBJECTS)

hexastore.o: hexastore.c hexastore.h store/hexastore/index.h store/hexastore/head.h store/hexastore/vector.h store/hexastore/terminal.h hexastore_types.h algebra/variablebindings.h misc/nodemap.h
	$(CC) $(INC) -c hexastore.c

store/hexastore/index.o: store/hexastore/index.c store/hexastore/index.h store/hexastore/terminal.h store/hexastore/vector.h store/hexastore/head.h hexastore_types.h
	$(CC) $(INC) -c -o store/hexastore/index.o store/hexastore/index.c

store/store.o: store/store.c store/store.h
	$(CC) $(INC) -c -o store/store.o store/store.c

store/tokyocabinet/tokyocabinet.o: store/tokyocabinet/tokyocabinet.c store/tokyocabinet/tokyocabinet.h hexastore_types.h
	$(CC) $(INC) -c -o store/tokyocabinet/tokyocabinet.o store/tokyocabinet/tokyocabinet.c

store/tokyocabinet/tcindex.o: store/tokyocabinet/tcindex.c store/tokyocabinet/tcindex.h hexastore_types.h
	$(CC) $(INC) -c -o store/tokyocabinet/tcindex.o store/tokyocabinet/tcindex.c

store/hexastore/hexastore.o: store/hexastore/hexastore.c store/hexastore/hexastore.h store/hexastore/head.h store/hexastore/vector.h store/hexastore/terminal.h store/hexastore/btree.h hexastore_types.h
	$(CC) $(INC) -c -o store/hexastore/hexastore.o store/hexastore/hexastore.c

store/hexastore/terminal.o: store/hexastore/terminal.c store/hexastore/terminal.h hexastore_types.h
	$(CC) $(INC) -c -o store/hexastore/terminal.o store/hexastore/terminal.c

store/hexastore/vector.o: store/hexastore/vector.c store/hexastore/vector.h store/hexastore/terminal.h hexastore_types.h
	$(CC) $(INC) -c -o store/hexastore/vector.o store/hexastore/vector.c

store/hexastore/head.o: store/hexastore/head.c store/hexastore/head.h store/hexastore/vector.h store/hexastore/terminal.h store/hexastore/btree.h hexastore_types.h
	$(CC) $(INC) -c -o store/hexastore/head.o store/hexastore/head.c

rdf/node.o: rdf/node.c rdf/node.h hexastore_types.h
	$(CC) $(INC) -c -o rdf/node.o rdf/node.c
	
misc/nodemap.o: misc/nodemap.c misc/nodemap.h misc/avl.h hexastore_types.h
	$(CC) $(INC) -c -o misc/nodemap.o misc/nodemap.c

misc/idmap.o: misc/idmap.c misc/idmap.h misc/avl.h hexastore_types.h
	$(CC) $(INC) -c -o misc/idmap.o misc/idmap.c

engine/bgp.o: engine/bgp.c engine/bgp.h hexastore_types.h
	$(CC) $(INC) -c -o engine/bgp.o engine/bgp.c

engine/expr.o: engine/expr.c engine/expr.h hexastore_types.h
	$(CC) $(INC) -c -o engine/expr.o engine/expr.c

engine/graphpattern.o: engine/graphpattern.c engine/graphpattern.h hexastore_types.h
	$(CC) $(INC) -c -o engine/graphpattern.o engine/graphpattern.c

engine/mergejoin.o: engine/mergejoin.c engine/mergejoin.h hexastore_types.h algebra/variablebindings.h
	$(CC) $(INC) -c -o engine/mergejoin.o engine/mergejoin.c

engine/nestedloopjoin.o: engine/nestedloopjoin.c engine/nestedloopjoin.h hexastore_types.h algebra/variablebindings.h
	$(CC) $(INC) -c -o engine/nestedloopjoin.o engine/nestedloopjoin.c

engine/hashjoin.o: engine/hashjoin.c engine/hashjoin.h hexastore_types.h algebra/variablebindings.h
	$(CC) $(INC) -c -o engine/hashjoin.o engine/hashjoin.c

algebra/variablebindings.o: algebra/variablebindings.c algebra/variablebindings.h hexastore_types.h rdf/node.h misc/nodemap.h
	$(CC) $(INC) -c -o algebra/variablebindings.o algebra/variablebindings.c

engine/variablebindings_iter.o: engine/variablebindings_iter.c engine/variablebindings_iter.h hexastore_types.h rdf/node.h misc/nodemap.h
	$(CC) $(INC) -c -o engine/variablebindings_iter.o engine/variablebindings_iter.c

engine/variablebindings_iter_sorting.o: engine/variablebindings_iter_sorting.c engine/variablebindings_iter_sorting.h hexastore_types.h rdf/node.h misc/nodemap.h
	$(CC) $(INC) -c -o engine/variablebindings_iter_sorting.o engine/variablebindings_iter_sorting.c

engine/materialize.o: engine/materialize.c engine/materialize.h hexastore_types.h rdf/node.h misc/nodemap.h
	$(CC) $(INC) -c -o engine/materialize.o engine/materialize.c

engine/filter.o: engine/filter.c engine/filter.h hexastore_types.h rdf/node.h misc/nodemap.h
	$(CC) $(INC) -c -o engine/filter.o engine/filter.c

rdf/triple.o: rdf/triple.c rdf/triple.h hexastore_types.h
	$(CC) $(INC) -c -o rdf/triple.o rdf/triple.c

store/hexastore/btree.o: store/hexastore/btree.c store/hexastore/btree.h hexastore_types.h
	$(CC) $(INC) -c -o store/hexastore/btree.o store/hexastore/btree.c

parser/parser.o: parser/parser.c parser/parser.h hexastore_types.h
	$(CC) $(INC) -c -o parser/parser.o parser/parser.c

algebra/bgp.o: algebra/bgp.c algebra/bgp.h hexastore_types.h
	$(CC) $(INC) -c -o algebra/bgp.o algebra/bgp.c

algebra/expr.o: algebra/expr.c algebra/expr.h hexastore_types.h
	$(CC) $(INC) -c -o algebra/expr.o algebra/expr.c

algebra/graphpattern.o: algebra/graphpattern.c algebra/graphpattern.h hexastore_types.h
	$(CC) $(INC) -c -o algebra/graphpattern.o algebra/graphpattern.c

engine/project.o: engine/project.c engine/project.h hexastore_types.h
	$(CC) $(INC) -c -o engine/project.o engine/project.c

misc/util.o: misc/util.c misc/util.h
	$(CC) $(INC) -c -o misc/util.o misc/util.c

optimizer/optimizer.o: optimizer/optimizer.c optimizer/optimizer.h
	$(CC) $(INC) -c -o optimizer/optimizer.o optimizer/optimizer.c

optimizer/plan.o: optimizer/plan.c optimizer/plan.h
	$(CC) $(INC) -c -o optimizer/plan.o optimizer/plan.c

########

# SPARQLParser.c:
# SPARQLParser.h: SPARQLParser.yy
sparql: parser/SPARQLParser.yy parser/SPARQLScanner.ll
	bison -o parser/SPARQLParser.c parser/SPARQLParser.yy
	flex -o parser/SPARQLScanner.c parser/SPARQLScanner.ll

parser/SPARQLScanner.h: parser/SPARQLScanner.c parser/SPARQLScanner.ll

parser/SPARQLScanner.c: parser/SPARQLScanner.ll

parser/SPARQLParser.o: parser/SPARQLParser.yy parser/SPARQLScanner.ll parser/SPARQLParser.h parser/SPARQLScanner.h
	$(CC) -DYYTEXT_POINTER=1 -W -Wall -Wextra -ansi -g -c  -o parser/SPARQLParser.o parser/SPARQLParser.c

parser/SPARQLScanner.o: parser/SPARQLScanner.c parser/SPARQLParser.h parser/SPARQLScanner.h
	$(CC) -DYYTEXT_POINTER=1 -Wextra -ansi -g -c  -o parser/SPARQLScanner.o parser/SPARQLScanner.c

########

libhx.o: $(OBJECTS)
	libtool $(OBJECTS) -o libhx.o

########

tests: t/nodemap.t t/node.t t/expr.t t/index.t t/terminal.t t/vector.t t/head.t t/btree.t t/join.t t/iter.t t/bgp.t t/materialize.t t/selectivity.t t/filter.t t/graphpattern.t t/parser.t t/variablebindings.t t/project.t t/triple.t t/hash.t t/store-hexastore.t t/store-tokyocabinet.t t/tokyocabinet.t t/optimizer.t

examples: examples/lubm_q4 examples/lubm_q8 examples/lubm_q9 examples/bench examples/knows

# mpi: examples/mpi

########
t/node.t: test/tap.o t/node.c rdf/node.h $(OBJECTS) test/tap.o
	$(CC) $(INC) $(LIBS) -o t/node.t t/node.c $(OBJECTS) test/tap.o

t/expr.t: test/tap.o t/expr.c algebra/expr.h $(OBJECTS) test/tap.o
	$(CC) $(INC) $(LIBS) -o t/expr.t t/expr.c $(OBJECTS) test/tap.o

t/nodemap.t: test/tap.o t/nodemap.c misc/nodemap.h $(OBJECTS) test/tap.o
	$(CC) $(INC) $(LIBS) -o t/nodemap.t t/nodemap.c $(OBJECTS) test/tap.o

t/index.t: test/tap.o t/index.c store/hexastore/index.h $(OBJECTS) test/tap.o
	$(CC) $(INC) $(LIBS) -o t/index.t t/index.c $(OBJECTS) test/tap.o

t/terminal.t: test/tap.o t/terminal.c store/hexastore/terminal.h $(OBJECTS) test/tap.o
	$(CC) $(INC) $(LIBS) -o t/terminal.t t/terminal.c $(OBJECTS) test/tap.o

t/vector.t: test/tap.o t/vector.c store/hexastore/vector.h $(OBJECTS) test/tap.o
	$(CC) $(INC) $(LIBS) -o t/vector.t t/vector.c $(OBJECTS) test/tap.o

t/head.t: test/tap.o t/head.c store/hexastore/head.h $(OBJECTS) test/tap.o
	$(CC) $(INC) $(LIBS) -o t/head.t t/head.c $(OBJECTS) test/tap.o

t/btree.t: test/tap.o t/btree.c store/hexastore/btree.h $(OBJECTS) test/tap.o
	$(CC) $(INC) $(LIBS) -o t/btree.t t/btree.c $(OBJECTS) test/tap.o

t/join.t: test/tap.o t/join.c $(OBJECTS) test/tap.o
	$(CC) $(INC) $(LIBS) -o t/join.t t/join.c $(OBJECTS) test/tap.o

t/iter.t: test/tap.o t/iter.c $(OBJECTS) test/tap.o
	$(CC) $(INC) $(LIBS) -o t/iter.t t/iter.c $(OBJECTS) test/tap.o

t/bgp.t: test/tap.o t/bgp.c $(OBJECTS) test/tap.o
	$(CC) $(INC) $(LIBS) -o t/bgp.t t/bgp.c $(OBJECTS) test/tap.o

t/filter.t: test/tap.o t/filter.c $(OBJECTS) test/tap.o
	$(CC) $(INC) $(LIBS) -o t/filter.t t/filter.c $(OBJECTS) test/tap.o

t/materialize.t: test/tap.o t/materialize.c $(OBJECTS) test/tap.o
	$(CC) $(INC) $(LIBS) -o t/materialize.t t/materialize.c $(OBJECTS) test/tap.o

t/selectivity.t: test/tap.o t/selectivity.c $(OBJECTS) test/tap.o
	$(CC) $(INC) $(LIBS) -o t/selectivity.t t/selectivity.c $(OBJECTS) test/tap.o

t/graphpattern.t: test/tap.o t/graphpattern.c $(OBJECTS) test/tap.o
	$(CC) $(INC) $(LIBS) -o t/graphpattern.t t/graphpattern.c $(OBJECTS) test/tap.o

t/parser.t: test/tap.o t/parser.c $(OBJECTS) test/tap.o
	$(CC) $(INC) $(LIBS) -o t/parser.t t/parser.c $(OBJECTS) test/tap.o

t/variablebindings.t: test/tap.o t/variablebindings.c $(OBJECTS) test/tap.o
	$(CC) $(INC) $(LIBS) -o t/variablebindings.t t/variablebindings.c $(OBJECTS) test/tap.o

t/project.t: test/tap.o t/project.c $(OBJECTS) test/tap.o
	$(CC) $(INC) $(LIBS) -o t/project.t t/project.c $(OBJECTS) test/tap.o

t/triple.t: test/tap.o t/triple.c $(OBJECTS) test/tap.o
	$(CC) $(INC) $(LIBS) -o t/triple.t t/triple.c $(OBJECTS) test/tap.o

t/hash.t: test/tap.o t/hash.c $(OBJECTS) test/tap.o
	$(CC) $(INC) $(LIBS) -o t/hash.t t/hash.c $(OBJECTS) test/tap.o

t/optimizer.t: test/tap.o t/optimizer.c $(OBJECTS) test/tap.o
	$(CC) $(INC) $(LIBS) -o t/optimizer.t t/optimizer.c $(OBJECTS) test/tap.o

t/store-hexastore.t: test/tap.o t/store-hexastore.c store/hexastore/hexastore.h $(OBJECTS) test/tap.o
	$(CC) $(INC) $(LIBS) -o t/store-hexastore.t t/store-hexastore.c $(OBJECTS) test/tap.o

t/store-tokyocabinet.t: test/tap.o t/store-tokyocabinet.c store/tokyocabinet/tokyocabinet.h $(OBJECTS) test/tap.o
	$(CC) $(INC) $(LIBS) -o t/store-tokyocabinet.t t/store-tokyocabinet.c $(OBJECTS) test/tap.o

t/tokyocabinet.t: test/tap.o t/tokyocabinet.c store/hexastore/hexastore.h $(OBJECTS) test/tap.o
	$(CC) $(INC) $(LIBS) -o t/tokyocabinet.t t/tokyocabinet.c $(OBJECTS) test/tap.o

########

examples/lubm_q4: examples/lubm_q4.c $(OBJECTS)
	$(CC) $(INC) $(LIBS) -o examples/lubm_q4 examples/lubm_q4.c $(OBJECTS)

examples/lubm_q8: examples/lubm_q8.c $(OBJECTS)
	$(CC) $(INC) $(LIBS) -o examples/lubm_q8 examples/lubm_q8.c $(OBJECTS)

examples/lubm_q9: examples/lubm_q9.c $(OBJECTS)
	$(CC) $(INC) $(LIBS) -o examples/lubm_q9 examples/lubm_q9.c $(OBJECTS)

examples/bench: examples/bench.c $(OBJECTS)
	$(CC) $(INC) $(LIBS) -o examples/bench examples/bench.c $(OBJECTS)

examples/knows: examples/knows.c $(OBJECTS)
	$(CC) $(INC) $(LIBS) -o examples/knows examples/knows.c $(OBJECTS)

########

misc/avl.o: misc/avl.c misc/avl.h hexastore_types.h
	$(CC) $(INC) -c -o misc/avl.o misc/avl.c

test/tap.o: test/tap.c test/tap.h
	$(CC) $(INC) -c -o test/tap.o test/tap.c

distclean:
	rm -f SPARQL parser/SPARQLParser.o parser/SPARQLScanner.o parser/SPARQLParser.c parser/SPARQLScanner.c parser/SPARQLParser.h
	rm -f examples/lubm_q[489] examples/bench examples/knows examples/mpi
	rm -rf examples/lubm_q[489].dSYM examples/bench.dSYM examples/knows.dSYM examples/mpi.dSYM
	rm -f parse print optimize a.out server parse_query dumpmap assign_ids
	rm -f *.o */*.o */*/*.o
	rm -rf *.dSYM t/*.dSYM
	rm -f t/*.t
	rm -f stack.h position.h location.h
	
clean:
	rm -f examples/lubm_q[489] examples/bench examples/knows examples/mpi
	rm -rf examples/lubm_q[489].dSYM examples/bench.dSYM examples/knows.dSYM examples/mpi.dSYM
	rm -f parse print optimize a.out server parse_query dumpmap assign_ids
	rm -f *.o */*.o */*/*.o
	rm -rf *.dSYM t/*.dSYM
	rm -f t/*.t
	rm -f stack.h position.h location.h
