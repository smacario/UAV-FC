################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../startup/startup_mkl43z4.c 

OBJS += \
./startup/startup_mkl43z4.o 

C_DEPS += \
./startup/startup_mkl43z4.d 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MKL43Z256VLH4 -DCPU_MKL43Z256VLH4_cm0plus -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DNDEBUG -D__REDLIB__ -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\board" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\source" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\drivers" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\device" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\CMSIS" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\component\uart" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\utilities" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\component\serial_manager" -I"C:\Users\Santiago\Desktop\Facultad\Kinetis\UAV_NAVC_pruebas\component\lists" -Os -fno-common -g -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


