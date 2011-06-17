/* blockfs : A technology for Dynamic Storage Tiering to be used in
   conjuction with loopback devices on Linux.
*/

#include <blockfs.h>
#include <fuse_opt.h>

 
/* The object describing the FUSE operations we implement */

static struct fuse_operations blockfs_oper = {
    .getattr	= blockfs_getattr,
    .readdir	= blockfs_readdir,
    .open	= blockfs_open,
    .read	= blockfs_read,
    .write	= blockfs_write,
};



/* A macro and a data structure for using the fuse command line
   parsing interface */

#define BLOCKFS_OPT(t, p, v) { t, offsetof(struct blockfs, p), v }

struct fuse_opt blockfs_opts[] = {
  BLOCKFS_OPT("-c %s", cache_device, 0),
  BLOCKFS_OPT("-b %s", block_device, 0),
};






/* The object which holds all the global information of our
   filesystem */

struct blockfs blockfs;


/* The objects which maintain information about the blocks on the
   block device and cache device respectively */

struct block_info *blkdev_block;

off_t *cdev_block;



int main(int argc, char *argv[])
{

  /* Parse the command line and extract the names of the block devices
     we will be working with. */

  struct fuse_args args = FUSE_ARGS_INIT(argc, argv);


  if ( fuse_opt_parse(&args, &blockfs, blockfs_opts, 0) == -1 ){
    printf ("failure in fuse_opt_parse\n");
    exit(1);
  }

  printf ("The cache device is : %s\n", blockfs.cache_device);
  printf ("The block device is : %s\n", blockfs.block_device);

  

  /* Perform the necessary initializations */
  blockfs_init();

  umask(0);

  return fuse_main(args.argc, args.argv, &blockfs_oper, NULL);
}
