#ifndef __BLOCKFS_LOCKS_H__
#define __BLOCKFS_LOCKS_H__

/* Our synchronization primitives 
 *
 * The argument to these macros is a reference to some memory
 * location. These macros assume the data at the first (least
 * significant) bit of the memory referenced by the argument to be the
 * locking bit. If the bit is set, it the blockfs_lock macro will busy
 * wait until it is reset again. The blockfs_unlock macro will reset
 * the said bit. For example, suppose we have the declaration
 *
 *    int x = 0; 
 *
 * Then the following call will return immediately:
 *
 *    blockfs_lock(x);
 *
 * On the other hand, the same call will busy wait if the declaration
 * was:
 *
 *    int x = 1;
 *
 */
 
#define blockfs_lock(addr)               \
       __asm__ ("0:\n\t"                 \
		"lock btsl %1, %0\n\t"   \
		"jc 0b"                  \
		: "=m" (addr)            \
		: "Jr" (0)               \
		: "cc"                   \
		);


#define blockfs_unlock(addr)             \
       __asm__ ( "lock btrl %1, %0"      \
		: "=m" (addr)            \
		: "Jr" (0)               \
		: "cc"                   \
		);


#endif /* __BLOCKFS_LOCKS_H__ */
