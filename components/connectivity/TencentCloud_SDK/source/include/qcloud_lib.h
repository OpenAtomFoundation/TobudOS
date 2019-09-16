/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef _QCLOUD_LIB_H_
#define  _QCLOUD_LIB_H_

#if defined(__cplusplus)
extern "C" {
#endif

#define __QCLOUD_API__

#define __QCLOUD_INTERNAL__

#define __QCLOUD_STATIC__   static

#define __QCLOUD_OSAL__

#define QCLOUD_MAX(a,b) ((a) > (b) ? (a) : (b))
#define QCLOUD_MIN(a,b) ((a) < (b) ? (a) : (b))

#if (QCLOUD_CFG_DEVELOP_DEBUG_EN > 0u)

#define QCLOUD_FUNC_ENTRY \
	{ \
	    printf("FUNC_ENTRY:   %s L#%d \n", __FUNCTION__, __LINE__); \
	}

#define QCLOUD_FUNC_EXIT \
	{ \
    	printf("FUNC_EXIT:   %s L#%d \n", __FUNCTION__, __LINE__); \
    	return; \
	}

#define QCLOUD_FUNC_EXIT_RC(x) \
	{ \
    	printf("FUNC_EXIT:   %s L#%d Return Code : %ld \n", __FUNCTION__, __LINE__, (long)(x)); \
    	return x; \
	}

#define QCLOUD_FUNC_EXIT_RC_IF(exp, expected, x) \
    if ((exp) == (expected)) { \
        printf("FUNC_EXIT:   %s L#%d Return Code : %ld \n", __FUNCTION__, __LINE__, (long)(x)); \
        return (x); \
    }

#define QCLOUD_FUNC_EXIT_RC_IF_NOT(exp, expected, x) \
    if ((exp) != (expected)) { \
        printf("FUNC_EXIT:   %s L#%d Return Code : %ld \n", __FUNCTION__, __LINE__, (long)(x)); \
        return (x); \
    }
#else

#define QCLOUD_FUNC_ENTRY

#define QCLOUD_FUNC_EXIT \
	{ \
		return;\
	}

#define QCLOUD_FUNC_EXIT_RC(x) \
	{ \
		return x; \
	}

#define QCLOUD_FUNC_EXIT_RC_IF(exp, expected, x) \
    if ((exp) == (expected)) { \
        return (x); \
    }

#define QCLOUD_FUNC_EXIT_RC_IF_NOT(exp, expected, x) \
    if ((exp) != (expected)) { \
        return (x); \
    }
#endif

#define QCLOUD_NUMBERIC_SANITY_CHECK(num, err) \
    do { \
        if (0 == (num)) { \
            QCLOUD_LOG_E("Invalid argument, numeric 0"); \
            return (err); \
        } \
    } while(0)

#define QCLOUD_NUMBERIC_SANITY_CHECK_RTN(num) \
    do { \
        if (0 == (num)) { \
            QCLOUD_LOG_E("Invalid argument, numeric 0"); \
            return; \
        } \
    } while(0)

#define QCLOUD_POINTER_SANITY_CHECK(ptr, err) \
    do { \
        if (NULL == (ptr)) { \
            QCLOUD_LOG_E("Invalid argument, %s = %p", #ptr, ptr); \
            return (err); \
        } \
    } while(0)

#define QCLOUD_POINTER_SANITY_CHECK_RTN(ptr) \
    do { \
        if (NULL == (ptr)) { \
            QCLOUD_LOG_E("Invalid argument, %s = %p", #ptr, ptr); \
            return; \
        } \
    } while(0)

#define QCLOUD_STRING_PTR_SANITY_CHECK(ptr, err) \
    do { \
        if (NULL == (ptr)) { \
            QCLOUD_LOG_E("Invalid argument, %s = %p", #ptr, (ptr)); \
            return (err); \
        } \
        if (0 == strlen((ptr))) { \
            QCLOUD_LOG_E("Invalid argument, %s = '%s'", #ptr, (ptr)); \
            return (err); \
        } \
    } while(0)

#define QCLOUD_STRING_PTR_SANITY_CHECK_RTN(ptr) \
    do { \
        if (NULL == (ptr)) { \
            QCLOUD_LOG_E("Invalid argument, %s = %p", #ptr, (ptr)); \
            return; \
        } \
        if (0 == strlen((ptr))) { \
            QCLOUD_LOG_E("Invalid argument, %s = '%s'", #ptr, (ptr)); \
            return; \
        } \
    } while(0)

#if defined(__cplusplus)
}
#endif

#endif

