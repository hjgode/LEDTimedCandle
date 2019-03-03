# simple AVR Makefile
#
# written by michael cousins (http://github.com/mcous)
# released to the public domain

# Makefile
#
# targets:
#   all:    compiles the source code
#   test:   tests the isp connection to the mcu
#   flash:  writes compiled hex file to the mcu's flash memory
#   fuse:   writes the fuse bytes to the MCU
#   disasm: disassembles the code for debugging
#   clean:  removes all .hex, .elf, and .o files in the source code and library directories

# parameters (change this stuff accordingly)
# project name
PRJ = LEDTimedCandle
# avr mcu
MCU = attiny13a
# mcu clock frequency
CLK = 1200000
# avr programmer (and port if necessary)
# e.g. PRG = usbtiny -or- PRG = arduino -P /dev/tty.usbmodem411
AVRMCU = attiny13
PRG = stk500v2
AVRPORT = -P/dev/ttyUSB0
# fuse values for avr: low, high, and extended
# these values are from an Arduino Uno (ATMega328P)
# see http://www.engbedded.com/fusecalc/ for other MCUs and options
LFU = 0x2A
HFU = 0xFF
LCK = 0x3f
# program source files (not including external libraries)
SRC = $(PRJ).c ./i2c/i2c.c
# where to look for external libraries (consisting of .c/.cpp files and .h files)
# e.g. EXT = ../../EyeToSee ../../YouSART
#EXT = ./i2c
EXT =


#################################################################################################
# \/ stuff nobody needs to worry about until such time that worrying about it is appropriate \/ #
#################################################################################################

# include path
INCLUDE := $(foreach dir, $(EXT), -I$(dir))
# c flags
CFLAGS    = -Wall -Os -DF_CPU=$(CLK) -mmcu=$(MCU) $(INCLUDE) -std=gnu99
# any aditional flags for c++
CPPFLAGS =

# executables
AVRDUDE = avrdude -v -c $(PRG) -p $(AVRMCU)
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE    = avr-size --format=avr --mcu=$(MCU)
CC      = avr-gcc

# generate list of objects
CFILES    = $(filter %.c, $(SRC))
EXTC     := $(foreach dir, $(EXT), $(wildcard $(dir)/*.c))
CPPFILES  = $(filter %.cpp, $(SRC))
EXTCPP   := $(foreach dir, $(EXT), $(wildcard $(dir)/*.cpp))
OBJ       = $(CFILES:.c=.o) $(EXTC:.c=.o) $(CPPFILES:.cpp=.o) $(EXTCPP:.cpp=.o)

# user targets
# compile all files
all: $(PRJ).hex

# test programmer connectivity
test:
	$(AVRDUDE) -v

# flash program to mcu
flash: all
	$(AVRDUDE) -U flash:w:$(PRJ).hex:i

# write fuses to mcu
fuse:
	$(AVRDUDE) -B32 $(AVRPORT) -e -Ulock:w:$(LCK):m -Uhfuse:w:$(HFU):m -Ulfuse:w:$(LFU):m
#-U lfuse:w:$(LFU):m -U hfuse:w:$(HFU):m -U efuse:w:$(EFU):m
#/home/hgode/bin/arduino-1.8.5/hardware/tools/avr/bin/avrdude -C/home/hgode/.arduino15/packages/MicroCore/hardware/avr/1.0.4/avrdude.conf 
#-v -pattiny13 -cstk500v2 -B32 -P/dev/ttyUSB0 -e -Ulock:w:0x3f:m -Uhfuse:w:0xff:m -Ulfuse:w:0x2A:m 

# generate disassembly files for debugging
disasm: $(PRJ).elf
	$(OBJDUMP) -d $(PRJ).elf

# remove compiled files
clean:
	rm -f *.hex *.elf *.o
	$(foreach dir, $(EXT), rm -f $(dir)/*.o;)

# other targets
# objects from c files, Compile and no Linking (-c)
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

# objects from c++ files
.cpp.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# elf file
$(PRJ).elf: $(OBJ)
	$(CC) $(CFLAGS) -o $(PRJ).elf $(OBJ)

# hex file
$(PRJ).hex: $(PRJ).elf
	rm -f $(PRJ).hex
	$(OBJCOPY) -j .text -j .data -O ihex $(PRJ).elf $(PRJ).hex
	$(SIZE) $(PRJ).elf
