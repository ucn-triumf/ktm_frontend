# Makefile for KTM MIDAS frontend...

#
TARGET = fektm.exe

# Yocto cross-compiler setup
POKY_DIR=/ladd/data0/olchansk/MityARM/Yocto/opt/poky/1.5/sysroots
POKY_EXT=arm-poky-linux-gnueabi
POKY_HOST=x86_64-pokysdk-linux
POKY_ARM=armv7a-vfp-neon-poky-linux-gnueabi
GCC_BIN=$(POKY_DIR)/$(POKY_HOST)/usr/bin/$(POKY_EXT)/$(POKY_EXT)-

#
CFLAGS = -static -g -Wall -std=c++11 --sysroot $(POKY_DIR)/$(POKY_ARM) -I$(MIDASSYS)/include -I../altera_hps/hwlib/include
LDFLAGS =  -g -Wall --sysroot  $(POKY_DIR)/$(POKY_ARM) -Wl,-rpath,$(POKY_DIR)/$(POKY_ARM)/usr/lib -pthread -lutil -lrt -ldl
CC = $(GCC_BIN)gcc
ARCH= arm

# MIDAS library
MIDASLIBS = $(MIDASSYS)/linux-arm/lib/libmidas.a
LIB_DIR = $(MIDASSYS)/linux-arm/lib

build: $(TARGET)
$(TARGET): fektm.o 
	$(CC) $(LDFLAGS)  $(LIB_DIR)/mfe.o $(MIDASLIBS) $^ -o $@  -lstdc++ 
	scp $(TARGET) root@ucn-ktm01:midascode/.
%.o : %.cxx
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(TARGET) *.a *.o *~ 
