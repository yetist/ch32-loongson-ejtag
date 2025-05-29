################################################################################
# MRS Version: 2.1.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Library/cherryusb/class/cdc/usbd_cdc_acm.c \
../Library/cherryusb/class/cdc/usbd_cdc_ecm.c \
../Library/cherryusb/class/cdc/usbh_cdc_acm.c \
../Library/cherryusb/class/cdc/usbh_cdc_ecm.c \
../Library/cherryusb/class/cdc/usbh_cdc_ncm.c 

C_DEPS += \
./Library/cherryusb/class/cdc/usbd_cdc_acm.d \
./Library/cherryusb/class/cdc/usbd_cdc_ecm.d \
./Library/cherryusb/class/cdc/usbh_cdc_acm.d \
./Library/cherryusb/class/cdc/usbh_cdc_ecm.d \
./Library/cherryusb/class/cdc/usbh_cdc_ncm.d 

OBJS += \
./Library/cherryusb/class/cdc/usbd_cdc_acm.o \
./Library/cherryusb/class/cdc/usbd_cdc_ecm.o \
./Library/cherryusb/class/cdc/usbh_cdc_acm.o \
./Library/cherryusb/class/cdc/usbh_cdc_ecm.o \
./Library/cherryusb/class/cdc/usbh_cdc_ncm.o 



# Each subdirectory must supply rules for building sources it contributes
Library/cherryusb/class/cdc/%.o: ../Library/cherryusb/class/cdc/%.c
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Ofast -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -DDEBUG=2 -DCH32LSEJTAG_HARDWARE_VERSION=0x0101 -I"d:/prj/embedded/ch32v203f6p6-blink/CH32V203F6P6/Debug" -I"d:/prj/embedded/ch32v203f6p6-blink/CH32V203F6P6/Core" -I"d:/prj/embedded/ch32v203f6p6-blink/CH32V203F6P6/User" -I"d:/prj/embedded/ch32v203f6p6-blink/CH32V203F6P6/Peripheral/inc" -I"d:/prj/embedded/ch32v203f6p6-blink/CH32V203F6P6/Library/cherryusb/common" -I"d:/prj/embedded/ch32v203f6p6-blink/CH32V203F6P6/Library/cherryusb/core" -I"d:/prj/embedded/ch32v203f6p6-blink/CH32V203F6P6/Library/cherryusb/class/hub" -I"d:/prj/embedded/ch32v203f6p6-blink/CH32V203F6P6/Library/cherryusb/class/cdc" -I"d:/prj/embedded/ch32v203f6p6-blink/CH32V203F6P6/Library/lsejtag" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
