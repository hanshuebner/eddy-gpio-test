CROSS		= /opt/lemonix/cdt/bin/arm-linux-
LDFLAGS 	+= -L/opt/lemonix/cdt/lib -L/opt/lemonix/cdt/bin
IFLAGS 		+= -I/opt/lemonix/cdt/include  -I./include -I../lemonix_2.1.0.1/include/asm/arch/
CXXFLAGS 	= -O2 -g -Wall $(IFLAGS) # -Wno-nonnull

CC	= $(CROSS)gcc
CXX	= $(CROSS)g++
STRIP	= $(CROSS)strip
AR	= $(CROSS)ar

LIBS	= -lrt SB_APIs/SB_APIs.a

TARGETS	= gpio

all: $(TARGETS)
	$(STRIP) gpio
	ftp -u ftp://root:root@eddy/ gpio

gpio: gpio.o 

clean:
	rm -f *.o $(TARGETS)
