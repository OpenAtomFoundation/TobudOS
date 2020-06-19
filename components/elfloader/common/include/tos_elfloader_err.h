/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
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

#ifndef _TOS_ELFLOADER_ERR_H_
#define  _TOS_ELFLOADER_ERR_H_

typedef enum elfloader_err_en {
    ELFLOADER_ERR_NONE,
    ELFLOADER_ERR_FD_READ_FAILED,
    ELFLOADER_ERR_HEADER_INVALID,
    ELFLOADER_ERR_NO_DYN,
    ELFLOADER_ERR_NO_SYMTAB,
    ELFLOADER_ERR_NO_STRTAB,
    ELFLOADER_ERR_NO_TEXT,
    ELFLOADER_ERR_NO_LOAD_SEGMENTS,
    ELFLOADER_ERR_OUT_OF_MEMORY,
    ELFLOADER_ERR_RELOCATE_FAILED,
    ELFLOADER_ERR_PTR_NULL,
    ELFLOADER_ERR_SECTION_NOT_FOUND,
    ELFLOADER_ERR_SYM_NOT_FOUND,
    ELFLOADER_ERR_TYPE_INVALID,
} el_err_t;

#endif /* _TOS_ELFLOADER_ERR_H_ */

