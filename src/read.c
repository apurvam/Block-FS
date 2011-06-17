#include <blockfs.h>
#include <blockfs_locks.h>
#include <blockfs_utils.h>


/* 
   This function sits between the loopback device driver and the
   actual (block and cache) devices.  The things to do here are:

   (0) Determine whether the request can be satisfied from the
   cache.
       
   (1) Copy data from the cache or block device into the passed
   buffer. 

   (2) Update the details (read and write counts) of the block in the
   blkdev_block data structure.

   (3) If the block had to be pulled from the block device, and if the
   block is eligible to be cached (ie. it is not a bad block, has a
   read-write ratio greater than 0.8, and has an access count greater
   than 512), compare the stats of this block with the block currently
   occupying the shared slot on the cache device. If the stats of the
   block on the block device are better, bring it into the cache.
   
*/


/* ASSUMPTIONS
 *
 * We assume that the applications using the loopback deivce have a
 * block size equal to BLOCKFS_BLKSIZE.
 *
 * We assume that the argument `size' will always be a multiple of
 * BLOCKFS_BLKSIZE , ie. that the upper layers will make requests to
 * the loopback device in multiples of the basic block size.
 */


/* read_block ()

   This function reads a single block and stores it in the buffer. It
   determines whether the block is in the cache device. It also
   updates the counts, and brings blocks into the cache device if they
   are eligible
*/

static ssize_t read_block(off_t offset, char *buf, size_t size);


/* update_block()

   This function updates the read and write counts of the passed
   block. It checks to see whether the block is eligible to be moved
   into the cache, and if so, it makes the change
*/

static void update_block(off_t block_num);



/* diff_read_write_ratio ()

   Compute the difference in the read ratio and write ratio of the
   give block.
*/

static inline double diff_read_write_ratio (struct block_info *block);


/* blockfs_read ()
 * 
 * This is the entry point into the read() module. 
 */

int blockfs_read(const char *path, char *buf, size_t size, off_t offset,
		 struct fuse_file_info *fi)
{

  size_t ret = 0;
  size_t read_size = 0;

  off_t curr_offset = offset;
  off_t next_offset = offset + BLOCKFS_BLKSIZE - (offset % BLOCKFS_BLKSIZE);

  if ( strcmp(path, blockfs.path) != 0) {
    return -ENOENT;
  }
  
  while (read_size != size){

    ret += read_block (curr_offset, buf + read_size, next_offset - curr_offset);

    update_block(curr_offset);

    read_size += next_offset - curr_offset;

    curr_offset = next_offset;

    if (size - read_size > BLOCKFS_BLKSIZE) next_offset += BLOCKFS_BLKSIZE;
    else next_offset += size - read_size;    

  }

  return ret;
  
}


/* read_block () 

   block_num is the number of the block we are to process. It can be
   used as a direct index into the blkdev_block and cdev_block data
   structures.

   buf is a buffer where we copy the data to. We can write at most
   BLOCKFS_BLKSIZE bytes to this buffer 


   BIG TODO : Think long and hard about what synchronization needs to
   be done. As of now, the synchronization code is very ad-hoc, and
   that approach will simply not do. Besides, I don't even think what
   is here currently is even correct.

*/

static ssize_t read_block(off_t offset, char *buf, size_t size)
{
  ssize_t ret;


  off_t block_num = OFFSET_2_BLOCKNO(offset);

  /* Check to see if the block is cached, and if so, read it from the cache */
  if (IS_CACHED(blkdev_block[block_num].flags)){
  
    off_t cache_slot_num = CACHE_SLOT_NUM(block_num);
    off_t cache_address = BLOCKNO_2_OFFSET(cache_slot_num) + (offset % BLOCKFS_BLKSIZE);

    ret = pread (blockfs.cdev_fd, buf, size, cache_address );
    if (ret == -1) {
      perror ("in blk_read, failed to read from cache device ");
      exit(1);
    }
  }
  else{
    /* Not cached, read from block device */
    ret = pread (blockfs.blkdev_fd, buf, size, offset);
    if (ret == -1) {
      perror ("in blk_read, failed to read from block device ");
      exit(1);
    }
  }
  
  return ret;
  
}


/* update_block() 
 
   Updates the passed block by incrementing the read count by one, and
   then checking to see if it deserves to be moved into the cache or
   not.
*/

static void update_block(off_t offset)
{

  off_t block_num = OFFSET_2_BLOCKNO(offset);
  struct block_info *block = blkdev_block + block_num;

  block->read_count++;

  if (!IS_CACHED(block->flags)){
    double access_count = block->read_count + block->write_count;

    if ( access_count >= MIN_ACCESS_COUNT
	 && diff_read_write_ratio(block) >= MIN_READ_WRITE_RATIO
	 && block->write_count <= MAX_WRITE_COUNT){

      /* This block is eligible to be cached. So now we check whether
	 it indeed deserves to be cached.
      */

      off_t cache_slot_num = CACHE_SLOT_NUM(block_num);
      off_t curr_cached_block_addr;
      struct block_info *curr_cached_block;

      /* Compute the block number of the currently cached block */

      curr_cached_block_addr = cdev_block[cache_slot_num] ;
      
      curr_cached_block = blkdev_block + (curr_cached_block_addr / BLOCKFS_BLKSIZE);
      
      
      if (diff_read_write_ratio(block) >  diff_read_write_ratio(curr_cached_block)){
	/* The passed block deserves to be in the cache */
	
	/* Read it from the disk and write it to the appropriate place in the cache */

	
	char tmp[BLOCKFS_BLKSIZE];

	ssize_t ret = pread (blockfs.blkdev_fd,
			     tmp,
			     BLOCKFS_BLKSIZE,
			     BLOCKNO_2_OFFSET(block_num));
      
	if (ret == -1 || ret < BLOCKFS_BLKSIZE){
	  perror ("in update_block, error reading from the block device ");
	  exit(1);
	}
	
	ret = pwrite (blockfs.cdev_fd, 
		      tmp, 
		      BLOCKFS_BLKSIZE, 
		      BLOCKNO_2_OFFSET(cache_slot_num));

	if (ret == -1  ||  ret < BLOCKFS_BLKSIZE){
	  perror ("in update_block, error writing to cache device ");
	  exit(1);
	}

	MARK_UNCACHED(curr_cached_block->flags);
	MARK_CACHED(block->flags);
	
      }
    }

  }

}


static inline double diff_read_write_ratio (struct block_info *block){
  double access_count = block->read_count + block->write_count;
  double read_ratio = block->read_count / access_count;
  double write_ratio = block->write_count / access_count;

  return read_ratio - write_ratio;
}
  
