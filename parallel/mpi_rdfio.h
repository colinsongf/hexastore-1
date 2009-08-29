#ifndef _MPI_RDFIO_H
#define _MPI_RDFIO_H

#include "mpi.h"
#include "hexastore.h"

int mpi_rdfio_readnt(char *filename, char *mapfilename, size_t bufsize, hx_hexastore **store, MPI_Comm comm);
int mpi_rdfio_readids(char *filename, size_t bufsize, hx_hexastore **store, MPI_Comm comm);

#endif /* _MPI_RDFIO_H */