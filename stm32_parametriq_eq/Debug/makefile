################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
PA-MAIN_MCU_CODEC2.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
PA-MAIN_MCU_CODEC2.list \


# All Target
all: PA-MAIN_MCU_CODEC2.elf secondary-outputs

# Tool invocations
PA-MAIN_MCU_CODEC2.elf: $(OBJS) $(USER_OBJS) E:\Documents\Kuliah\Proyek_Akhir\Program\STM32CubeIDE-Andi_PA\PA-MAIN_MCU_CODEC2\STM32F407VETX_FLASH.ld
	arm-none-eabi-gcc -o "PA-MAIN_MCU_CODEC2.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"E:\Documents\Kuliah\Proyek_Akhir\Program\STM32CubeIDE-Andi_PA\PA-MAIN_MCU_CODEC2\STM32F407VETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="PA-MAIN_MCU_CODEC2.map" -Wl,--gc-sections -Wl,--verbose -static -L"../Libs" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

PA-MAIN_MCU_CODEC2.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "PA-MAIN_MCU_CODEC2.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
