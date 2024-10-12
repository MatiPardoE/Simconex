################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/mbedSW/srcMbedSW/tpic8101/tpic8101.c 

OBJS += \
./src/srcMbedSW/tpic8101/tpic8101.o 

C_DEPS += \
./src/srcMbedSW/tpic8101/tpic8101.d 


# Each subdirectory must supply rules for building sources it contributes
src/srcMbedSW/tpic8101/tpic8101.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/mbedSW/srcMbedSW/tpic8101/tpic8101.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcMbedSW/tpic8101/tpic8101.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


