################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_cbc.c \
../Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_ccm.c \
../Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_cfb.c \
../Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_ctr.c \
../Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_ecb.c \
../Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_gcm.c \
../Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_keywrap.c \
../Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_ofb.c \
../Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_xts.c \
../Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_chachapoly.c 

OBJS += \
./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_cbc.o \
./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_ccm.o \
./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_cfb.o \
./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_ctr.o \
./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_ecb.o \
./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_gcm.o \
./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_keywrap.o \
./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_ofb.o \
./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_xts.o \
./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_chachapoly.o 

C_DEPS += \
./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_cbc.d \
./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_ccm.d \
./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_cfb.d \
./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_ctr.d \
./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_ecb.d \
./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_gcm.d \
./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_keywrap.d \
./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_ofb.d \
./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_xts.d \
./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_chachapoly.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/STM32_Cryptographic/legacy_v3/src/cipher/%.o Drivers/STM32_Cryptographic/legacy_v3/src/cipher/%.su Drivers/STM32_Cryptographic/legacy_v3/src/cipher/%.cyclo: ../Drivers/STM32_Cryptographic/legacy_v3/src/cipher/%.c Drivers/STM32_Cryptographic/legacy_v3/src/cipher/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32_Cryptographic/include -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-STM32_Cryptographic-2f-legacy_v3-2f-src-2f-cipher

clean-Drivers-2f-STM32_Cryptographic-2f-legacy_v3-2f-src-2f-cipher:
	-$(RM) ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_cbc.cyclo ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_cbc.d ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_cbc.o ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_cbc.su ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_ccm.cyclo ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_ccm.d ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_ccm.o ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_ccm.su ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_cfb.cyclo ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_cfb.d ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_cfb.o ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_cfb.su ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_ctr.cyclo ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_ctr.d ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_ctr.o ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_ctr.su ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_ecb.cyclo ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_ecb.d ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_ecb.o ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_ecb.su ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_gcm.cyclo ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_gcm.d ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_gcm.o ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_gcm.su ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_keywrap.cyclo ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_keywrap.d ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_keywrap.o ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_keywrap.su ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_ofb.cyclo ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_ofb.d ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_ofb.o ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_ofb.su ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_xts.cyclo ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_xts.d ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_xts.o ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_aes_xts.su ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_chachapoly.cyclo ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_chachapoly.d ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_chachapoly.o ./Drivers/STM32_Cryptographic/legacy_v3/src/cipher/legacy_v3_chachapoly.su

.PHONY: clean-Drivers-2f-STM32_Cryptographic-2f-legacy_v3-2f-src-2f-cipher

