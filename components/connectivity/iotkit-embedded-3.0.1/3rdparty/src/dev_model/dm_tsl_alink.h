/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#if defined(DEPRECATED_LINKKIT)
    #ifndef _DM_TSL_ALINK_H_
        #define _DM_TSL_ALINK_H_

        /**
        * @brief Create TSL struct from TSL string.
        *        This function used to parse TSL string into TSL struct.
        *
        * @param tsl. The TSL string in JSON format.
        * @param tsl_len. The length of tsl
        * @param shadow. The pointer of TSL Struct pointer, will be malloc memory.
        *                This memory should be free by dm_shw_destroy.
        *
        * @return success or fail.
        *
        */
        int dm_tsl_alink_create(_IN_ const char *tsl, _IN_ int tsl_len, _OU_ dm_shw_t **shadow);

    #endif
#endif