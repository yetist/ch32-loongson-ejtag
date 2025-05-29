################################################################################
# MRS Version: 2.1.0
# Automatically-generated file. Do not edit!
################################################################################
-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/subdir.mk
-include Startup/subdir.mk
-include Peripheral/src/subdir.mk
-include Library/lsejtag/subdir.mk
-include Library/cherryusb/core/subdir.mk
-include Library/cherryusb/class/wireless/subdir.mk
-include Library/cherryusb/class/video/subdir.mk
-include Library/cherryusb/class/vendor/wifi/subdir.mk
-include Library/cherryusb/class/vendor/serial/subdir.mk
-include Library/cherryusb/class/vendor/net/subdir.mk
-include Library/cherryusb/class/template/subdir.mk
-include Library/cherryusb/class/msc/subdir.mk
-include Library/cherryusb/class/hub/subdir.mk
-include Library/cherryusb/class/hid/subdir.mk
-include Library/cherryusb/class/dfu/subdir.mk
-include Library/cherryusb/class/cdc/subdir.mk
-include Library/cherryusb/class/audio/subdir.mk
-include Library/cherryusb/class/aoa/subdir.mk
-include Library/cherryusb/class/adb/subdir.mk
-include Library/cherryusb/subdir.mk
-include Debug/subdir.mk
-include Core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
CH32LoongsonEJTAG.hex \

SECONDARY_LIST += \
CH32LoongsonEJTAG.lst \

SECONDARY_SIZE += \
CH32LoongsonEJTAG.siz \


# All Target
all: 
	$(MAKE) --no-print-directory main-build 

main-build: CH32LoongsonEJTAG.elf secondary-outputs

# Tool invocations
CH32LoongsonEJTAG.elf: $(OBJS) $(USER_OBJS)
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Ofast -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -T "d:/prj/embedded/ch32v203f6p6-blink/CH32V203F6P6/Ld/Link.ld" -nostartfiles -Xlinker --gc-sections -Wl,-Map,"CH32LoongsonEJTAG.map" --specs=nano.specs --specs=nosys.specs -o "CH32LoongsonEJTAG.elf" $(OBJS) $(USER_OBJS) $(LIBS)
CH32LoongsonEJTAG.hex: CH32LoongsonEJTAG.elf
	@	riscv-none-embed-objcopy -O ihex "CH32LoongsonEJTAG.elf" "CH32LoongsonEJTAG.hex"
CH32LoongsonEJTAG.lst: CH32LoongsonEJTAG.elf
	@	riscv-none-embed-objdump --all-headers --demangle --disassemble -M xw "CH32LoongsonEJTAG.elf" > "CH32LoongsonEJTAG.lst"
CH32LoongsonEJTAG.siz: CH32LoongsonEJTAG.elf
	riscv-none-embed-size --format=berkeley "CH32LoongsonEJTAG.elf"

# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(S_DEPS)$(S_UPPER_DEPS)$(ASM_DEPS)$(ASM_UPPER_DEPS)$(C_DEPS) CH32LoongsonEJTAG.elf

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets