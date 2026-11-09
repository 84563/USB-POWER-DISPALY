################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/user/QC_Get.c \
../Core/Src/user/ina220.c \
../Core/Src/user/menu.c \
../Core/Src/user/oled.c 

OBJS += \
./Core/Src/user/QC_Get.o \
./Core/Src/user/ina220.o \
./Core/Src/user/menu.o \
./Core/Src/user/oled.o 

C_DEPS += \
./Core/Src/user/QC_Get.d \
./Core/Src/user/ina220.d \
./Core/Src/user/menu.d \
./Core/Src/user/oled.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/user/%.o Core/Src/user/%.su: ../Core/Src/user/%.c Core/Src/user/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G030xx -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G0xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-user

clean-Core-2f-Src-2f-user:
	-$(RM) ./Core/Src/user/QC_Get.d ./Core/Src/user/QC_Get.o ./Core/Src/user/QC_Get.su ./Core/Src/user/ina220.d ./Core/Src/user/ina220.o ./Core/Src/user/ina220.su ./Core/Src/user/menu.d ./Core/Src/user/menu.o ./Core/Src/user/menu.su ./Core/Src/user/oled.d ./Core/Src/user/oled.o ./Core/Src/user/oled.su

.PHONY: clean-Core-2f-Src-2f-user

