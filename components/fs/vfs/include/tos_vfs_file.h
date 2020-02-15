#ifndef _TOS_VFS_FILE_H_
#define  _TOS_VFS_FILE_H_

#if TOS_CFG_VFS_EN > 0u

typedef struct vfs_inode_st vfs_inode_t;

// why do you open so many files in a IOT system?
#define VFS_FILE_OPEN_MAX               20

// why your file path so deep?
#define VFS_PATH_MAX                    31

// open flags(vfs_oflag_t): open method flags (3rd argument of tos_vfs_open)
#define VFS_OFLAG_READ          0x01
#define VFS_OFLAG_WRITE         0x02
#define	VFS_OFLAG_EXISTING      0x00
#define VFS_OFLAG_CREATE_NEW    0x04
#define VFS_OFLAG_CREATE_ALWAYS 0x08
#define VFS_OFLAG_OPEN_ALWAYS   0x10
#define VFS_OFLAG_OPEN_APPEND   0x30

typedef enum vfs_whence_en {
    VFS_SEEK_SET,   /* the offset is set to offset bytes */
    VFS_SEEK_CUR,   /* the offset is set to its current location plus offset bytes */
    VFS_SEEK_END,   /* the offset is set to the size of the file plus offset bytes */
} vfs_whence_t;

/* type of a directory entry
   > ls
   d .
   d ..
   f .bashrc
   f .profile
   d Desktop
 */
typedef enum vfs_type_en {
    VFS_TYPE_FILE,
    VFS_TYPE_DIRECTORY,
    VFS_TYPE_OTHER,
} vfs_type_t;

typedef enum vfs_mode_en {
    VFS_MODE_READ_ONLY,
    VFS_MODE_WRITE_ONLY,
    VFS_MODE_READ_WRITE,
} vfs_mode_t;

typedef struct vfs_file_stat_st {
    vfs_type_t  type;       /*file type */
    vfs_mode_t  mode;       /* access mode */
    size_t      size;       /* size of file/directory, in bytes */
    uint32_t    blk_size;   /* block size used for filesystem I/O */
    uint32_t    blk_num;    /* number of blocks allocated */
    uint32_t    atime;      /* time of last access */
    uint32_t    mtime;      /* time of last modification */
    uint32_t    ctime;      /* time of last status change */
} vfs_fstat_t;

typedef struct vfs_fs_stat_st {
    uint32_t    type;       /* type of filesystem */
    size_t      name_len;   /* maximum length of filenames */
    uint32_t    blk_size;   /* optimal block size for transfers */
    uint32_t    blk_num;    /* total data blocks in the file system of this size */
    uint32_t    blk_free;   /* free blocks in the file system */
    uint32_t    blk_avail;  /* free blocks avail to non-superuser */
    uint32_t    file_num;   /* total file nodes in the file system */
    uint32_t    file_free;  /* free file nodes in the file system */
} vfs_fsstat_t;

typedef struct vfs_directory_entry_st {
    vfs_type_t          type;   /* entry type */
    size_t              size;   /* size */
    uint32_t            date;   /* modified date */
    uint32_t            time;   /* modified time */
    char                name[VFS_PATH_MAX];
} vfs_dirent_t;

typedef struct vfs_directory_st {
    vfs_dirent_t    dirent;
    vfs_off_t       pos; /*< read offset */
    vfs_inode_t    *inode;
    void           *private; /*< usually used to hook the real dirent struct of the certain filesystem */
} vfs_dir_t;

typedef struct vfs_file_st {
    int             flags;
    vfs_off_t       pos; /*< read offset */
    vfs_inode_t    *inode;
    void           *private; /*< usually used to hook the real file struct of the certain filesystem */
} vfs_file_t;

__KNL__ vfs_file_t *vfs_fd2file(int fd);

__KNL__ int vfs_file2fd(vfs_file_t *file);

__KNL__ vfs_file_t *vfs_file_alloc(void);

__KNL__ void vfs_file_free(vfs_file_t *file);

__KNL__ vfs_file_t *vfs_file_dup(vfs_file_t *file);

__KNL__ vfs_dir_t *vfs_dir_alloc(void);

__KNL__ void vfs_dir_free(vfs_dir_t *dir);

#endif

#endif /* _TOS_VFS_FILE_H_ */
