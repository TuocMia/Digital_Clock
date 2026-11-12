STD_LIB = ../STM32F10x_StdPeriph_Lib_V3.5.0/Libraries
STD_DEV = $(STD_LIB)/CMSIS/CM3/DeviceSupport/ST/STM32F10x
STD_DRV = $(STD_LIB)/STM32F10x_StdPeriph_Driver

CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS  = -mcpu=cortex-m3 -mthumb -Os -g -Wall -ffunction-sections -fdata-sections
CFLAGS += -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER
CFLAGS += -I. -Ibsp -Ihal/inc
CFLAGS += -I$(STD_LIB)/CMSIS/CM3/CoreSupport -I$(STD_DEV) -I$(STD_DRV)/inc

# Chi build main.c + bsp + hal + StdPeriph (khong FreeRTOS, khong app/)
SRCS  = main.c
SRCS += $(wildcard bsp/*.c)
SRCS += $(wildcard hal/src/*.c)
SRCS += $(STD_DRV)/src/stm32f10x_rcc.c
SRCS += $(STD_DRV)/src/stm32f10x_gpio.c
SRCS += $(STD_DRV)/src/stm32f10x_spi.c

ASM     = $(STD_DEV)/startup/gcc_ride7/startup_stm32f10x_md.s
LDFLAGS = -Tbsp/linker.ld -nostartfiles --specs=nosys.specs -Wl,--gc-sections

all: firmware.bin

firmware.elf: $(SRCS) $(ASM) bsp/linker.ld
	$(CC) $(CFLAGS) $(LDFLAGS) $(ASM) $(SRCS) -o firmware.elf

firmware.bin: firmware.elf
	$(OBJCOPY) -O binary firmware.elf firmware.bin

clean:
	rm -f *.o *.elf *.bin

flash: firmware.bin
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program firmware.bin verify reset exit 0x08000000"

.PHONY: all clean flash