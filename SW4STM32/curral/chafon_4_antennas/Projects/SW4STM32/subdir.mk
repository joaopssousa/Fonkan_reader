################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
C:/Projetos/Agro/Projects/Curral/SW4STM32/startup_stm32f407xx.s 

OBJS += \
./Projects/SW4STM32/startup_stm32f407xx.o 

S_DEPS += \
./Projects/SW4STM32/startup_stm32f407xx.d 


# Each subdirectory must supply rules for building sources it contributes
Projects/SW4STM32/startup_stm32f407xx.o: C:/Projetos/Agro/Projects/Curral/SW4STM32/startup_stm32f407xx.s
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -c -x assembler-with-cpp -MMD -MP -MF"Projects/SW4STM32/startup_stm32f407xx.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

