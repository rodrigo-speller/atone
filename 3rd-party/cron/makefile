# project targets

all: libcron
libcron: build
build: before-build build-core after-build
rebuild: clean build

.PHONY: clean build rebuild libcron

# project configuration

INCLUDE_PATH+=$(SRCDIR)
INCLUDE_PATH+=include
ARFLAGS=rc
#CFLAGS+=-Wall
CFLAGS+=-fdiagnostics-color=always
CFLAGS+=-std=c17
CFLAGS+=-MMD

#-----------------------------------------------------------------------

# default directories

SRCDIR?=src
BUILDDIR?=build
TARGETDIR?=$(BUILDDIR)/$(TARGET)
OUTDIR?=$(TARGETDIR)/bin
OBJDIR?=$(TARGETDIR)/obj

# cleanup

CLEAN+=$(OUTDIR)
CLEAN+=$(OBJDIR)

# input parameters

DEBUG?=0
VERBOSE?=0

ifneq ($(DEBUG), 0)
    TARGET=debug
    CFLAGS+=-g
    CFLAGS+=-Og
else
    TARGET=release
    CFLAGS+=-Os
endif

ifneq ($(VERBOSE), 0)
    CFLAGS+=-v
endif

# artifacts

SOURCES=$(shell find "$(SRCDIR)" -name *.c)
OBJECTS=$(SOURCES:%.c=$(OBJDIR)/%.o)

# expand variables

CFLAGS+=$(foreach d, $(INCLUDE_PATH), -I$d)

# internal targets

# BUILD

before-build:
	@echo "Building..."
	@echo "  Target: $(TARGET)"
	@echo "  Output directory: $(OUTDIR)"
	@-mkdir -p $(OUTDIR) $(dir $(OBJECTS))

build-core: $(OUTDIR)/libcron.a

$(OUTDIR)/libcron.a: $(OBJECTS)
	@echo "  Creating library $@"
	@$(AR) $(ARFLAGS) $(OUTDIR)/libcron.a $(OBJECTS)

$(OBJECTS): $(OBJDIR)/%.o: %.c
	@echo "  Compiling $<"
	@$(CC) $(CFLAGS) -o $@ -c $<

after-build:
	@echo "Build completed"

#CLEAN

clean:
	@-rm -rv -- $(CLEAN) || true

clean-all:
	@-rm -rv -- $(BUILDDIR) || true

$(TARGETDIR)/.marker: makefile
	@-mkdir -p $(TARGETDIR)
	@touch $@
	$(MAKE) clean

# includes

-include $(OBJECTS:.o=.d)
-include $(TARGETDIR)/.marker
