# project targets

all: liboptparse
liboptparse: build
build: before-build build-core after-build
rebuild: clean build

.PHONY: clean build rebuild liboptparse

# project configuration

INCLUDE_PATH+=$(SRCDIR)
INCLUDE_PATH+=include
ARFLAGS=rc
CXXFLAGS+=-Wall
CXXFLAGS+=-fdiagnostics-color=always
CXXFLAGS+=-std=c++17
CXXFLAGS+=-MMD

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
    CXXFLAGS+=-g
    CXXFLAGS+=-Og
else
    TARGET=release
    CXXFLAGS+=-Os
endif

ifneq ($(VERBOSE), 0)
    CXXFLAGS+=-v
endif

# artifacts

SOURCES=$(shell find "$(SRCDIR)" -name *.cpp)
OBJECTS=$(SOURCES:%.cpp=$(OBJDIR)/%.o)

# expand variables

CXXFLAGS+=$(foreach d, $(INCLUDE_PATH), -I$d)

# internal targets

# BUILD

before-build:
	@echo "Building..."
	@echo "  Target: $(TARGET)"
	@echo "  Output directory: $(OUTDIR)"
	@-mkdir -p $(OUTDIR) $(dir $(OBJECTS))

build-core: $(OUTDIR)/liboptparse.a

$(OUTDIR)/liboptparse.a: $(OBJECTS)
	@echo "  Creating library $@"
	@$(AR) $(ARFLAGS) $(OUTDIR)/liboptparse.a $(OBJECTS)

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	@echo "  Compiling $<"
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

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
