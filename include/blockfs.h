/* blockfs : A technology for Dynamic Storage Tiering to be used in
 * conjuction with loopback devices on Linux.
 */

#ifndef __BLOCKFS_H_
#define __BLOCKFS_H_

#define FUSE_USE_VERSION 26

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
/* For pread()/pwrite() */
#define _XOPEN_SOURCE 500
#endif /* linux */



#include <fuse.h>

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/types.h>


/***** TUNABLES ******/

/* The following parameters enable one to easily tune the caching
 * algorithms. Blocks are placed in the cache based on these parameters
 */

#define MIN_ACCESS_COUNT 512
#define MIN_READ_WRITE_RATIO 0.8
#define MAX_WRITE_COUNT 2048



/* The size of the blocks on the underlying block devices */
#define BLOCKFS_BLKSIZE 4096 


/* MACROS for block-2-byte and vice-versa manipulation, */
#define BLOCKNO_2_OFFSET(BNO) ((BNO) * BLOCKFS_BLKSIZE)
#define OFFSET_2_BLOCKNO(OFT) ((OFT) / BLOCKFS_BLKSIZE)
#define NUMBLOCKS(SIZE) ((SIZE) / BLOCKFS_BLKSIZE + ((SIZE) % BLOCKFS_BLKSIZE == 0 ? 0 : 1))


/* A macro to calculate the cache slot corresponding to a given block
   number */

#define CACHE_SLOT_NUM(BNO) ((BNO) & blockfs.cdev_size)


/* The structure where the global data of our filesystem will be
   stored */
struct blockfs
{
  
  /* The names of the block and cache devices */  
  char *cache_device;    
  char *block_device;
  

  /* The size of the cache and block devices in number of blocks of
     size BLOCKFS_BLKSIZE*/
  off_t cdev_size;        
  off_t blkdev_size;
  

  /* The path of the single file exported by blockfs */
  char path[10];  
  
  /* The file descriptors of the cache and block devices we are
   * using. 
   */
  int cdev_fd;
  int blkdev_fd;


};



/* The data structure we use to keep track of the blocks on the block
   device */

struct block_info{
  double read_count;
  double write_count;
  unsigned char flags; /* Bit 0 of this data field is a lock for the
			 given block.
			 
			 Bit 1 contains information whether the block
			 is cached or not. If the value is 0 then the
			 block is not cached. This bit is manipulated
			 by the MARK_CACHED, MARK_NOTCACHED, and
			 IS_CACHED macros.
		       */ 
};




/* Handles of the objects which store information about blocks on the
   block device and cache device respectively */

extern struct block_info *blkdev_block;

extern off_t *cdev_block; /* Contains the block_address currently
			     residing in each block of the cache. This
			     is distinct from block_number, which is
			     the block_address/BLOCKFS_BLKSIZE */



/* The handle of the data structure containing information about our
   filesystem */

extern struct blockfs blockfs;



/* The FUSE function declarations */

int blockfs_open(const char *path, struct fuse_file_info *fi);

int blockfs_getattr(const char *path, struct stat *stbuf);

int blockfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		    off_t offset, struct fuse_file_info *fi);

int blockfs_read(const char *path, char *buf, size_t size, off_t offset,
		 struct fuse_file_info *fi);


int blockfs_write(const char *path, const char *buf, size_t size,
		  off_t offset, struct fuse_file_info *fi);




/* Miscellaneous functions */
void blockfs_init();



 
#endif // __BLOCKFS_H_


