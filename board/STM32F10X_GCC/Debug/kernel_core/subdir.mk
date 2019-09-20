################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_event.c \
D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_fifo.c \
D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_global.c \
D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_mmblk.c \
D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_mmheap.c \
D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_msg.c \
D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_mutex.c \
D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_pend.c \
D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_queue.c \
D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_robin.c \
D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_sched.c \
D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_sem.c \
D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_sys.c \
D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_task.c \
D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_tick.c \
D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_time.c \
D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_timer.c 

OBJS += \
./kernel_core/tos_event.o \
./kernel_core/tos_fifo.o \
./kernel_core/tos_global.o \
./kernel_core/tos_mmblk.o \
./kernel_core/tos_mmheap.o \
./kernel_core/tos_msg.o \
./kernel_core/tos_mutex.o \
./kernel_core/tos_pend.o \
./kernel_core/tos_queue.o \
./kernel_core/tos_robin.o \
./kernel_core/tos_sched.o \
./kernel_core/tos_sem.o \
./kernel_core/tos_sys.o \
./kernel_core/tos_task.o \
./kernel_core/tos_tick.o \
./kernel_core/tos_time.o \
./kernel_core/tos_timer.o 

C_DEPS += \
./kernel_core/tos_event.d \
./kernel_core/tos_fifo.d \
./kernel_core/tos_global.d \
./kernel_core/tos_mmblk.d \
./kernel_core/tos_mmheap.d \
./kernel_core/tos_msg.d \
./kernel_core/tos_mutex.d \
./kernel_core/tos_pend.d \
./kernel_core/tos_queue.d \
./kernel_core/tos_robin.d \
./kernel_core/tos_sched.d \
./kernel_core/tos_sem.d \
./kernel_core/tos_sys.d \
./kernel_core/tos_task.d \
./kernel_core/tos_tick.d \
./kernel_core/tos_time.d \
./kernel_core/tos_timer.d 


# Each subdirectory must supply rules for building sources it contributes
kernel_core/tos_event.o: D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_event.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I"..\..\..\\arch\arm\arm-v7m\common\include" -I"..\..\..\\arch\arm\arm-v7m\cortex-m3\gcc" -I"..\..\..\\kernel\core\include" -I"..\..\..\\kernel\pm\include" -I"..\..\..\\osal\cmsis_os" -I"..\..\..\\board\STM32F10X_GCC\SEGGER\RTT" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

kernel_core/tos_fifo.o: D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_fifo.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I"..\..\..\\arch\arm\arm-v7m\common\include" -I"..\..\..\\arch\arm\arm-v7m\cortex-m3\gcc" -I"..\..\..\\kernel\core\include" -I"..\..\..\\kernel\pm\include" -I"..\..\..\\osal\cmsis_os" -I"..\..\..\\board\STM32F10X_GCC\SEGGER\RTT" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

kernel_core/tos_global.o: D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_global.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I"..\..\..\\arch\arm\arm-v7m\common\include" -I"..\..\..\\arch\arm\arm-v7m\cortex-m3\gcc" -I"..\..\..\\kernel\core\include" -I"..\..\..\\kernel\pm\include" -I"..\..\..\\osal\cmsis_os" -I"..\..\..\\board\STM32F10X_GCC\SEGGER\RTT" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

kernel_core/tos_mmblk.o: D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_mmblk.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I"..\..\..\\arch\arm\arm-v7m\common\include" -I"..\..\..\\arch\arm\arm-v7m\cortex-m3\gcc" -I"..\..\..\\kernel\core\include" -I"..\..\..\\kernel\pm\include" -I"..\..\..\\osal\cmsis_os" -I"..\..\..\\board\STM32F10X_GCC\SEGGER\RTT" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

kernel_core/tos_mmheap.o: D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_mmheap.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I"..\..\..\\arch\arm\arm-v7m\common\include" -I"..\..\..\\arch\arm\arm-v7m\cortex-m3\gcc" -I"..\..\..\\kernel\core\include" -I"..\..\..\\kernel\pm\include" -I"..\..\..\\osal\cmsis_os" -I"..\..\..\\board\STM32F10X_GCC\SEGGER\RTT" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

kernel_core/tos_msg.o: D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_msg.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I"..\..\..\\arch\arm\arm-v7m\common\include" -I"..\..\..\\arch\arm\arm-v7m\cortex-m3\gcc" -I"..\..\..\\kernel\core\include" -I"..\..\..\\kernel\pm\include" -I"..\..\..\\osal\cmsis_os" -I"..\..\..\\board\STM32F10X_GCC\SEGGER\RTT" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

kernel_core/tos_mutex.o: D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_mutex.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I"..\..\..\\arch\arm\arm-v7m\common\include" -I"..\..\..\\arch\arm\arm-v7m\cortex-m3\gcc" -I"..\..\..\\kernel\core\include" -I"..\..\..\\kernel\pm\include" -I"..\..\..\\osal\cmsis_os" -I"..\..\..\\board\STM32F10X_GCC\SEGGER\RTT" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

kernel_core/tos_pend.o: D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_pend.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I"..\..\..\\arch\arm\arm-v7m\common\include" -I"..\..\..\\arch\arm\arm-v7m\cortex-m3\gcc" -I"..\..\..\\kernel\core\include" -I"..\..\..\\kernel\pm\include" -I"..\..\..\\osal\cmsis_os" -I"..\..\..\\board\STM32F10X_GCC\SEGGER\RTT" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

kernel_core/tos_queue.o: D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_queue.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I"..\..\..\\arch\arm\arm-v7m\common\include" -I"..\..\..\\arch\arm\arm-v7m\cortex-m3\gcc" -I"..\..\..\\kernel\core\include" -I"..\..\..\\kernel\pm\include" -I"..\..\..\\osal\cmsis_os" -I"..\..\..\\board\STM32F10X_GCC\SEGGER\RTT" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

kernel_core/tos_robin.o: D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_robin.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I"..\..\..\\arch\arm\arm-v7m\common\include" -I"..\..\..\\arch\arm\arm-v7m\cortex-m3\gcc" -I"..\..\..\\kernel\core\include" -I"..\..\..\\kernel\pm\include" -I"..\..\..\\osal\cmsis_os" -I"..\..\..\\board\STM32F10X_GCC\SEGGER\RTT" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

kernel_core/tos_sched.o: D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_sched.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I"..\..\..\\arch\arm\arm-v7m\common\include" -I"..\..\..\\arch\arm\arm-v7m\cortex-m3\gcc" -I"..\..\..\\kernel\core\include" -I"..\..\..\\kernel\pm\include" -I"..\..\..\\osal\cmsis_os" -I"..\..\..\\board\STM32F10X_GCC\SEGGER\RTT" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

kernel_core/tos_sem.o: D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_sem.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I"..\..\..\\arch\arm\arm-v7m\common\include" -I"..\..\..\\arch\arm\arm-v7m\cortex-m3\gcc" -I"..\..\..\\kernel\core\include" -I"..\..\..\\kernel\pm\include" -I"..\..\..\\osal\cmsis_os" -I"..\..\..\\board\STM32F10X_GCC\SEGGER\RTT" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

kernel_core/tos_sys.o: D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_sys.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I"..\..\..\\arch\arm\arm-v7m\common\include" -I"..\..\..\\arch\arm\arm-v7m\cortex-m3\gcc" -I"..\..\..\\kernel\core\include" -I"..\..\..\\kernel\pm\include" -I"..\..\..\\osal\cmsis_os" -I"..\..\..\\board\STM32F10X_GCC\SEGGER\RTT" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

kernel_core/tos_task.o: D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_task.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I"..\..\..\\arch\arm\arm-v7m\common\include" -I"..\..\..\\arch\arm\arm-v7m\cortex-m3\gcc" -I"..\..\..\\kernel\core\include" -I"..\..\..\\kernel\pm\include" -I"..\..\..\\osal\cmsis_os" -I"..\..\..\\board\STM32F10X_GCC\SEGGER\RTT" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

kernel_core/tos_tick.o: D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_tick.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I"..\..\..\\arch\arm\arm-v7m\common\include" -I"..\..\..\\arch\arm\arm-v7m\cortex-m3\gcc" -I"..\..\..\\kernel\core\include" -I"..\..\..\\kernel\pm\include" -I"..\..\..\\osal\cmsis_os" -I"..\..\..\\board\STM32F10X_GCC\SEGGER\RTT" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

kernel_core/tos_time.o: D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_time.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I"..\..\..\\arch\arm\arm-v7m\common\include" -I"..\..\..\\arch\arm\arm-v7m\cortex-m3\gcc" -I"..\..\..\\kernel\core\include" -I"..\..\..\\kernel\pm\include" -I"..\..\..\\osal\cmsis_os" -I"..\..\..\\board\STM32F10X_GCC\SEGGER\RTT" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

kernel_core/tos_timer.o: D:/TencentOS-Tiny/TencentOS-tiny/kernel/core/tos_timer.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I"..\..\..\\arch\arm\arm-v7m\common\include" -I"..\..\..\\arch\arm\arm-v7m\cortex-m3\gcc" -I"..\..\..\\kernel\core\include" -I"..\..\..\\kernel\pm\include" -I"..\..\..\\osal\cmsis_os" -I"..\..\..\\board\STM32F10X_GCC\SEGGER\RTT" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


