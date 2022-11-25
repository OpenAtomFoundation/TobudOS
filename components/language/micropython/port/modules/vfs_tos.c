/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 KY-zhang-X
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/mpconfig.h"

#if MICROPY_VFS_TOS

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/mpthread.h"
#include "extmod/vfs.h"
#include "vfs_tos.h"
#if !(MP_GEN_HDR)
#include "tos_vfs.h"
#endif

#include <stdio.h>
#include <string.h>

typedef struct _mp_obj_vfs_tos_t {
    mp_obj_base_t base;
    vstr_t root;
    size_t root_len;
    bool readonly;
} mp_obj_vfs_tos_t;

// It's a nice trick for reducing memory reallocation to add `path` after `self->root` directly rather create a new vstr.
// But it's not so useful on repeated calling (e.g. rename).
STATIC const char *vfs_tos_get_path_str(mp_obj_vfs_tos_t *self, mp_obj_t path) {
    if (self->root_len == 0) {
        return mp_obj_str_get_str(path);
    } else {
        self->root.len = self->root_len;
        vstr_add_str(&self->root, mp_obj_str_get_str(path));
        return vstr_null_terminated_str(&self->root);
    }
}

STATIC mp_obj_t vfs_tos_get_path_obj(mp_obj_vfs_tos_t *self, mp_obj_t path) {
    if (self-> root_len == 0) {
        return path;
    } else {
        self->root.len = self->root_len;
        vstr_add_str(&self->root, mp_obj_str_get_str(path));
        return mp_obj_new_str(self->root.buf, self->root.len);
    }
}

STATIC mp_import_stat_t mp_vfs_tos_import_stat(void *self_in, const char *path) {
    mp_obj_vfs_tos_t *self = self_in;
    if (self->root_len != 0) {
        self->root.len = self->root_len;
        vstr_add_str(&self->root, path);
        path = vstr_null_terminated_str(&self->root);
    }
    vfs_fstat_t st;
    int ret = tos_vfs_stat(path, &st);
    if (ret == 0) {
        if (st.type == VFS_TYPE_DIRECTORY) {
            return MP_IMPORT_STAT_DIR;
        } else if (st.type == VFS_TYPE_FILE) {
            return MP_IMPORT_STAT_FILE;
        }
    }
    return MP_IMPORT_STAT_NO_EXIST;
}

STATIC mp_obj_t vfs_tos_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);

    mp_obj_vfs_tos_t *vfs = mp_obj_malloc(mp_obj_vfs_tos_t, type);
    vstr_init(&vfs->root, 0);
    if (n_args == 1) {
        vstr_add_str(&vfs->root, mp_obj_str_get_str(args[0]));
        vstr_add_char(&vfs->root, '/');
    }
    vfs->root_len = vfs->root.len;
    vfs->readonly = false;

    return MP_OBJ_FROM_PTR(vfs);
}

STATIC mp_obj_t vfs_tos_mount(mp_obj_t self_in, mp_obj_t readonly, mp_obj_t mkfs) {
    mp_obj_vfs_tos_t *self = MP_OBJ_TO_PTR(self_in);
    if (mp_obj_is_true(readonly)) {
        self->readonly = true;
    }
    if (mp_obj_is_true(mkfs)) {
        mp_raise_OSError(MP_EPERM);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(vfs_tos_mount_obj, vfs_tos_mount);

STATIC mp_obj_t vfs_tos_umount(mp_obj_t self_in) {
    (void)self_in;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(vfs_tos_umount_obj, vfs_tos_umount);

STATIC mp_obj_t vfs_tos_open(mp_obj_t self_in, mp_obj_t path_in, mp_obj_t mode_in) {
    mp_obj_vfs_tos_t *self = MP_OBJ_TO_PTR(self_in);
    const char *mode = mp_obj_str_get_str(mode_in);
    if (self->readonly
        && (strchr(mode, 'w') != NULL || strchr(mode, 'a') != NULL || strchr(mode, '+') != NULL)) {
        mp_raise_OSError(MP_EROFS);
    }
    if (!mp_obj_is_small_int(path_in)) {
        path_in = vfs_tos_get_path_obj(self, path_in);
    }
    return mp_vfs_tos_file_open(&mp_type_textio, path_in, mode_in);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(vfs_tos_open_obj, vfs_tos_open);

STATIC mp_obj_t vfs_tos_chdir(mp_obj_t self_in, mp_obj_t path_in) {
    // FIXME chdir() is not supported by tos
    mp_raise_OSError(MP_EPERM);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(vfs_tos_chdir_obj, vfs_tos_chdir);

STATIC mp_obj_t vfs_tos_getcwd(mp_obj_t self_in) {
    // FIXME getcwd() is not supported by tos
    return mp_obj_new_str("/", 1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(vfs_tos_getcwd_obj, vfs_tos_getcwd);

typedef struct _vfs_tos_ilistdir_it_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    bool is_str;
    VFS_DIR *dir;
} vfs_tos_ilistdir_it_t;

STATIC mp_obj_t vfs_tos_ilistdir_it_iternext(mp_obj_t self_in) {
    vfs_tos_ilistdir_it_t *self = MP_OBJ_FROM_PTR(self_in);

    if (self->dir == NULL) {
        return MP_OBJ_STOP_ITERATION;
    }

    for (;;) {
        vfs_dirent_t *dirent = tos_vfs_readdir(self->dir);
        if (dirent == NULL) {
            tos_vfs_closedir(self->dir);
            self->dir = NULL;
            return MP_OBJ_STOP_ITERATION;
        }
        const char *fn = dirent->name;

        if (fn[0] == '.' && (fn[1] == 0 || fn[1] == '.')) {
            // skip . and ..
            continue;
        }

        // make 3-tuple with info about this entry
        mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(3, NULL));

        if (self->is_str) {
            t->items[0] = mp_obj_new_str(fn, strlen(fn));
        } else {
            t->items[0] = mp_obj_new_bytes((const byte *)fn, strlen(fn));
        }

        if (dirent->type == VFS_TYPE_DIRECTORY) {
            t->items[1] = MP_OBJ_NEW_SMALL_INT(MP_S_IFDIR);
        } else if (dirent->type == VFS_TYPE_FILE) {
            t->items[1] = MP_OBJ_NEW_SMALL_INT(MP_S_IFREG);
        } else {
            // unknown type should be 0
            t->items[1] = MP_OBJ_NEW_SMALL_INT(0);
        }

        // ino is not supported by tos
        t->items[2] = MP_OBJ_NEW_SMALL_INT(0);

        return MP_OBJ_FROM_PTR(t);
    }
}

STATIC mp_obj_t vfs_tos_ilistdir(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_tos_t *self = MP_OBJ_TO_PTR(self_in);
    vfs_tos_ilistdir_it_t *iter = mp_obj_malloc(vfs_tos_ilistdir_it_t, &mp_type_polymorph_iter);
    iter->iternext = vfs_tos_ilistdir_it_iternext;
    iter->is_str = mp_obj_get_type(path_in) == &mp_type_str;
    const char *path = vfs_tos_get_path_str(self, path_in);
    if (path[0] == '\0') {
        path = ".";
    }
    iter->dir = tos_vfs_opendir(path);
    if (iter->dir == NULL) {
        // mp_raise_OSError(errno)
        return mp_const_none;
    }
    return MP_OBJ_FROM_PTR(iter);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(vfs_tos_ilistdir_obj, vfs_tos_ilistdir);

STATIC mp_obj_t vfs_tos_mkdir(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_tos_t *self = MP_OBJ_TO_PTR(self_in);
    const char *path = vfs_tos_get_path_str(self, path_in);
    int ret = tos_vfs_mkdir(path);
    if (ret != 0) {
        mp_raise_OSError(vfs_err_to_errno_table[-ret]);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(vfs_tos_mkdir_obj, vfs_tos_mkdir);

STATIC mp_obj_t vfs_tos_remove(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_tos_t *self = MP_OBJ_TO_PTR(self_in);
    const char *path = vfs_tos_get_path_str(self, path_in);
    int ret = tos_vfs_unlink(path);
    if (ret != 0) {
        mp_raise_OSError(vfs_err_to_errno_table[-ret]);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(vfs_tos_remove_obj, vfs_tos_remove);

STATIC mp_obj_t vfs_tos_rename(mp_obj_t self_in, mp_obj_t old_path_in, mp_obj_t new_path_in) {
    mp_obj_vfs_tos_t *self = MP_OBJ_TO_PTR(self_in);
    const char *old_path = vfs_tos_get_path_str(self, old_path_in);
    // const char *new_path = vfs_tos_get_path_str(self, new_path_in);
    const char *new_path = mp_obj_str_get_str(new_path_in);
    vstr_t *new_path_vstr = NULL;
    if (self->root_len != 0) {
        int new_path_len = strlen(new_path);
        new_path_vstr = vstr_new(self->root_len + new_path_len);
        vstr_add_strn(new_path_vstr, self->root.buf, self->root_len);
        vstr_add_strn(new_path_vstr, new_path, new_path_len);
        new_path = vstr_null_terminated_str(new_path_vstr);
    }

    int ret = tos_vfs_rename(old_path, new_path);
    if (new_path_vstr) { vstr_free(new_path_vstr); }
    if (ret != 0) {
        mp_raise_OSError(vfs_err_to_errno_table[-ret]);
    }
    return mp_const_none;
    
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(vfs_tos_rename_obj, vfs_tos_rename);

STATIC mp_obj_t vfs_tos_rmdir(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_tos_t *self = MP_OBJ_TO_PTR(self_in);
    const char *path = vfs_tos_get_path_str(self, path_in);
    // FIXME: rmdir is not supported by fatfs
    int ret = tos_vfs_rmdir(path);
    if (ret != 0) {
        mp_raise_OSError(vfs_err_to_errno_table[-ret]);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(vfs_tos_rmdir_obj, vfs_tos_rmdir);

STATIC mp_obj_t vfs_tos_stat(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_tos_t *self = MP_OBJ_TO_PTR(self_in);
    vfs_fstat_t st;
    const char *path = vfs_tos_get_path_str(self, path_in);
    int ret = tos_vfs_stat(path, &st);
    if (ret != 0) {
        mp_raise_OSError(vfs_err_to_errno_table[-ret]);
    }
    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(10, NULL));
    t->items[0] = MP_OBJ_NEW_SMALL_INT(st.mode);
    t->items[1] = mp_obj_new_int_from_uint(0); // st_ino
    t->items[2] = mp_obj_new_int_from_uint(0); // st_dev
    t->items[3] = mp_obj_new_int_from_uint(0); // st_nlink
    t->items[4] = mp_obj_new_int_from_uint(0); // st_uid
    t->items[5] = mp_obj_new_int_from_uint(0); // st_gid
    t->items[6] = mp_obj_new_int_from_uint(st.size); // st_size
    t->items[7] = mp_obj_new_int_from_uint(st.atime); // st_atime
    t->items[8] = mp_obj_new_int_from_uint(st.mtime); // st_mtime
    t->items[9] = mp_obj_new_int_from_uint(st.ctime); // st_ctime
    return MP_OBJ_FROM_PTR(t);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(vfs_tos_stat_obj, vfs_tos_stat);

#if MICROPY_PY_UOS_STATVFS
STATIC mp_obj_t vfs_tos_statvfs(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_tos_t *self = MP_OBJ_TO_PTR(self_in);
    vfs_fsstat_t st;
    const char *path = vfs_tos_get_path_str(self, path_in);
    int ret = tos_vfs_statfs(path, &st);
    if (ret != 0) {
        mp_raise_OSError(vfs_err_to_errno_table[-ret]);
    }
    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(10, NULL));
    t->items[0] = MP_OBJ_NEW_SMALL_INT(st.blk_size); // f_bsize
    t->items[1] = MP_OBJ_NEW_SMALL_INT(st.blk_size); // f_frsize
    t->items[2] = MP_OBJ_NEW_SMALL_INT(st.blk_num); // f_blocks
    t->items[3] = MP_OBJ_NEW_SMALL_INT(st.blk_free); // f_bfree
    t->items[4] = MP_OBJ_NEW_SMALL_INT(st.blk_avail); // f_bavail
    t->items[5] = MP_OBJ_NEW_SMALL_INT(st.file_num); // f_files
    t->items[6] = MP_OBJ_NEW_SMALL_INT(st.file_free); // f_ffree
    t->items[7] = MP_OBJ_NEW_SMALL_INT(st.file_free); // f_favail
    t->items[8] = MP_OBJ_NEW_SMALL_INT(0); // f_flags
    t->items[9] = MP_OBJ_NEW_SMALL_INT(st.name_len); // f_namemax
    return MP_OBJ_FROM_PTR(t);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(vfs_tos_statvfs_obj, vfs_tos_statvfs);


#endif /* MICROPY_PY_UOS_STATVFS */

STATIC const mp_rom_map_elem_t vfs_tos_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_mount), MP_ROM_PTR(&vfs_tos_mount_obj) },
    { MP_ROM_QSTR(MP_QSTR_umount), MP_ROM_PTR(&vfs_tos_umount_obj) },
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&vfs_tos_open_obj) },

    { MP_ROM_QSTR(MP_QSTR_chdir), MP_ROM_PTR(&vfs_tos_chdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_getcwd), MP_ROM_PTR(&vfs_tos_getcwd_obj) },
    { MP_ROM_QSTR(MP_QSTR_ilistdir), MP_ROM_PTR(&vfs_tos_ilistdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_mkdir), MP_ROM_PTR(&vfs_tos_mkdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove), MP_ROM_PTR(&vfs_tos_remove_obj) },
    { MP_ROM_QSTR(MP_QSTR_rename), MP_ROM_PTR(&vfs_tos_rename_obj) },
    { MP_ROM_QSTR(MP_QSTR_rmdir), MP_ROM_PTR(&vfs_tos_rmdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_stat), MP_ROM_PTR(&vfs_tos_stat_obj) },
    #if MICROPY_PY_UOS_STATVFS
    { MP_ROM_QSTR(MP_QSTR_statvfs), MP_ROM_PTR(&vfs_tos_statvfs_obj) },
    #endif
};
STATIC MP_DEFINE_CONST_DICT(vfs_tos_locals_dict, vfs_tos_locals_dict_table);

STATIC const mp_vfs_proto_t vfs_tos_proto = {
    .import_stat = mp_vfs_tos_import_stat,
};

const mp_obj_type_t mp_type_vfs_tos = {
    { &mp_type_type },
    .name = MP_QSTR_VfsTos,
    .make_new = vfs_tos_make_new,
    .protocol = &vfs_tos_proto,
    .locals_dict = (mp_obj_dict_t *)&vfs_tos_locals_dict,
};

#endif /* MICROPY_VFS_TOS */
