################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/mbedSW/srcMbedSW/thermocouple/thermocouple.c 

OBJS += \
./src/srcMbedSW/thermocouple/thermocouple.o 

C_DEPS += \
./src/srcMbedSW/thermocouple/thermocouple.d 


# Each subdirectory must supply rules for building sources it contributes
src/srcMbedSW/thermocouple/thermocouple.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/mbedSW/srcMbedSW/thermocouple/thermocouple.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcMbedSW/thermocouple/thermocouple.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


