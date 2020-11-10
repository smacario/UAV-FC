################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/GPS.c \
../source/I2C.c \
../source/MMA8451.c \
../source/QMC5883L.c \
../source/UAV_NAVC_pruebas.c \
../source/init_board.c \
../source/mtb.c \
../source/semihost_hardfault.c 

OBJS += \
./source/GPS.o \
./source/I2C.o \
./source/MMA8451.o \
./source/QMC5883L.o \
./source/UAV_NAVC_pruebas.o \
./source/init_board.o \
./source/mtb.o \
./source/semihost_hardfault.o 

C_DEPS += \
./source/GPS.d \
./source/I2C.d \
./source/MMA8451.d \
./source/QMC5883L.d \
./source/UAV_NAVC_pruebas.d \
./source/init_board.d \
./source/mtb.d \
./source/semihost_hardfault.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DSCANF_FLOAT_ENABLE=1 -DCPU_MKL43Z256VLH4 -DCPU_MKL43Z256VLH4_cm0plus -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=0 -DPRINTF_FLOAT_ENABLE=1 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__NEWLIB__ -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\board" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\source" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\drivers" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\device" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\CMSIS" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\component\uart" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\utilities" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\component\serial_manager" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\component\lists" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m0plus -mthumb -D__NEWLIB__ -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


