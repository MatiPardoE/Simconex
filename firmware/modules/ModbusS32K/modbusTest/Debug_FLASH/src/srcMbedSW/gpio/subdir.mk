################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/mbedSW/srcMbedSW/gpio/gpio.c 

OBJS += \
./src/srcMbedSW/gpio/gpio.o 

C_DEPS += \
./src/srcMbedSW/gpio/gpio.d 


# Each subdirectory must supply rules for building sources it contributes
src/srcMbedSW/gpio/gpio.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/mbedSW/srcMbedSW/gpio/gpio.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcMbedSW/gpio/gpio.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


