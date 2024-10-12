################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/srcS32K1XX/subdir.mk
-include src/srcMbedSW/tpic8101/subdir.mk
-include src/srcMbedSW/time/subdir.mk
-include src/srcMbedSW/thermocouple/subdir.mk
-include src/srcMbedSW/str/subdir.mk
-include src/srcMbedSW/std/subdir.mk
-include src/srcMbedSW/ssd1309oled/subdir.mk
-include src/srcMbedSW/rpm/subdir.mk
-include src/srcMbedSW/rgb/subdir.mk
-include src/srcMbedSW/mux507/subdir.mk
-include src/srcMbedSW/mc34gd3000/subdir.mk
-include src/srcMbedSW/max31855/subdir.mk
-include src/srcMbedSW/math/subdir.mk
-include src/srcMbedSW/lm75/subdir.mk
-include src/srcMbedSW/knock/subdir.mk
-include src/srcMbedSW/gpio/subdir.mk
-include src/srcMbedSW/fonts/subdir.mk
-include src/srcMbedSW/fir/subdir.mk
-include src/srcMbedSW/circularBuffer/subdir.mk
-include src/srcMbedSW/blcd/subdir.mk
-include src/srcMbedSW/binarySearch/subdir.mk
-include src/srcMbedSW/array/subdir.mk
-include src/srcMbedSW/analog/subdir.mk
-include src/srcMbedSW/subdir.mk
-include src/subdir.mk
-include Project_Settings/Startup_Code/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_SIZE += \
modbusTest.siz \


# All Target
all: modbusTest.elf secondary-outputs

# Tool invocations
modbusTest.elf: $(OBJS) C:/Users/toto_/Desktop/FBR-SIMCONEX/Simconex/firmware/modules/ModbusS32K/modbusTest/Project_Settings/Linker_Files/S32K116_17_flash.ld $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Standard S32DS C Linker'
	arm-none-eabi-gcc -o "modbusTest.elf" "@modbusTest.args"  $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

modbusTest.siz: modbusTest.elf
	@echo 'Invoking: Standard S32DS Print Size'
	arm-none-eabi-size --format=berkeley modbusTest.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) ./*
	-@echo ' '

secondary-outputs: $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
