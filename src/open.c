/* An implementation of  open()  for blockfs 
 *
 * Our filesystem only exports a single file. This function will be
 * called when the loopback driver tries to open this file for
 * mounting as a loopback device.
 */

#include <blockfs.h>





int blockfs_open(const char *path, struct fuse_file_info *fi)
{

  /* Check the path */
  if (strcmp(path, blockfs.path) != 0){
    return -ENOENT;
  }

  /* Since the loopback driver typically only opens files in
   * Read/Write mode, we make sure that this is the only way to open
   * our file. 
   *
   * This check will conceivably do no harm in the expected usage
   * scenario, but will warn us if something un-expected is happening.
   */

  if ((fi->flags & 3) != O_RDWR){ 
    return -EACCES;
  }

  return 0;

}

