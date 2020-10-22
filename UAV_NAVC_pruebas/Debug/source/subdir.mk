################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/I2C.c \
../source/MAG3110.c \
../source/UAV_NAVC_pruebas.c \
../source/init_board.c \
../source/mma8451.c \
../source/mtb.c \
../source/semihost_hardfault.c 

OBJS += \
./source/I2C.o \
./source/MAG3110.o \
./source/UAV_NAVC_pruebas.o \
./source/init_board.o \
./source/mma8451.o \
./source/mtb.o \
./source/semihost_hardfault.o 

C_DEPS += \
./source/I2C.d \
./source/MAG3110.d \
./source/UAV_NAVC_pruebas.d \
./source/init_board.d \
./source/mma8451.d \
./source/mtb.d \
./source/semihost_hardfault.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MKL43Z256VLH4 -DCPU_MKL43Z256VLH4_cm0plus -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\board" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\source" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\drivers" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\device" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\CMSIS" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\component\uart" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\utilities" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\component\serial_manager" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\component\lists" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


