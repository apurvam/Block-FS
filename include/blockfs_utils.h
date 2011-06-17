/* blockfs_utils.h
 *
 * Useful macros and functions which may be used in blockfs.
 *
 */

#ifndef __BLOCKFS_UTILS_H__
#define __BLOCKFS_UTILS_H__


#define BLOCKFS_CACHE_BIT 0x2

#define MARK_CACHED(flag)   (flag  =  (flag | BLOCKFS_CACHE_BIT) )

#define MARK_UNCACHED(flag) (flag  =  (flag & (~BLOCKFS_CACHE_BIT) ))

#define IS_CACHED(flag)  (flag & BLOCKFS_CACHE_BIT)
       


#endif /* __BLOCKFS_UTILS_H__ */
