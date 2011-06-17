/* An implementation of readdir 
 *
 * Our filesystem exports a single file located in the root
 * directory. Thus we check to make sure that readdir is executed only
 * on the root directory, and if so, we return the name of the only
 * file to be found there (along with the obligatory .  and ..
 * files).
 */

#include <blockfs.h>


int blockfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi)
{
    (void) offset;
    (void) fi;

    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, blockfs.path + 1, NULL, 0);

    return 0;


}

