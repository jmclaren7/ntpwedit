# GCC defines
CC=gcc
RM=cmd /c del
AR=ar
RANLIB=ranlib
O=o
RES=o
LIB=a
CFLAGS=-Wall -pedantic
LDFLAGS=-mwindows

TARGET=libdlglib.$(LIB)
OBJS=dialogs.$(O) unicode.$(O) dlgpack.$(O)\
     ctl_checkbox.$(O) \
     ctl_groupbox.$(O) \
     ctl_button.$(O) \
     ctl_listview.$(O) \
     ctl_edit.$(O) \
     ctl_label.$(O) \
     ctl_image.$(O) \
     ctl_richview.$(O) \
     ctl_hotkey.$(O) \
     ctl_progress.$(O)

.PHONY: build release dep clean rebuild build.h perldep gccdep

debug: CFLAGS+= -DDEBUG
debug: build

release: CFLAGS+= -DRELEASE
release: clean build

rebuild: clean debug

build: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) r $@ $^
	$(RANLIB) $@

dep: gccdep

perldep:
	perl makedep.pl $(O) > Makefile.dep

gccdep:
	gcc -MM *.c > Makefile.dep

-include Makefile.dep

clean:
	-$(RM) $(TARGET) *.$(O) *.$(RES) *.bak *.map *.mem
