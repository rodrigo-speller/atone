# project targets

all: optparse
optparse: build
build: before-build build-core after-build
rebuild: clean build
install: before-install install-core after-install

.PHONY: clean build rebuild optparse

# project configuration

INCLUDE_PATH+=$(SRCDIR)
INCLUDE_PATH+=include
ARFLAGS=rc
CXXFLAGS+=-Wall
CXXFLAGS+=-fdiagnostics-color=always
CXXFLAGS+=-std=c++17
CXXFLAGS+=$(foreach d, $(INCLUDE_PATH), -I$d)

#-----------------------------------------------------------------------

DEBUG?=0
VERBOSE?=0

ifneq ($(DEBUG), 0)
    TARGET=debug
    CXXFLAGS+=-g
    CXXFLAGS+=-Og
else
    TARGET=release
    CXXFLAGS+=-Os
endif

ifneq ($(VERBOSE), 0)
    CXXFLAGS+=-v
endif

# default directories

SRCDIR?=src
INSTALLDIR?=lib
BUILDDIR?=build
TARGETDIR?=$(BUILDDIR)/$(TARGET)
OUTDIR?=$(TARGETDIR)/bin
OBJDIR?=$(TARGETDIR)/obj

# cleanup

CLEAN+=$(OUTDIR)
CLEAN+=$(OBJDIR)

# artifacts

SOURCES=$(shell find "$(SRCDIR)" -name *.cpp)
OBJECTS=$(SOURCES:%.cpp=$(OBJDIR)/%.o)

# internal targets

# BUILD

before-build:
	@echo "Building..."
	@echo "  Target: $(TARGET)"
	@echo "  Output directory: $(OUTDIR)"
	@-mkdir -p $(OUTDIR) $(dir $(OBJECTS))

build-core: $(OUTDIR)/optparse.a

$(OUTDIR)/optparse.a: $(OBJECTS)
	@echo "  Creating library $@"
	@$(AR) $(ARFLAGS) $(OUTDIR)/optparse.a $(OBJECTS)

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	@echo "  Compiling $<"
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

after-build:
	@echo "Build completed"

# INSTALL

before-install: build
	@echo "Installing..."

install-core: $(INSTALLDIR)/liboptparse.a

$(INSTALLDIR)/liboptparse.a: $(OUTDIR)/optparse.a
	@cp $< $@
	@echo "  $@"

after-install:
	@echo "Installation completed"

#CLEAN

clean:
	@-rm -rv -- $(CLEAN) || true

clean-all:
	@-rm -rv -- $(BUILDDIR) || true

$(TARGETDIR)/.marker: makefile
	@touch $@
	$(MAKE) clean

-include $(OBJECTS:.o=.d)
-include $(TARGETDIR)/.marker
