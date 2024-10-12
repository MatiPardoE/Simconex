################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/mbedSW/srcMbedSW/mc34gd3000/mc34gd3000.c 

OBJS += \
./src/srcMbedSW/mc34gd3000/mc34gd3000.o 

C_DEPS += \
./src/srcMbedSW/mc34gd3000/mc34gd3000.d 


# Each subdirectory must supply rules for building sources it contributes
src/srcMbedSW/mc34gd3000/mc34gd3000.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/mbedSW/srcMbedSW/mc34gd3000/mc34gd3000.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcMbedSW/mc34gd3000/mc34gd3000.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


