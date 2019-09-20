################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../kernel/pm/tos_pm.c \
../kernel/pm/tos_tickless.c 

OBJS += \
./kernel/pm/tos_pm.o \
./kernel/pm/tos_tickless.o 

C_DEPS += \
./kernel/pm/tos_pm.d \
./kernel/pm/tos_tickless.d 


# Each subdirectory must supply rules for building sources it contributes
kernel/pm/%.o: ../kernel/pm/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I"..\..\..\\board\STM32F10X_GCC\kernel\core\include" -I"..\..\..\\board\STM32F10X_GCC\arch\arm\arm-v7m\common\include" -I"..\..\..\\board\STM32F10X_GCC\osal\cmsis_os" -I"..\..\..\\board\STM32F10X_GCC\kernel\pm\include" -I"..\..\..\\board\STM32F10X_GCC\arch\arm\arm-v7m\cortex-m3\gcc" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


