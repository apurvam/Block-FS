#include <blockfs.h>

/***** update_block  *****
 * 
 * This function updates the statistics of the passed block. The only
 * thing it does is to set the write count to be the original write
 * count + 70% of the read count.
 *
 * The effect of this adjustment is that, if this block is currently
 * cached, it is highly probable that it will be evicted from the
 * cache the next time a decision is to be made about choosing a block
 * to occupy its cache slot.
 */

static inline void update_block (struct block_info *);


/***** blockfs_write ****
 
   This function will be called by the loopback driver whenever a
   write is issued on the loopback device. The things to do, [and the
   details to understand], are:

   (0) Write the data onto the correct location on the block device.
   
   (1) Update the stats of the block in the blkdev_blocks structure.
   
   (2) If the block was in the cache, mark the cache slot as empty in
       the cdev_block structure.

   (I still need to work out how to take care of proper
   synchronisation of each of the above operations)
*/


int blockfs_write(const char *path, const char *buf, size_t size,
                     off_t offset, struct fuse_file_info *fi)
{

  off_t num_blocks = NUMBLOCKS(size);
  off_t base_block_num  = OFFSET_2_BLOCKNO(offset);

  struct block_info *b;
  int count;
  ssize_t ret;
  
  if (strcmp(path, blockfs.path) != 0) {
    return -ENOENT;
  }

  ret = pwrite (blockfs.blkdev_fd, buf, size, offset);

  if (ret == -1 || ret < size) {
    perror("in blockfs_write(), error writing to block device ");
    exit(1);
  }

  count = 0;

  while( count != num_blocks) {
    b = blkdev_block + base_block_num + count;

    update_block (b);

    ++count;
  }
  

  return ret;
}



static inline void update_block (struct block_info *b)
{
  b->write_count  +=  0.7 * b->read_count;
  
}
