################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/mbedSW/srcMbedSW/fir/fir.c 

OBJS += \
./src/srcMbedSW/fir/fir.o 

C_DEPS += \
./src/srcMbedSW/fir/fir.d 


# Each subdirectory must supply rules for building sources it contributes
src/srcMbedSW/fir/fir.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/mbedSW/srcMbedSW/fir/fir.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcMbedSW/fir/fir.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


