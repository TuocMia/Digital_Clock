STD_LIB = ../STM32F10x_StdPeriph_Lib_V3.5.0/Libraries
STD_DEV = $(STD_LIB)/CMSIS/CM3/DeviceSupport/ST/STM32F10x
RTOS    = middlewares/FreeRTOS

CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS  = -mcpu=cortex-m3 -mthumb -Os -g -Wall
CFLAGS += -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER
CFLAGS += -fno-unwind-tables -fno-asynchronous-unwind-tables
CFLAGS += -I. -Ibsp -Ihal/inc -Iapp/inc
CFLAGS += -I$(RTOS)/include -I$(RTOS)/portable/GCC/ARM_CM3
CFLAGS += -I$(STD_LIB)/CMSIS/CM3/CoreSupport
CFLAGS += -I$(STD_DEV)
CFLAGS += -I$(STD_LIB)/STM32F10x_StdPeriph_Driver/inc

MAIN   ?= main.c
SRCS  = $(MAIN)
SRCS += $(wildcard bsp/*.c)
SRCS += $(wildcard hal/src/*.c)
SRCS += $(wildcard app/src/*.c)
SRCS += $(RTOS)/tasks.c $(RTOS)/list.c $(RTOS)/queue.c $(RTOS)/event_groups.c
SRCS += $(RTOS)/portable/GCC/ARM_CM3/port.c
SRCS += $(RTOS)/portable/MemMang/heap_4.c
SRCS += $(STD_DEV)/system_stm32f10x.c
SRCS += $(STD_LIB)/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c
SRCS += $(STD_LIB)/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c

ASM = $(STD_DEV)/startup/gcc_ride7/startup_stm32f10x_md.s

LDFLAGS = -Tbsp/linker.ld -nostartfiles --specs=nosys.specs

all: firmware.bin

firmware.elf: $(SRCS) $(ASM) bsp/linker.ld
	$(CC) $(CFLAGS) $(LDFLAGS) $(ASM) $(SRCS) -o firmware.elf

firmware.bin: firmware.elf
	$(OBJCOPY) -O binary firmware.elf firmware.bin

clean:
	rm -f *.o *.elf *.bin

flash: firmware.bin
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program firmware.bin verify reset exit 0x08000000"
