# GCC defines
CC=gcc
STRIP=strip
RM=del
CAT=type
O=o
RES=o

%.$(RES): %.rc
	windres -i $< -o $@


CFLAGS=-Wall -pedantic -I dlglib -I openssl-0.9.7d/outinc
TARGET=ntpwedit.exe
OBJS=main.$(O) ntpw.$(O) ntreg.$(O) message.$(O) dlgpass.$(O) version.$(O) \
     dlgabout.$(O) log.$(O)
DEBUG_OBJS=
LIBFILES=dlglib/libdlglib.a openssl-0.9.7d/out/libcrypto.a
LIBS=-L dlglib -ldlglib -L openssl-0.9.7d/out -lcrypto -lcomctl32
LDFLAGS=-mwindows -Wl,-Map,$(TARGET).map
RESS=resource.$(RES)

.PHONY: build release dep clean rebuild perldep gccdep dlglib debug openssl

debug: OBJS+= $(DEBUG_OBJS)
debug: CFLAGS+= -DDEBUG -g
debug: $(DEBUG_OBJS) build

rebuild: clean debug

release: CFLAGS+= -DRELEASE
release: clean build
	$(STRIP) $(TARGET)

build: dlglib $(TARGET)

$(TARGET): $(OBJS) $(RESS) $(LIBFILES)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(RESS) $(LIBS)
	-perl map.pl $(TARGET).map > $(TARGET).map.txt

resource.$(RES): resource.rc app.ico $(TARGET).manifest

dlglib:
	$(MAKE) -C $@ $(MAKECMDGOALS)

dep: dlglib gccdep

perldep:
	perl makedep.pl $(O) > Makefile.dep

gccdep:
	gcc $(CFLAGS) -MM *.c > Makefile.dep

include Makefile.dep

clean: dlglib
	-$(RM) $(TARGET)
	-$(RM) *.$(O)
	-$(RM) *.$(RES)
	-$(RM) *.bak
	-$(RM) *.map
	-$(RM) *.mem
	-$(RM) *.map.txt

