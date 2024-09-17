CFLAGS ?= -W -Wall -Wextra -Wundef -Wshadow -Wdouble-promotion -Wformat-truncation 
CFLAGS += -g3 -Os -fno-common -ffunction-sections -fdata-sections -Wno-shadow
CFLAGS += -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16
LDFLAGS ?= -Tlink.ld -nostartfiles -nostdlib --specs nano.specs -lc -lgcc -Wl,--gc-sections -Wl,-Map=$@.map
SOURCES = hal.c main.c

build: firmware.bin

firmware.elf: link.ld $(SOURCES) 
	arm-none-eabi-gcc $(SOURCES) $(CFLAGS) $(CFLAGS_EXTRA) $(LDFLAGS) -o $@

firmware.bin: firmware.elf
	arm-none-eabi-objcopy -O binary $< $@

flash: firmware.bin
	st-flash --reset write $< 0x8000000

clean:
	$(RM) firmware.*

format:
	clang-format -Werror --style="{BasedOnStyle: llvm, IndentWidth: 4}" -i *.c *.h
