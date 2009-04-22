#include "storage.h"

int _hx_storage_mmap_grow ( hx_storage_manager* s );

hx_storage_manager* hx_new_memory_storage_manager( void ) {
	hx_storage_manager* s	= (hx_storage_manager*) calloc( 1, sizeof( hx_storage_manager ) );
	s->flags				= HX_STORAGE_MEMORY;
	return s;
}

hx_storage_manager* hx_new_mmap_storage_manager( const char* filename ) {
	int fd	= open( filename, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR );
	if (fd == -1) {
		perror( "Failed to open mmap file" );
		return NULL;
	}
	
	hx_storage_manager* s	= (hx_storage_manager*) calloc( 1, sizeof( hx_storage_manager ) );
	s->filename	= filename;
	struct stat st;
	fstat( fd, &st );
	
	if (st.st_size == 0) {
		for (int i = 0; i < 4096; i++)
			write( fd, "", 1 );
		fstat( fd, &st );
	}
	
	s->fd	= fd;
	s->m	= mmap( NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 );
	if (s->m == NULL) {
		free( s );
		return NULL;
	}
	
	uint64_t next_id	= HX_STORAGE_HEADER_SIZE;
	memcpy( s->m, "HXMM", 4 );
	memcpy( &(((uint8_t*) s->m)[4]), &next_id, sizeof( uint64_t ) );
	
	s->size					= st.st_size;
	s->flags				= HX_STORAGE_MMAP;
	return s;
}

hx_storage_manager* hx_open_mmap_storage_manager( const char* filename, int prot ) {
	int fd	= open( filename, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR );
	if (fd == -1) {
		perror( "Failed to open mmap file" );
		return NULL;
	}
	
	hx_storage_manager* s	= (hx_storage_manager*) calloc( 1, sizeof( hx_storage_manager ) );
	s->filename	= filename;
	struct stat st;
	fstat( fd, &st );
	s->fd	= fd;
	s->prot	= prot;
	int flags	= (s->prot & PROT_WRITE) ? MAP_SHARED : MAP_PRIVATE;
	s->m	= mmap( NULL, st.st_size, s->prot | PROT_WRITE, flags, fd, 0 );
	if (s->m == NULL) {
		free( s );
		return NULL;
	}
	
	if (strncmp( (const char*) s->m, "HXMM", 4 ) != 0) {
		munmap( s->m, st.st_size );
		free( s );
		fprintf( stderr, "*** Bad header cookie while trying to open mmaped hexastore\n" );
		return NULL;
	}
	
	s->size					= st.st_size;
	s->flags				= HX_STORAGE_MMAP;
	return s;
}

int _hx_storage_mmap_grow ( hx_storage_manager* s ) {
	if (s->flags & HX_STORAGE_MMAP) {
		if (s->freeze_handler != NULL) {
			s->freeze_handler( s, s->freeze_arg );
		}
		uint64_t next_block	= HX_STORAGE_MMAP_NEXT_BLOCK(s);
		munmap( s->m, s->size );
		fsync( s->fd );
		
		off_t addsize	= (s->size == 0) ? 4096 : s->size;
		off_t newsize	= s->size + addsize;
		if (lseek( s->fd, 0, SEEK_END ) == -1) {
			perror( "Failed to seek to end of mmap file" );
		}
		
		for (int i = 0; i < addsize; i++)
			write( s->fd, "", 1 );
		fsync( s->fd );
		
		if (lseek( s->fd, 0, SEEK_SET ) == -1) {
			perror( "Failed to seek to beginning of mmap file" );
		}
		
		struct stat st;
		fstat( s->fd, &st );
		if (st.st_size != newsize) {
			fprintf( stderr, "*** expected new size is %d bytes, but got %d bytes\n", (int) newsize, (int) st.st_size );
		}
		
		void* prev	= s->m;
		int flags	= (s->prot & PROT_WRITE) ? MAP_SHARED : MAP_PRIVATE;
		s->m	= mmap( s->m, newsize, s->prot | PROT_WRITE, flags | MAP_FIXED, s->fd, 0 );
		if (s->m == NULL) {
			fprintf( stderr, "*** could not mmap after growing file\n" );
			free( s );
			return 1;
		} else if (s->m != prev) {
			fprintf( stderr, "*** mmap address has changed\n" );
		}
		HX_STORAGE_MMAP_NEXT_BLOCK(s)	= next_block;
		s->size		= st.st_size;
		if (s->thaw_handler != NULL) {
			s->thaw_handler( s, s->thaw_arg );
		}
		return 0;
	} else {
		return 1;
	}
}

hx_storage_manager* hx_new_file_storage_manager( const char* filename );
hx_storage_manager* hx_open_file_storage_manager( const char* filename );

int hx_free_storage_manager( hx_storage_manager* s ) {
	if (s->flags & HX_STORAGE_MEMORY) {
		free( s );
		return 0;
	} else if (s->flags & HX_STORAGE_MMAP) {
		munmap( s->m, s->size );
		close( s->fd );
		free( s );
		return 0;
	} else {
		fprintf( stderr, "*** trying to free unimplemented storage manager\n" );
		return 1;
	}
}

void* hx_storage_new_block( hx_storage_manager* s, size_t size ) {
	if (s->flags & HX_STORAGE_MEMORY) {
		return calloc( 1, size );
	} else if (s->flags & HX_STORAGE_MMAP) {
//		fprintf( stderr, "allocating %d bytes in mmap\n", (int) size );
		while ((HX_STORAGE_MMAP_NEXT_BLOCK(s) + HX_STORAGE_BLOCK_HEADER_SIZE + size) >= s->size) {
// 			fprintf( stderr, "*** need to grow mmap file:\n" );
// 			fprintf( stderr, "\tcurrent size is %d bytes\n", (int) s->size );
// 			fprintf( stderr, "\tcurrent offset is %d bytes\n", (int) HX_STORAGE_MMAP_NEXT_BLOCK(s) );
// 			fprintf( stderr, "\trequested new allocation is %d bytes\n", (int) size );
			_hx_storage_mmap_grow( s );
		}
		void* p			= (void*) ((int8_t*) s->m + HX_STORAGE_MMAP_NEXT_BLOCK(s));
		uint32_t* sz	= (uint32_t*) p;
		*sz				= (uint32_t) size;
		HX_STORAGE_MMAP_NEXT_BLOCK(s)	+= size;
		void* block		= ((int8_t*) p + sizeof( uint32_t ) );
//		fprintf( stderr, "-> %p\n", (void*) block );
		return block;
	} else {
		fprintf( stderr, "*** trying to create new block with unimplemented storage manager\n" );
		return NULL;
	}
}

int hx_storage_release_block( hx_storage_manager* s, void* block ) {
	if (s->flags & HX_STORAGE_MEMORY) {
		free( block );
		return 0;
	} else if (s->flags & HX_STORAGE_MMAP) {
		// XXX mark this page as unused somehow, so that hx_storage_new_block might re-use it...
		return 0;
	} else {
		fprintf( stderr, "*** trying to free block with unimplemented storage manager\n" );
		return 1;
	}
}

int hx_storage_sync_block( hx_storage_manager* s, void* block ) {
	if (s->flags & HX_STORAGE_MEMORY) {
		return 0;
	} else if (s->flags & HX_STORAGE_MMAP) {
		// XXX
		return 0;
	} else {
		fprintf( stderr, "*** trying to sync block with unimplemented storage manager\n" );
		return 1;
	}
}

hx_storage_id_t hx_storage_id_from_block ( hx_storage_manager* s, void* block ) {
	if (s->flags & HX_STORAGE_MEMORY) {
		return (hx_storage_id_t) block;
	} else if (s->flags & HX_STORAGE_MMAP) {
		hx_storage_id_t id	= (hx_storage_id_t) ((intptr_t) block - (intptr_t) s->m);
		return id;
	} else {
		fprintf( stderr, "*** trying to get block id with unimplemented storage manager\n" );
		return 0;
	}
}

void* hx_storage_block_from_id ( hx_storage_manager* s, hx_storage_id_t id ) {
	if (s->flags & HX_STORAGE_MEMORY) {
//		fprintf( stderr, "%x <> %p\n", id, (void*) id );
		return (void*) id;
	} else if (s->flags & HX_STORAGE_MMAP) {
		if (id == 0) {
			return NULL;
		} else {
			void* p	= (void*) ((uint8_t*) s->m + id);
			return p;
		}
	} else {
		fprintf( stderr, "*** trying to get block pointer with unimplemented storage manager\n" );
		return NULL;
	}
}

void* hx_storage_first_block ( hx_storage_manager* s ) {
	return hx_storage_block_from_id( s, HX_STORAGE_HEADER_SIZE + HX_STORAGE_BLOCK_HEADER_SIZE );
}

int hx_storage_set_freeze_remap_handler ( hx_storage_manager* s, hx_storage_handler* h, void* arg ) {
	s->freeze_handler	= h;
	s->freeze_arg		= arg;
	return 0;
}

int hx_storage_set_thaw_remap_handler ( hx_storage_manager* s, hx_storage_handler* h, void* arg ) {
	s->thaw_handler	= h;
	s->thaw_arg		= arg;
	return 0;
}

