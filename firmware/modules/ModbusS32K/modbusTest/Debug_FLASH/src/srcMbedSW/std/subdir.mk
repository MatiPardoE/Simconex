################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/mbedSW/srcMbedSW/std/std.c 

OBJS += \
./src/srcMbedSW/std/std.o 

C_DEPS += \
./src/srcMbedSW/std/std.d 


# Each subdirectory must supply rules for building sources it contributes
src/srcMbedSW/std/std.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/mbedSW/srcMbedSW/std/std.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcMbedSW/std/std.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


