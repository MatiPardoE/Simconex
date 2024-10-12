################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_adc.c \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_clock.c \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_crc.c \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_dma.c \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_flexcan.c \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_flexram.c \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_ftfc.c \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_ftm.c \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_gpio.c \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_i2c.c \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_lpit.c \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_lpuart.c \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_nvic.c \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_pdb.c \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_rcm.c \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_spi.c \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_systick.c \
C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_template.c 

OBJS += \
./src/srcS32K1XX/fw_adc.o \
./src/srcS32K1XX/fw_clock.o \
./src/srcS32K1XX/fw_crc.o \
./src/srcS32K1XX/fw_dma.o \
./src/srcS32K1XX/fw_flexcan.o \
./src/srcS32K1XX/fw_flexram.o \
./src/srcS32K1XX/fw_ftfc.o \
./src/srcS32K1XX/fw_ftm.o \
./src/srcS32K1XX/fw_gpio.o \
./src/srcS32K1XX/fw_i2c.o \
./src/srcS32K1XX/fw_lpit.o \
./src/srcS32K1XX/fw_lpuart.o \
./src/srcS32K1XX/fw_nvic.o \
./src/srcS32K1XX/fw_pdb.o \
./src/srcS32K1XX/fw_rcm.o \
./src/srcS32K1XX/fw_spi.o \
./src/srcS32K1XX/fw_systick.o \
./src/srcS32K1XX/fw_template.o 

C_DEPS += \
./src/srcS32K1XX/fw_adc.d \
./src/srcS32K1XX/fw_clock.d \
./src/srcS32K1XX/fw_crc.d \
./src/srcS32K1XX/fw_dma.d \
./src/srcS32K1XX/fw_flexcan.d \
./src/srcS32K1XX/fw_flexram.d \
./src/srcS32K1XX/fw_ftfc.d \
./src/srcS32K1XX/fw_ftm.d \
./src/srcS32K1XX/fw_gpio.d \
./src/srcS32K1XX/fw_i2c.d \
./src/srcS32K1XX/fw_lpit.d \
./src/srcS32K1XX/fw_lpuart.d \
./src/srcS32K1XX/fw_nvic.d \
./src/srcS32K1XX/fw_pdb.d \
./src/srcS32K1XX/fw_rcm.d \
./src/srcS32K1XX/fw_spi.d \
./src/srcS32K1XX/fw_systick.d \
./src/srcS32K1XX/fw_template.d 


# Each subdirectory must supply rules for building sources it contributes
src/srcS32K1XX/fw_adc.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_adc.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcS32K1XX/fw_adc.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/srcS32K1XX/fw_clock.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_clock.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcS32K1XX/fw_clock.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/srcS32K1XX/fw_crc.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_crc.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcS32K1XX/fw_crc.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/srcS32K1XX/fw_dma.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_dma.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcS32K1XX/fw_dma.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/srcS32K1XX/fw_flexcan.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_flexcan.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcS32K1XX/fw_flexcan.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/srcS32K1XX/fw_flexram.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_flexram.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcS32K1XX/fw_flexram.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/srcS32K1XX/fw_ftfc.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_ftfc.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcS32K1XX/fw_ftfc.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/srcS32K1XX/fw_ftm.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_ftm.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcS32K1XX/fw_ftm.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/srcS32K1XX/fw_gpio.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_gpio.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcS32K1XX/fw_gpio.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/srcS32K1XX/fw_i2c.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_i2c.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcS32K1XX/fw_i2c.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/srcS32K1XX/fw_lpit.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_lpit.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcS32K1XX/fw_lpit.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/srcS32K1XX/fw_lpuart.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_lpuart.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcS32K1XX/fw_lpuart.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/srcS32K1XX/fw_nvic.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_nvic.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcS32K1XX/fw_nvic.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/srcS32K1XX/fw_pdb.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_pdb.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcS32K1XX/fw_pdb.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/srcS32K1XX/fw_rcm.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_rcm.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcS32K1XX/fw_rcm.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/srcS32K1XX/fw_spi.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_spi.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcS32K1XX/fw_spi.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/srcS32K1XX/fw_systick.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_systick.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcS32K1XX/fw_systick.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/srcS32K1XX/fw_template.o: C:/Users/toto_/OneDrive/Documentos/Electronics/EmbeddedUtils/baremetal/S32K1XX_HAL/srcS32K1XX/fw_template.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/srcS32K1XX/fw_template.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


