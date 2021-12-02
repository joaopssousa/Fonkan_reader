################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Projects/SW4STM32/subdir.mk
-include Projects/subdir.mk
-include Middlewares/LoRaWAN/Utilities/subdir.mk
-include Middlewares/LoRaWAN/Patterns/Basic/subdir.mk
-include Middlewares/LoRaWAN/Mac/Region/subdir.mk
-include Middlewares/LoRaWAN/Mac/subdir.mk
-include Middlewares/LoRaWAN/Crypto/subdir.mk
-include Middlewares/FatFs/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/subdir.mk
-include Drivers/FATFS/subdir.mk
-include Drivers/CMSIS/subdir.mk
-include Drivers/BSP/sx1276mb1mas/subdir.mk
-include Drivers/BSP/STM32F4xx_Nucleo/subdir.mk
-include Drivers/BSP/Components/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
UserApp.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
UserApp.list \

OBJCOPY_BIN += \
UserApp.bin \


# All Target
all:
	+@make --no-print-directory main-build && make --no-print-directory post-build

# Main-build Target
main-build: UserApp.elf secondary-outputs

# Tool invocations
UserApp.elf: $(OBJS) $(USER_OBJS) ../STM32F407VGTx_FLASH.ld
	arm-none-eabi-gcc -o "UserApp.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"../STM32F407VGTx_FLASH.ld" --specs=nosys.specs -Wl,-Map="Curral.map" -Wl,--gc-sections -static -L../../../../Bootloader/2_Images_SBSFU/SW4STM32/STM32F407_NUCLEO_2_Images_SBSFU/Debug -Xlinker -L ../../../../Bootloader/Linker_Common/SW4STM32 --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

UserApp.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "UserApp.list"
	@echo 'Finished building: $@'
	@echo ' '

UserApp.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "UserApp.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

post-build:
	arm-none-eabi-objcopy -O binary "UserApp.elf" "../../UserApp.bin"
	arm-none-eabi-size "UserApp.elf"
	"../../../../Bootloader/2_Images_SECoreBin/SW4STM32/postbuild.sh" "../.." "./UserApp.elf" "../../UserApp.bin" "1" "1"
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
