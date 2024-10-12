################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/mbedSW/srcMbedSW/rgb/rgb.c 

OBJS += \
./src/srcMbedSW/rgb/rgb.o 

C_DEPS += \
./src/srcMbedSW/rgb/rgb.d 


# Each subdirectory must supply rules for building sources it contributes
src/srcMbedSW/rgb/rgb.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/mbedSW/srcMbedSW/rgb/rgb.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcMbedSW/rgb/rgb.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


