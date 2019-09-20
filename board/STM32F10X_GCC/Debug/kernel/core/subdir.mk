################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../kernel/core/tos_event.c \
../kernel/core/tos_fifo.c \
../kernel/core/tos_global.c \
../kernel/core/tos_mmblk.c \
../kernel/core/tos_mmheap.c \
../kernel/core/tos_msg.c \
../kernel/core/tos_mutex.c \
../kernel/core/tos_pend.c \
../kernel/core/tos_queue.c \
../kernel/core/tos_robin.c \
../kernel/core/tos_sched.c \
../kernel/core/tos_sem.c \
../kernel/core/tos_sys.c \
../kernel/core/tos_task.c \
../kernel/core/tos_tick.c \
../kernel/core/tos_time.c \
../kernel/core/tos_timer.c 

OBJS += \
./kernel/core/tos_event.o \
./kernel/core/tos_fifo.o \
./kernel/core/tos_global.o \
./kernel/core/tos_mmblk.o \
./kernel/core/tos_mmheap.o \
./kernel/core/tos_msg.o \
./kernel/core/tos_mutex.o \
./kernel/core/tos_pend.o \
./kernel/core/tos_queue.o \
./kernel/core/tos_robin.o \
./kernel/core/tos_sched.o \
./kernel/core/tos_sem.o \
./kernel/core/tos_sys.o \
./kernel/core/tos_task.o \
./kernel/core/tos_tick.o \
./kernel/core/tos_time.o \
./kernel/core/tos_timer.o 

C_DEPS += \
./kernel/core/tos_event.d \
./kernel/core/tos_fifo.d \
./kernel/core/tos_global.d \
./kernel/core/tos_mmblk.d \
./kernel/core/tos_mmheap.d \
./kernel/core/tos_msg.d \
./kernel/core/tos_mutex.d \
./kernel/core/tos_pend.d \
./kernel/core/tos_queue.d \
./kernel/core/tos_robin.d \
./kernel/core/tos_sched.d \
./kernel/core/tos_sem.d \
./kernel/core/tos_sys.d \
./kernel/core/tos_task.d \
./kernel/core/tos_tick.d \
./kernel/core/tos_time.d \
./kernel/core/tos_timer.d 


# Each subdirectory must supply rules for building sources it contributes
kernel/core/%.o: ../kernel/core/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I"..\..\..\\board\STM32F10X_GCC\kernel\core\include" -I"..\..\..\\board\STM32F10X_GCC\arch\arm\arm-v7m\common\include" -I"..\..\..\\board\STM32F10X_GCC\osal\cmsis_os" -I"..\..\..\\board\STM32F10X_GCC\kernel\pm\include" -I"..\..\..\\board\STM32F10X_GCC\arch\arm\arm-v7m\cortex-m3\gcc" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


