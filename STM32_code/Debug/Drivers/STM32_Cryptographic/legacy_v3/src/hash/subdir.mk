################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha1.c \
../Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha224.c \
../Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha256.c \
../Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha384.c \
../Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha512.c 

OBJS += \
./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha1.o \
./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha224.o \
./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha256.o \
./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha384.o \
./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha512.o 

C_DEPS += \
./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha1.d \
./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha224.d \
./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha256.d \
./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha384.d \
./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha512.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/STM32_Cryptographic/legacy_v3/src/hash/%.o Drivers/STM32_Cryptographic/legacy_v3/src/hash/%.su Drivers/STM32_Cryptographic/legacy_v3/src/hash/%.cyclo: ../Drivers/STM32_Cryptographic/legacy_v3/src/hash/%.c Drivers/STM32_Cryptographic/legacy_v3/src/hash/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32_Cryptographic/include -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-STM32_Cryptographic-2f-legacy_v3-2f-src-2f-hash

clean-Drivers-2f-STM32_Cryptographic-2f-legacy_v3-2f-src-2f-hash:
	-$(RM) ./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha1.cyclo ./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha1.d ./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha1.o ./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha1.su ./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha224.cyclo ./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha224.d ./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha224.o ./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha224.su ./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha256.cyclo ./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha256.d ./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha256.o ./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha256.su ./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha384.cyclo ./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha384.d ./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha384.o ./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha384.su ./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha512.cyclo ./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha512.d ./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha512.o ./Drivers/STM32_Cryptographic/legacy_v3/src/hash/legacy_v3_sha512.su

.PHONY: clean-Drivers-2f-STM32_Cryptographic-2f-legacy_v3-2f-src-2f-hash

