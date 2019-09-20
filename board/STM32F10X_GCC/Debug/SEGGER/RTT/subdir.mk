################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SEGGER/RTT/SEGGER_RTT.c \
../SEGGER/RTT/SEGGER_RTT_printf.c \
../SEGGER/RTT/SEGGER_SYSVIEW.c 

OBJS += \
./SEGGER/RTT/SEGGER_RTT.o \
./SEGGER/RTT/SEGGER_RTT_printf.o \
./SEGGER/RTT/SEGGER_SYSVIEW.o 

C_DEPS += \
./SEGGER/RTT/SEGGER_RTT.d \
./SEGGER/RTT/SEGGER_RTT_printf.d \
./SEGGER/RTT/SEGGER_SYSVIEW.d 


# Each subdirectory must supply rules for building sources it contributes
SEGGER/RTT/%.o: ../SEGGER/RTT/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I"..\..\..\\arch\arm\arm-v7m\common\include" -I"..\..\..\\arch\arm\arm-v7m\cortex-m3\gcc" -I"..\..\..\\kernel\core\include" -I"..\..\..\\kernel\pm\include" -I"..\..\..\\osal\cmsis_os" -I"..\..\..\\board\STM32F10X_GCC\SEGGER\RTT" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


