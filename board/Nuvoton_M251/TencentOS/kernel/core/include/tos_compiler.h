#ifndef _TOS_COMPILER_H_
#define  _TOS_COMPILER_H_

// function with __API__ prefix, api for user
#define __API__

// function with __KERNEL__ prefix, only for kernel
#define __KERNEL__

// function with __HOOK__ prefix, should be implemented by user
#define __HOOK__

// function with __DEBUG__ prefix, only for debug
#define __DEBUG__

// function with __PORT__ is architecture depended
#define __PORT__

/*------------------ RealView Compiler -----------------*/
#if defined(__CC_ARM)

#define __ASM__             __asm
#define __VOLATILE__        volatile

#define __INLINE__          inline
#define __STATIC__          static

#if (__ARMCC_VERSION < 5060750) // how to know the exact number?
#define __STATIC_INLINE__   static
#else
#define __STATIC_INLINE__   static inline
#endif

#define likely(x)           __builtin_expect(!!(x), 1)
#define unlikely(x)         __builtin_expect(!!(x), 0)
#define __UNUSED__          __attribute__((__unused__))
#define __USED__            __attribute__((__used__))
#define __PACKED__          __attribute__((packed))
#define __ALIGNED__(x)      __attribute__((aligned(x)))
#define __PURE__            __attribute__((__pure__))
#define __CONST__           __attribute__((__const__))
#define __NO_RETURN__       __attribute__((__noreturn__))

/*------------------ ARM Compiler V6 -------------------*/
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)

#define __ASM__             __asm
#define __VOLATILE__        volatile

#define __INLINE__          inline
#define __STATIC__          static
#define __STATIC_INLINE__   static inline

#define likely(x)           __builtin_expect(!!(x), 1)
#define unlikely(x)         __builtin_expect(!!(x), 0)
#define __UNUSED__          __attribute__((__unused__))
#define __USED__            __attribute__((__used__))
#define __PACKED__          __attribute__((packed))
#define __ALIGNED__(x)      __attribute__((aligned(x)))
#define __PURE__            __attribute__((__pure__))
#define __CONST__           __attribute__((__const__))
#define __NO_RETURN__       __attribute__((__noreturn__))
#define __NAKED__           __attribute__((naked))

/*------------------ ICC Compiler ----------------------*/
#elif defined(__ICCARM__)

#define __ASM__             __asm
#define __VOLATILE__        volatile

#define __INLINE__          inline
#define __STATIC__          static
#define __STATIC_INLINE__   static inline

#define likely(x)           (x)
#define unlikely(x)         (x)
#define __UNUSED__
#define __USED__
#define __PACKED__
#define __ALIGNED__(x)
#define __PURE__
#define __CONST__
#define __NO_RETURN__
#define __NAKED__

/*------------------ GNU Compiler ----------------------*/
#elif defined(__GNUC__)

#define __ASM__             __asm
#define __VOLATILE__        volatile

#define __INLINE__          inline
#define __STATIC__          static
#define __STATIC_INLINE__   static inline

#define likely(x)           __builtin_expect(!!(x), 1)
#define unlikely(x)         __builtin_expect(!!(x), 0)
#define __UNUSED__          __attribute__((__unused__))
#define __USED__            __attribute__((__used__))
#define __PACKED__          __attribute__((packed))
#define __ALIGNED__(x)      __attribute__((aligned(x)))
#define __PURE__            __attribute__((__pure__))
#define __CONST__           __attribute__((__const__))
#define __NO_RETURN__       __attribute__((__noreturn__))
#define __NAKED__           __attribute__((naked))

#endif

#endif /* _TOS_COMPILER_H_ */

