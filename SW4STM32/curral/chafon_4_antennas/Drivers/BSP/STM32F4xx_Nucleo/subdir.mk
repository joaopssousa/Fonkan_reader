################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Projetos/Agro/Drivers/BSP/STM32F4xx_Nucleo/stm32f4xx_nucleo.c 

OBJS += \
./Drivers/BSP/STM32F4xx_Nucleo/stm32f4xx_nucleo.o 

C_DEPS += \
./Drivers/BSP/STM32F4xx_Nucleo/stm32f4xx_nucleo.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/STM32F4xx_Nucleo/stm32f4xx_nucleo.o: C:/Projetos/Agro/Drivers/BSP/STM32F4xx_Nucleo/stm32f4xx_nucleo.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32F407xx -DUSE_CHAFON_4_ANTENNAS -DUSE_STM32F4XX_NUCLEO -DUSE_HAL_DRIVER -DREGION_AU915 '-DACTIVE_REGION=LORAMAC_REGION_AU915' -c -I../../../Core/inc -I../../../../../Middlewares/ST/STM32_Secure_Engine/Core -I../../../../../Middlewares/Third_Party/LoRaWAN/Patterns/Basic -I../../../../../Drivers/BSP/Components/fonkan -I../../../../../Drivers/BSP/Components/chafon -I../../../../../Middlewares/Third_Party/LoRaWAN/Patterns/Advanced/LmHandler/packages -I../../../../../Middlewares/Third_Party/LoRaWAN/Patterns/Advanced/LmHandler -I../../../../../Middlewares/Third_Party/LoRaWAN/Patterns/Advanced -I../../../../../Middlewares/Third_Party/LoRaWAN/Mac/region -I../../../LoRaWAN/App/inc -I../../../../../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../../../../../Drivers/BSP/STM32F4xx_Nucleo -I../../../../../Drivers/STM32F4xx_HAL_Driver/Inc -I../../../../../Drivers/CMSIS/Include -I../../../../../Middlewares/Third_Party/LoRaWAN/Crypto -I../../../../../Middlewares/Third_Party/FatFs/src -I../../../../../Middlewares/Third_Party/LoRaWAN/Mac -I../../../../../Middlewares/Third_Party/LoRaWAN/Phy -I../../../../../Middlewares/Third_Party/LoRaWAN/Utilities -I../../../../../Drivers/BSP/X_NUCLEO_IKS01A1 -I../../../../../Drivers/BSP/X_NUCLEO_IKS01A2 -I../../../../../Drivers/BSP/Components/Common -I../../../../../Drivers/FATFS/App -I../../../../../Drivers/FATFS/Target -I../../../../../Drivers/BSP/Components/station -I../../../../../Drivers/BSP/Components/hts221 -I../../../../../Drivers/BSP/Components/lps22hb -I../../../../../Drivers/BSP/Components/lps25hb -I../../../../../Drivers/BSP/Components/sx1276 -I../../../../../Drivers/BSP/sx1276mb1mas -I../../../../../Projects/Bootloader/Linker_Common/SW4STM32 -I../../../../../Projects/Bootloader/2_Images_SECoreBin/Inc -I../../../../../Projects/Bootloader/2_Images_SBSFU/SBSFU/App -Os -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/BSP/STM32F4xx_Nucleo/stm32f4xx_nucleo.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

