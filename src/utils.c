/* Utility functions */

#include <blockfs.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/fs.h>


static off_t get_size (const char *path)
{
  off_t ret;
  struct stat sbuf;
  
  int fd;
 
  if (stat (path, &sbuf) == -1){
    perror("error : in get_size, could not stat ");
    exit(1);
  }
 
  
  if (S_ISBLK(sbuf.st_mode)){
    if ( (fd = open(path, O_RDONLY)) == -1){
      perror("error : in get_size, failure in call to open() ");
      exit(1);
    }
    if (ioctl (fd, BLKGETSIZE64, &ret) == -1){
      perror ("error : in get_size, failure in call to ioctl() "); 
      exit(1);
    }
    close (fd);
  }

  else if (S_ISREG(sbuf.st_mode)){ 
    ret = sbuf.st_size; 
  } 
  else{ 
    fprintf(stderr, "in utils.c/get_size() : %s not a regular or block special file\n", path); 
    exit(1); 
  } 

  fprintf(stderr, "sizeof(%s) == %lu\n", path, ret);

  return ret; 

}


/* The following function computes and stores the size of the block
 * and cache device in number of blocks. If the size in bytes is not
 * an even multiple of BLOCKFS_BLKSIZE, then the function rounds down
 * to the nearest block number.
 */

void blockfs_calc_and_store_sizes()
{
  blockfs.cdev_size = (get_size(blockfs.cache_device)) / BLOCKFS_BLKSIZE;
  blockfs.blkdev_size = (get_size(blockfs.block_device)) / BLOCKFS_BLKSIZE;
}


void blockfs_create_maps(void)
{

  blkdev_block = (struct block_info *) calloc (blockfs.blkdev_size, sizeof(struct block_info));

  cdev_block = (off_t *) calloc (blockfs.cdev_size, sizeof(off_t));

}
