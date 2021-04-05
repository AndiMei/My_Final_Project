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
stm32_parametric_eq.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
stm32_parametric_eq.list \


# All Target
all: stm32_parametric_eq.elf secondary-outputs

# Tool invocations
stm32_parametric_eq.elf: $(OBJS) $(USER_OBJS) E:\Documents\Kuliah\Proyek_Akhir\Software\stm32_parametriq_eq\STM32F407VETX_FLASH.ld
	arm-none-eabi-gcc -o "stm32_parametric_eq.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"E:\Documents\Kuliah\Proyek_Akhir\Software\stm32_parametriq_eq\STM32F407VETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="stm32_parametric_eq.map" -Wl,--gc-sections -Wl,--verbose -static -L"../Libs" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -u _printf_float -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

stm32_parametric_eq.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "stm32_parametric_eq.list"
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
