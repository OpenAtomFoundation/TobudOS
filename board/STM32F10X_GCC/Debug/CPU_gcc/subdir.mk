################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/TencentOS-Tiny/TencentOS-tiny/arch/arm/arm-v7m/cortex-m3/gcc/port_c.c 

S_UPPER_SRCS += \
D:/TencentOS-Tiny/TencentOS-tiny/arch/arm/arm-v7m/cortex-m3/gcc/port_s.S 

OBJS += \
./CPU_gcc/port_c.o \
./CPU_gcc/port_s.o 

S_UPPER_DEPS += \
./CPU_gcc/port_s.d 

C_DEPS += \
./CPU_gcc/port_c.d 


# Each subdirectory must supply rules for building sources it contributes
CPU_gcc/port_c.o: D:/TencentOS-Tiny/TencentOS-tiny/arch/arm/arm-v7m/cortex-m3/gcc/port_c.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I"..\..\..\\arch\arm\arm-v7m\common\include" -I"..\..\..\\arch\arm\arm-v7m\cortex-m3\gcc" -I"..\..\..\\kernel\core\include" -I"..\..\..\\kernel\pm\include" -I"..\..\..\\osal\cmsis_os" -I"..\..\..\\board\STM32F10X_GCC\SEGGER\RTT" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CPU_gcc/port_s.o: D:/TencentOS-Tiny/TencentOS-tiny/arch/arm/arm-v7m/cortex-m3/gcc/port_s.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -x assembler-with-cpp -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I"..\..\..\\arch\arm\arm-v7m\cortex-m3\gcc" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


