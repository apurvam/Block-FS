/* An implemention of getattr() for blockfs */

#include <blockfs.h>


int blockfs_getattr(const char *path, struct stat *stbuf)
{

  memset(stbuf, 0, sizeof(struct stat));

  if (strcmp(path, "/") == 0){
    stbuf->st_mode = S_IFDIR | 0755;
    stbuf->st_nlink = 2;
    stbuf->st_size = BLOCKFS_BLKSIZE;
    stbuf->st_blocks = 1;

  }
  else if (strcmp(path, blockfs.path) == 0){
    stbuf->st_mode = S_IFREG | 0666;
    stbuf->st_nlink = 1;
    stbuf->st_size = blockfs.blkdev_size * BLOCKFS_BLKSIZE;
    stbuf->st_blocks = blockfs.blkdev_size;
  }
  else return -ENOENT;

  fprintf(stderr, "Device size: %lu\n", stbuf->st_size / 1048576);

  stbuf->st_blksize = BLOCKFS_BLKSIZE;
  stbuf->st_uid = getuid();
  stbuf->st_gid = getgid();

  return 0;


}

