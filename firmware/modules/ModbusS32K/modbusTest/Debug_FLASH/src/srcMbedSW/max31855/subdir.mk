################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/mbedSW/srcMbedSW/max31855/max31855.c 

OBJS += \
./src/srcMbedSW/max31855/max31855.o 

C_DEPS += \
./src/srcMbedSW/max31855/max31855.d 


# Each subdirectory must supply rules for building sources it contributes
src/srcMbedSW/max31855/max31855.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/mbedSW/srcMbedSW/max31855/max31855.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcMbedSW/max31855/max31855.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


