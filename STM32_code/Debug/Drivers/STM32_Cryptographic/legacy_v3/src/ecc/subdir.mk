################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/STM32_Cryptographic/legacy_v3/src/ecc/legacy_v3_c25519.c \
../Drivers/STM32_Cryptographic/legacy_v3/src/ecc/legacy_v3_ecc.c \
../Drivers/STM32_Cryptographic/legacy_v3/src/ecc/legacy_v3_ed25519.c 

OBJS += \
./Drivers/STM32_Cryptographic/legacy_v3/src/ecc/legacy_v3_c25519.o \
./Drivers/STM32_Cryptographic/legacy_v3/src/ecc/legacy_v3_ecc.o \
./Drivers/STM32_Cryptographic/legacy_v3/src/ecc/legacy_v3_ed25519.o 

C_DEPS += \
./Drivers/STM32_Cryptographic/legacy_v3/src/ecc/legacy_v3_c25519.d \
./Drivers/STM32_Cryptographic/legacy_v3/src/ecc/legacy_v3_ecc.d \
./Drivers/STM32_Cryptographic/legacy_v3/src/ecc/legacy_v3_ed25519.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/STM32_Cryptographic/legacy_v3/src/ecc/%.o Drivers/STM32_Cryptographic/legacy_v3/src/ecc/%.su Drivers/STM32_Cryptographic/legacy_v3/src/ecc/%.cyclo: ../Drivers/STM32_Cryptographic/legacy_v3/src/ecc/%.c Drivers/STM32_Cryptographic/legacy_v3/src/ecc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32_Cryptographic/include -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-STM32_Cryptographic-2f-legacy_v3-2f-src-2f-ecc

clean-Drivers-2f-STM32_Cryptographic-2f-legacy_v3-2f-src-2f-ecc:
	-$(RM) ./Drivers/STM32_Cryptographic/legacy_v3/src/ecc/legacy_v3_c25519.cyclo ./Drivers/STM32_Cryptographic/legacy_v3/src/ecc/legacy_v3_c25519.d ./Drivers/STM32_Cryptographic/legacy_v3/src/ecc/legacy_v3_c25519.o ./Drivers/STM32_Cryptographic/legacy_v3/src/ecc/legacy_v3_c25519.su ./Drivers/STM32_Cryptographic/legacy_v3/src/ecc/legacy_v3_ecc.cyclo ./Drivers/STM32_Cryptographic/legacy_v3/src/ecc/legacy_v3_ecc.d ./Drivers/STM32_Cryptographic/legacy_v3/src/ecc/legacy_v3_ecc.o ./Drivers/STM32_Cryptographic/legacy_v3/src/ecc/legacy_v3_ecc.su ./Drivers/STM32_Cryptographic/legacy_v3/src/ecc/legacy_v3_ed25519.cyclo ./Drivers/STM32_Cryptographic/legacy_v3/src/ecc/legacy_v3_ed25519.d ./Drivers/STM32_Cryptographic/legacy_v3/src/ecc/legacy_v3_ed25519.o ./Drivers/STM32_Cryptographic/legacy_v3/src/ecc/legacy_v3_ed25519.su

.PHONY: clean-Drivers-2f-STM32_Cryptographic-2f-legacy_v3-2f-src-2f-ecc

