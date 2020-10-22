################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../device/system_MKL43Z4.c 

OBJS += \
./device/system_MKL43Z4.o 

C_DEPS += \
./device/system_MKL43Z4.d 


# Each subdirectory must supply rules for building sources it contributes
device/%.o: ../device/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MKL43Z256VLH4 -DCPU_MKL43Z256VLH4_cm0plus -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\board" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\source" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\drivers" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\device" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\CMSIS" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\component\uart" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\utilities" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\component\serial_manager" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\component\lists" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


