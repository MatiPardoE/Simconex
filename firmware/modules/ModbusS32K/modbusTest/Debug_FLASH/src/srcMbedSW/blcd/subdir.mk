################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/mbedSW/srcMbedSW/blcd/blcdHall.c \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/mbedSW/srcMbedSW/blcd/blcdPhases.c 

OBJS += \
./src/srcMbedSW/blcd/blcdHall.o \
./src/srcMbedSW/blcd/blcdPhases.o 

C_DEPS += \
./src/srcMbedSW/blcd/blcdHall.d \
./src/srcMbedSW/blcd/blcdPhases.d 


# Each subdirectory must supply rules for building sources it contributes
src/srcMbedSW/blcd/blcdHall.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/mbedSW/srcMbedSW/blcd/blcdHall.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcMbedSW/blcd/blcdHall.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/srcMbedSW/blcd/blcdPhases.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/mbedSW/srcMbedSW/blcd/blcdPhases.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcMbedSW/blcd/blcdPhases.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


