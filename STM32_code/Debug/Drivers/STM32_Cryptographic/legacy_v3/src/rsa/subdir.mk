################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/STM32_Cryptographic/legacy_v3/src/rsa/legacy_v3_rsa.c 

OBJS += \
./Drivers/STM32_Cryptographic/legacy_v3/src/rsa/legacy_v3_rsa.o 

C_DEPS += \
./Drivers/STM32_Cryptographic/legacy_v3/src/rsa/legacy_v3_rsa.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/STM32_Cryptographic/legacy_v3/src/rsa/%.o Drivers/STM32_Cryptographic/legacy_v3/src/rsa/%.su Drivers/STM32_Cryptographic/legacy_v3/src/rsa/%.cyclo: ../Drivers/STM32_Cryptographic/legacy_v3/src/rsa/%.c Drivers/STM32_Cryptographic/legacy_v3/src/rsa/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32_Cryptographic/include -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-STM32_Cryptographic-2f-legacy_v3-2f-src-2f-rsa

clean-Drivers-2f-STM32_Cryptographic-2f-legacy_v3-2f-src-2f-rsa:
	-$(RM) ./Drivers/STM32_Cryptographic/legacy_v3/src/rsa/legacy_v3_rsa.cyclo ./Drivers/STM32_Cryptographic/legacy_v3/src/rsa/legacy_v3_rsa.d ./Drivers/STM32_Cryptographic/legacy_v3/src/rsa/legacy_v3_rsa.o ./Drivers/STM32_Cryptographic/legacy_v3/src/rsa/legacy_v3_rsa.su

.PHONY: clean-Drivers-2f-STM32_Cryptographic-2f-legacy_v3-2f-src-2f-rsa

