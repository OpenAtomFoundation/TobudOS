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

#include "py/mphal.h"
#include "py/mpthread.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "vfs_tos.h"
#include "unistd.h"
#if !(MP_GEN_HDR)
#include "tos_vfs.h"
#endif

const byte vfs_err_to_errno_table[] = {
    [VFS_ERR_NONE] = 0,
    [VFS_ERR_BUFFER_NULL] = MP_EINVAL,
    [VFS_ERR_DEVICE_NOT_REGISTERED] = MP_ENODEV,
    [VFS_ERR_DEVICE_ALREADY_REGISTERED] = 0, // should never be here
    [VFS_ERR_FILE_NO_AVAILABLE] = MP_EACCES,
    [VFS_ERR_FILE_NOT_OPEN] = MP_EBADF,
    [VFS_ERR_FS_ALREADY_MOUNTED] = 0, // should never be here
    [VFS_ERR_FS_ALREADY_REGISTERED] = 0, // should never be here
    [VFS_ERR_FS_NOT_REGISTERED] = MP_ENODEV,
    [VFS_ERR_FS_NOT_MOUNT] = MP_ENODEV,
    [VFS_ERR_OPS_NULL] = MP_ENODEV,
    [VFS_ERR_OPS_FAILED] = MP_ENODEV,
    [VFS_ERR_INODE_NAME_TOO_LONG] = MP_EINVAL,
    [VFS_ERR_INODE_CREATE_FAILED] = MP_ENOMEM,
    [VFS_ERR_INODE_NOT_FOUND] = MP_ENOENT,
    [VFS_ERR_INODE_INVALID] = MP_ENOENT,
    [VFS_ERR_INODE_BUSY] = MP_EBUSY,
    [VFS_ERR_INODE_INAVALIABLE] = MP_ENOENT,
    [VFS_ERR_OPEN_DIR] = MP_EISDIR, // not used
    [VFS_ERR_OUT_OF_MEMORY] = MP_ENOMEM,
    [VFS_ERR_PARA_INVALID] = MP_EINVAL,
    [VFS_ERR_PATH_TOO_LONG] = MP_EINVAL,
};

typedef struct _mp_obj_vfs_tos_file_t {
    mp_obj_base_t base;
    int fd;
} mp_obj_vfs_tos_file_t;

#if MICROPY_CPYTHON_COMPAT
STATIC void check_fd_is_open(const mp_obj_vfs_tos_file_t *o) {
    if (o->fd < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("I/O operation on closed file"));
    }
}
#else
#define check_fd_is_open(o)
#endif

STATIC void vfs_tos_file_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_vfs_tos_file_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<io.%s %d>", mp_obj_get_type_str(self_in), self->fd);
}

mp_obj_t mp_vfs_tos_file_open(const mp_obj_type_t *type, mp_obj_t file_in, mp_obj_t mode_in) {
    // [REF] vfs_posix_file.c > mp_vfs_posix_file_open
    // [REF] vfs_fat_file.c > mp_vfs_fat_file_open
    mp_obj_vfs_tos_file_t *o = m_new_obj(mp_obj_vfs_tos_file_t);
    const char *mode_s = mp_obj_str_get_str(mode_in);

    int mode = 0;
    while (*mode_s) {
        switch (*mode_s++) {
            case 'r':
                mode |= VFS_OFLAG_READ;
                break;
            case 'w':
                mode |= VFS_OFLAG_WRITE | VFS_OFLAG_CREATE_ALWAYS;
                break;
            case 'x':
                mode |= VFS_OFLAG_WRITE | VFS_OFLAG_CREATE_NEW;
                break;
            case 'a':
                mode |= VFS_OFLAG_WRITE | VFS_OFLAG_OPEN_ALWAYS; // FIXME: VFS_OFLAG_OPEN_APPEND ?
                break;
            case '+':
                mode |= VFS_OFLAG_READ | VFS_OFLAG_WRITE;
                break;
            #if MICROPY_PY_IO_FILEIO
            // If we don't have io.FileIO, then files are in text mode implicitly
            case 'b':
                type = &mp_type_vfs_tos_fileio;
                break;
            case 't':
                type = &mp_type_vfs_tos_textio;
                break;
            #endif /* MICROPY_PY_IO_FILEIO */
        }
    }

    o->base.type = type;

    mp_obj_t fid = file_in;

    if (mp_obj_is_small_int(fid)) {
        o->fd = MP_OBJ_SMALL_INT_VALUE(fid);
        return MP_OBJ_FROM_PTR(o);
    }

    const char *fname = mp_obj_str_get_str(fid);
    int r = tos_vfs_open(fname, mode);
    if (r < 0) {
        mp_raise_OSError(vfs_err_to_errno_table[-r]);
    }
    o->fd = r;
    return MP_OBJ_FROM_PTR(o);
}

STATIC mp_obj_t vfs_tos_file_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // [REF] vfs_posix_file.c > vfs_posix_file_make_new
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_file, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_mode, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_QSTR(MP_QSTR_r)} },
    };

    mp_arg_val_t arg_vals[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, args, MP_ARRAY_SIZE(allowed_args), allowed_args, arg_vals);
    return mp_vfs_tos_file_open(type, arg_vals[0].u_obj, arg_vals[1].u_obj);
}

STATIC mp_obj_t vfs_tos_file_fileno(mp_obj_t self_in) {
    mp_obj_vfs_tos_file_t *self = MP_OBJ_TO_PTR(self_in);
    check_fd_is_open(self);
    return MP_OBJ_NEW_SMALL_INT(self->fd);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(vfs_tos_file_fileno_obj, vfs_tos_file_fileno);

STATIC mp_obj_t vfs_tos_file___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    return mp_stream_close(args[0]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(vfs_tos_file___exit___obj, 4, 4, vfs_tos_file___exit__);

STATIC mp_uint_t vfs_tos_file_read(mp_obj_t o_in, void *buf, mp_uint_t size, int *errcode) {
    // [REF] vfs_posix_file.c > vfs_posix_file_read
    mp_obj_vfs_tos_file_t *o = MP_OBJ_TO_PTR(o_in);
    check_fd_is_open(o);
    int r = tos_vfs_read(o->fd, buf, size);
    if (r < 0) {
        *errcode = vfs_err_to_errno_table[-r];
        return MP_STREAM_ERROR;
    }
    return (mp_uint_t)r;
}

STATIC mp_uint_t vfs_tos_file_write(mp_obj_t o_in, const void *buf, mp_uint_t size, int *errcode) {
    // [REF] vfs_posix_file.c > vfs_posix_file_write
    mp_obj_vfs_tos_file_t *o = MP_OBJ_TO_PTR(o_in);
    check_fd_is_open(o);
    #if MICROPY_PY_OS_DUPTERM
    // FIXME: not supported
    if (o->fd <= STDERR_FILENO) {
        mp_hal_stdout_tx_strn(buf, size);
        return size;
    }
    #endif /* MICROPY_PY_OS_DUPTERM */
    int r = tos_vfs_write(o->fd, buf, size);
    if (r < 0) {
        *errcode = vfs_err_to_errno_table[-r];
        return MP_STREAM_ERROR;
    }
    return (mp_uint_t)r;
}

STATIC mp_uint_t vfs_tos_file_ioctl(mp_obj_t o_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    // [REF] vfs_posix_file.c > vfs_posix_file_ioctl
    mp_obj_vfs_tos_file_t *o = MP_OBJ_TO_PTR(o_in);

    if (request != MP_STREAM_CLOSE) {
        check_fd_is_open(o);
    }

    switch (request) {
        case MP_STREAM_FLUSH: {
            int r = tos_vfs_sync(o->fd);
            if (r < 0) {
                *errcode = vfs_err_to_errno_table[-r];
                return MP_STREAM_ERROR;
            }
            return 0;
        }
        case MP_STREAM_SEEK: {
            struct mp_stream_seek_t *s = (struct mp_stream_seek_t *)arg;
            int r = tos_vfs_lseek(o->fd, s->offset, s->whence);
            if (r < 0) {
                *errcode = vfs_err_to_errno_table[-r];
                return MP_STREAM_ERROR;
            }
            s->offset = r;
            return 0;
        }
        case MP_STREAM_CLOSE: {
            int r = tos_vfs_close(o->fd);
            if (r < 0) {
                *errcode = vfs_err_to_errno_table[-r];
                return MP_STREAM_ERROR;
            }
            #if MICROPY_CPYTHON_COMPAT
            o->fd = -1;
            #endif
            return 0;
        }
        case MP_STREAM_GET_FILENO:
            return o->fd;
        #if MICROPY_PY_USELECT
        case MP_STREAM_POLL: {
            mp_raise_NotImplementedError(MP_ERROR_TEXT("poll on file not available on TencentOS-tiny"));
        }
        #endif
        default:
            *errcode = MP_EINVAL;
            return MP_STREAM_ERROR;
    }
}

STATIC const mp_rom_map_elem_t vfs_tos_rawfile_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_fileno), MP_ROM_PTR(&vfs_tos_file_fileno_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_readlines), MP_ROM_PTR(&mp_stream_unbuffered_readlines_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_seek), MP_ROM_PTR(&mp_stream_seek_obj) },
    { MP_ROM_QSTR(MP_QSTR_tell), MP_ROM_PTR(&mp_stream_tell_obj) },
    { MP_ROM_QSTR(MP_QSTR_flush), MP_ROM_PTR(&mp_stream_flush_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_stream_close_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&vfs_tos_file___exit___obj) },
};

STATIC MP_DEFINE_CONST_DICT(vfs_tos_rawfile_locals_dict, vfs_tos_rawfile_locals_dict_table);

#if MICROPY_PY_IO_FILEIO
STATIC const mp_stream_p_t vfs_tos_fileio_stream_p = {
    .read = vfs_tos_file_read,
    .write = vfs_tos_file_write,
    .ioctl = vfs_tos_file_ioctl,
};

const mp_obj_type_t mp_type_vfs_tos_fileio = {
    { &mp_type_type },
    .name = MP_QSTR_FileIO,
    .print = vfs_tos_file_print,
    .make_new = vfs_tos_file_make_new,
    .getiter = mp_identity_getiter,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &vfs_tos_fileio_stream_p,
    .locals_dict = (mp_obj_dict_t *)&vfs_tos_rawfile_locals_dict,
};
#endif /* MICROPY_PY_IO_FILEIO */

STATIC const mp_stream_p_t vfs_tos_textio_stream_p = {
    .read = vfs_tos_file_read,
    .write = vfs_tos_file_write,
    .ioctl = vfs_tos_file_ioctl,
    .is_text = true,
};

const mp_obj_type_t mp_type_vfs_tos_textio = {
    {&mp_type_type},
    .name = MP_QSTR_TextIOWrapper,
    .print = vfs_tos_file_print,
    .make_new = vfs_tos_file_make_new,
    .getiter = mp_identity_getiter,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &vfs_tos_textio_stream_p,
    .locals_dict = (mp_obj_dict_t *)&vfs_tos_rawfile_locals_dict,
};

// Note: buffering and encoding args are currently ignored
mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_file, ARG_mode, ARG_encoding };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_file, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_mode, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_QSTR(MP_QSTR_r)} },
        { MP_QSTR_buffering, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_encoding, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // If the file is an integer then delegate straight to the POSIX handler
    return mp_vfs_tos_file_open(&mp_type_textio, args[ARG_file].u_obj, args[ARG_mode].u_obj);
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

#endif /* MICROPY_VFS_TOS */
