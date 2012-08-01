##
## makefile
##
## Used for compiling meter monitor firmware
## Target: ATMEGA8 :: AVR-GCC :: AVRDUDE
##
## Written by Tony Myatt - Quantum Torque 2007
##
F_CPU = 8000000
# Makefile configuration
MCU = atmega32
FORMAT = ihex
TARGET = Test
SRC = $(TARGET).c 
ASRC = 

# Compiler/Linker and other programs
CC = avr-gcc
OBJCOPY = avr-objcopy
AVRSIZE = avr-size
AVRDUDE = avrdude

# Compiler/Linker flags
CFLAGS += -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -Wall -Wstrict-prototypes -Wa,-adhlns=$(<:.c=.lst) -std=gnu99
LDFLAGS = -Wl,-Map=$(TARGET).map,--cref
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS)

# Define all object files
OBJ = $(SRC:.c=.o) $(ASRC:.S=.o)

# Programming support using avrdude
AVRDUDE_FLAGS = -p $(MCU) -P lpt1 -c stk200
AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex

# Targets for building
all: $(TARGET).elf $(TARGET).hex size

program: $(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)
	
clean:
	rm -f $(TARGET).elf $(TARGET).hex $(OBJ:.o=.lst) $(OBJ)

size:
	$(AVRSIZE) -A $(TARGET).elf

# Rules for output files
%.hex: %.elf
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

%.elf: $(OBJ)
	$(CC) $(ALL_CFLAGS) $(OBJ) --output $@ $(LDFLAGS)

%.o : %.c
	$(CC) -c $(ALL_CFLAGS) $< -o $@
	
# Object file dependencies
main.o: usart.h
