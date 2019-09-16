#ifndef _TOS_VFS_TYPES_H_
#define  _TOS_VFS_TYPES_H_

typedef void    VFS_DIR;

typedef int32_t     vfs_off_t;
typedef uint32_t    vfs_oflag_t;

#ifndef ssize_t
#define ssize_t     int
#endif

#endif
