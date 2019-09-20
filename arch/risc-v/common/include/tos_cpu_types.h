#ifndef _TOS_CPU_TYPES_H_
#define _TOS_CPU_TYPES_H_

/* CPU address type based on address bus size.          */
#if     (TOS_CFG_CPU_ADDR_SIZE == CPU_WORD_SIZE_32)
typedef uint32_t    cpu_addr_t;
#elif   (TOS_CFG_CPU_ADDR_SIZE == CPU_WORD_SIZE_16)
typedef uint16_t    cpu_addr_t;
#else
typedef uint8_t     cpu_addr_t;
#endif

/* CPU data type based on data bus size.          */
#if     (TOS_CFG_CPU_DATA_SIZE == CPU_WORD_SIZE_32)
typedef uint32_t    cpu_data_t;
#elif   (TOS_CFG_CPU_DATA_SIZE == CPU_WORD_SIZE_16)
typedef uint16_t    cpu_data_t;
#else
typedef uint8_t     cpu_data_t;
#endif

#if     (TOS_CFG_CPU_HRTIMER_EN > 0)
#if     (TOS_CFG_CPU_HRTIMER_SIZE == CPU_WORD_SIZE_08)
typedef uint8_t     cpu_hrtimer_t;
#elif   (TOS_CFG_CPU_HRTIMER_SIZE == CPU_WORD_SIZE_16)
typedef uint16_t    cpu_hrtimer_t;
#elif   (TOS_CFG_CPU_HRTIMER_SIZE == CPU_WORD_SIZE_64)
typedef uint64_t    cpu_hrtimer_t;
#else
typedef uint32_t    cpu_hrtimer_t;
#endif
#else
typedef uint32_t    cpu_hrtimer_t;
#endif

//typedef cpu_addr_t  size_t;
typedef cpu_addr_t  cpu_cpsr_t;

#endif
