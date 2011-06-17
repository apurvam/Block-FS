/* The initialization functions */

#include <blockfs.h>


/* The following functions are defined in utils.c */

void blockfs_create_maps();

void blockfs_calc_and_store_sizes();



void blockfs_init()
{

  /* Calculate the sizes of the block device and cache and store them
     in the blockfs structure */

  blockfs_calc_and_store_sizes();



  /* Store path of the file we export */
  strcpy(blockfs.path, "/blockfs");



  /* Create the data structures mapping blocks on the block device to
     blocks on the cache device and vice versa */

  blockfs_create_maps();



  /* Open the underlying devices */

  blockfs.cdev_fd = open (blockfs.cache_device, O_RDWR);
  if (blockfs.cdev_fd == -1){
    perror("Error opening cache device ");
    exit(1);
  }
  
  blockfs.blkdev_fd = open (blockfs.block_device, O_RDWR);
  if (blockfs.blkdev_fd == -1){
    perror("Error opening the block device ");
    exit(1);
  }

}


