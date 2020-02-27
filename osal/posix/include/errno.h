/*---------------------------------------------------------------------------- * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

#ifndef _POSIX_ERRNO_H_
#define  _POSIX_ERRNO_H_

#define ENOERR              0
#define ENOERR_STR          "No error."

#define E2BIG               1
#define E2BIG_STR           "Argument list too long."

#define EACCES              2
#define EACCES_STR          "Permission denied."

#define EADDRINUSE          3
#define EADDRINUSE_STR      "Address in use."

#define EADDRNOTAVAIL       4
#define EADDRNOTAVAIL_STR   "Address not available."

#define EAFNOSUPPORT        5
#define EAFNOSUPPORT_STR    "Address family not supported."

#define EAGAIN              6
#define EAGAIN_STR          "Resource unavailable, try again (may be the same value as [EWOULDBLOCK])."

#define EALREADY            7
#define EALREADY_STR        "Connection already in progress."

#define EBADF               8
#define EBADF_STR           "Bad file descriptor."

#define EBADMSG             9
#define EBADMSG_STR         "Bad message."

#define EBUSY               10
#define EBUSY_STR           "Device or resource busy."

#define ECANCELED           11
#define ECANCELED_STR       "Operation canceled."

#define ECHILD              12
#define ECHILD_STR          "No child processes."

#define ECONNABORTED        13
#define ECONNABORTED_STR    "Connection aborted."

#define ECONNREFUSED        14
#define ECONNREFUSED_STR    "Connection refused."

#define ECONNRESET          15
#define ECONNRESET_STR      "Connection reset."

#define EDEADLK             16
#define EDEADLK_STR         "Resource deadlock would occur."

#define EDESTADDRREQ        17
#define EDESTADDRREQ_STR    "Destination address required."

#define EDOM                18
#define EDOM_STR            "Mathematics argument out of domain of function."

#define EDQUOT              19
#define EDQUOT_STR          "Reserved."

#define EEXIST              20
#define EEXIST_STR          "File exists."

#define EFAULT              21
#define EFAULT_STR          "Bad address."

#define EFBIG               22
#define EFBIG_STR           "File too large."

#define EHOSTUNREACH        23
#define EHOSTUNREACH_STR    "Host is unreachable."

#define EIDRM               24
#define EIDRM_STR           "Identifier removed."

#define EILSEQ              25
#define EILSEQ_STR          "Illegal byte sequence."

#define EINPROGRESS         26
#define EINPROGRESS_STR     "Operation in progress."

#define EINTR               27
#define EINTR_STR           "Interrupted function."

#define EINVAL              28
#define EINVAL_STR          "Invalid argument."

#define EIO                 29
#define EIO_STR             "I/O error."

#define EISCONN             30
#define EISCONN_STR         "Socket is connected."

#define EISDIR              31
#define EISDIR_STR          "Is a directory."

#define ELOOP               32
#define ELOOP_STR           "Too many levels of symbolic links."

#define EMFILE              33
#define EMFILE_STR          "File descriptor value too large."

#define EMLINK              34
#define EMLINK_STR          "Too many links."

#define EMSGSIZE            35
#define EMSGSIZE_STR        "Message too large."

#define EMULTIHOP           36
#define EMULTIHOP_STR       "Reserved."

#define ENAMETOOLONG        37
#define ENAMETOOLONG_STR    "Filename too long."

#define ENETDOWN            38
#define ENETDOWN_STR        "Network is down."

#define ENETRESET           39
#define ENETRESET_STR       "Connection aborted by network."

#define ENETUNREACH         40
#define ENETUNREACH_STR     "Network unreachable."

#define ENFILE              41
#define ENFILE_STR          "Too many files open in system."

#define ENOBUFS             42
#define ENOBUFS_STR         "No buffer space available."

#define ENODATA             43
#define ENODATA_STR         "No message is available on the STREAM head read queue. "

#define ENODEV              44
#define ENODEV_STR          "No such device."

#define ENOENT              45
#define ENOENT_STR          "No such file or directory."

#define ENOEXEC             46
#define ENOEXEC_STR         "Executable file format error."

#define ENOLCK              47
#define ENOLCK_STR          "No locks available."

#define ENOLINK             48
#define ENOLINK_STR         "Reserved."

#define ENOMEM              49
#define ENOMEM_STR          "Not enough space."

#define ENOMSG              50
#define ENOMSG_STR          "No message of the desired type."

#define ENOPROTOOPT         51
#define ENOPROTOOPT_STR     "Protocol not available."

#define ENOSPC              52
#define ENOSPC_STR          "No space left on device."

#define ENOSR               53
#define ENOSR_STR           "No STREAM resources."

#define ENOSTR              54
#define ENOSTR_STR          "Not a STREAM."

#define ENOSYS              55
#define ENOSYS_STR          "Functionality not supported."

#define ENOTCONN            56
#define ENOTCONN_STR        "The socket is not connected."

#define ENOTDIR             57
#define ENOTDIR_STR         "Not a directory or a symbolic link to a directory."

#define ENOTEMPTY           58
#define ENOTEMPTY_STR       "Directory not empty."

#define ENOTRECOVERABLE     59
#define ENOTRECOVERABLE_STR "State not recoverable."

#define ENOTSOCK            60
#define ENOTSOCK_STR        "Not a socket."

#define ENOTSUP             61
#define ENOTSUP_STR         "Not supported (may be the same value as [EOPNOTSUPP]). "

#define ENOTTY              62
#define ENOTTY_STR          "Inappropriate I/O control operation."

#define ENXIO               63
#define ENXIO_STR           "No such device or address."

#define EOPNOTSUPP          64
#define EOPNOTSUPP_STR      "Operation not supported on socket (may be the same value as [ENOTSUP])."

#define EOVERFLOW           65
#define EOVERFLOW_STR       "Value too large to be stored in data type."

#define EOWNERDEAD          66
#define EOWNERDEAD_STR      "Previous owner died."

#define EPERM               67
#define EPERM_STR           "Operation not permitted."

#define EPIPE               68
#define EPIPE_STR           "Broken pipe."

#define EPROTO              69
#define EPROTO_STR          "Protocol error."

#define EPROTONOSUPPORT     70
#define EPROTONOSUPPORT_STR "Protocol not supported."

#define EPROTOTYPE          71
#define EPROTOTYPE_STR      "Protocol wrong type for socket."

#define ERANGE              72
#define ERANGE_STR          "Result too large."

#define EROFS               73
#define EROFS_STR           "Read-only file system."

#define ESPIPE              74
#define ESPIPE_STR          "Invalid seek."

#define ESRCH               75
#define ESRCH_STR           "No such process."

#define ESTALE              76
#define ESTALE_STR          "Reserved."

#define ETIME               77
#define ETIME_STR           "Stream ioctl() timeout."

#define ETIMEDOUT           78
#define ETIMEDOUT_STR       "Connection timed out."

#define ETXTBSY             79
#define ETXTBSY_STR         "Text file busy."

#define EWOULDBLOCK         80
#define EWOULDBLOCK_STR     "Operation would block (may be the same value as [EAGAIN])."

#define EXDEV               81
#define EXDEV_STR           "Cross-device link."

#endif /* _POSIX_ERRNO_H_ */

