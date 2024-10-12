################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/mbedSW/srcMbedSW/circularBuffer/cBuff.c 

OBJS += \
./src/srcMbedSW/circularBuffer/cBuff.o 

C_DEPS += \
./src/srcMbedSW/circularBuffer/cBuff.d 


# Each subdirectory must supply rules for building sources it contributes
src/srcMbedSW/circularBuffer/cBuff.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/mbedSW/srcMbedSW/circularBuffer/cBuff.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcMbedSW/circularBuffer/cBuff.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


