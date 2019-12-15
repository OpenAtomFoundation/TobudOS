#ifndef _TOS_CPU_TYPES_H_
#define  _TOS_CPU_TYPES_H_

#if( __DATA_MODEL__ == __DATA_MODEL_SMALL__ )
/* CPU data type based on data bus size.          */
typedef uint16_t    cpu_data_t;
/* CPU address type based on address bus size.          */
typedef uint16_t    cpu_addr_t;
#else
/* CPU data type based on data bus size.          */
typedef uint32_t    cpu_data_t;
/* CPU address type based on address bus size.          */
typedef uint32_t    cpu_addr_t;
#endif

//typedef cpu_addr_t  size_t;
typedef cpu_addr_t  cpu_cpsr_t;

#endif

