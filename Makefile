EXEC = blockfs

INCLUDEDIRS = include/
FUSE_CFLAGS = $(shell pkg-config fuse --cflags)
FUSE_LIBFLAGS = $(shell pkg-config fuse --libs)

CFLAGS = -Wall  $(FUSE_CFLAGS) -I $(INCLUDEDIRS)

OBJFILES = 	src/blockfs.o 		\
		src/open.o 		\
		src/readdir.o 		\
		src/getattr.o		\
		src/read.o 		\
		src/write.o		\
		src/init.o		\
		src/utils.o		\



$(EXEC) : $(OBJFILES)
	$(CC) -Wall $(FUSE_LIBFLAGS) -o $(EXEC) $(OBJFILES)

src/blockfs.o : include/blockfs.h

src/open.o : include/blockfs.h

src/readdir.o : include/blockfs.h

src/read.o : include/blockfs.h include/blockfs_locks.h include/blockfs_utils.h

src/write.o : include/blockfs.h include/blockfs_locks.h

src/getattr.o : include/blockfs.h

src/utils.o : include/blockfs.h

src/init.o : include/blockfs.h

# Clean all object and executable files

clean : 
	rm  src/*.o  $(EXEC)

# Clean all files, including backup files created by the editors

cleanall :
	rm src/*.o src/*~ $(EXEC) include/*~ ./*~

