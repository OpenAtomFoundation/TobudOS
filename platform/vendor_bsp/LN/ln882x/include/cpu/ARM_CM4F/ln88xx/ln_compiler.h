#ifndef  __LN_COMPILER_H__
#define  __LN_COMPILER_H__

/*------------------ RealView Compiler -----------------*/
#if defined(__CC_ARM)

#define ARMCC_V5

#define __ASM__             __asm
#define __VOLATILE__        volatile

#define __INLINE__          inline
#define __STATIC__          static

#if (__ARMCC_VERSION < 5060750)
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
#define __WEAK__            __attribute__((weak))

/*------------------ ARM Compiler V6 -------------------*/
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)

#define ARMCC_V6

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
#define __WEAK__            __attribute__((weak))

/*------------------ ICC Compiler ----------------------*/
#elif defined(__ICCARM__)  || defined(__ICC430__) // __IAR_SYSTEMS_ICC__

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
#define __WEAK__            __weak

/*------------------ ICC Compiler for STM8/AVR ----------------------*/
#elif defined(__IAR_SYSTEMS_ICC__)

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
#define __WEAK__            __weak

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
#define __WEAK__            __attribute__((weak))

#endif

#endif /* __LN_COMPILER_H__ */

