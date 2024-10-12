################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/mbedSW/srcMbedSW/binarySearch/bs.c 

OBJS += \
./src/srcMbedSW/binarySearch/bs.o 

C_DEPS += \
./src/srcMbedSW/binarySearch/bs.d 


# Each subdirectory must supply rules for building sources it contributes
src/srcMbedSW/binarySearch/bs.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/mbedSW/srcMbedSW/binarySearch/bs.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcMbedSW/binarySearch/bs.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


