# project targets

all: atone
atone: build
build: before-build 3rd-party build-core after-build
rebuild: clean build

.PHONY: clean build rebuild atone

# project configuration

PRECOMPILE+=src/atone.h

LIBRARY+=optparse
LIBRARY+=yaml-cpp

LIBRARY_PATH+=3rd-party/optparse/$(BUILDDIR)/$(TARGET)/bin
LIBRARY_PATH+=3rd-party/yaml-cpp/$(BUILDDIR)/$(TARGET)/bin

INCLUDE_PATH+=$(SRCDIR)
INCLUDE_PATH+=3rd-party/optparse/include
INCLUDE_PATH+=3rd-party/yaml-cpp/include

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
PCHDIR?=$(TARGETDIR)/pch

#cleanup

CLEAN+=$(OUTDIR)
CLEAN+=$(OBJDIR)
CLEAN+=$(PCHDIR)

# input parameters

DEBUG?=0
VERBOSE?=0

ifneq ($(DEBUG), 0)
    TARGET=debug
    CXXFLAGS+=-g
    CXXFLAGS+=-Og
else
    TARGET=release
    CXXFLAGS+=-Os -ffunction-sections -fdata-sections
    LDFLAGS+=-Wl,--gc-sections -s
endif

ifneq ($(VERBOSE), 0)
    CXXFLAGS+=-v
endif

ifdef ATONE_BUILD_VERSION
    CXXFLAGS+=-D "ATONE_BUILD_VERSION=\"$(ATONE_BUILD_VERSION)\""
endif

ifdef ATONE_BUILD_NUMBER
    CXXFLAGS+=-D "ATONE_BUILD_NUMBER=\"$(ATONE_BUILD_NUMBER)\""
endif

# artifacts

SOURCES=$(shell find "$(SRCDIR)" -name *.cpp)
OBJECTS=$(SOURCES:%.cpp=$(OBJDIR)/%.o)
PCHS=$(PRECOMPILE:%=$(PCHDIR)/%.gch)

# expand variables

CXXFLAGS+=$(foreach d, $(INCLUDE_PATH), -I$d)
LDFLAGS+=$(foreach d, $(LIBRARY_PATH), -L$d)
LDLIBS+=$(foreach d, $(LIBRARY), -l$d)
PCHINCS+=$(foreach d, $(PCHDIR)/$(PRECOMPILE), -include $d)
PCHFLAGS+=$(CXXFLAGS) -x c++-header

# internal targets

# BUILD

before-build:
	@echo "Building..."
	@echo "  Target: $(TARGET)"
	@echo "  Output directory: $(OUTDIR)"
	@-mkdir -p $(OUTDIR) $(dir $(OBJECTS)) $(dir $(PCHDIR)/$(PRECOMPILE))

build-core: $(PCHS) $(OUTDIR)/atone

$(OUTDIR)/atone: $(OBJECTS)
	@echo "  Linking $@"
	@$(CXX) $(LDFLAGS) -o $(OUTDIR)/atone $(OBJECTS) $(LDLIBS)

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	@echo "  Compiling $<"
	@$(CXX) $(PCHINCS) $(CXXFLAGS) -o $@ -c $<

$(PCHS): $(PCHDIR)/%.gch: %
	@echo "  Precompiling $<"
	@$(CXX) $(PCHFLAGS) -c $< -o $(PCHDIR)/$<.gch

after-build:
	@echo "Build completed"

# 3rd-party

3rd-party: \
  3rd-party/optparse/$(BUILDDIR)/$(TARGET)/bin/liboptparse.a \
  3rd-party/yaml-cpp/$(BUILDDIR)/$(TARGET)/bin/libyaml-cpp.a

3rd-party/optparse/$(BUILDDIR)/$(TARGET)/bin/liboptparse.a:
	@echo "Building optparse"
	@$(MAKE) -C 3rd-party/optparse

3rd-party/yaml-cpp/$(BUILDDIR)/$(TARGET)/bin/libyaml-cpp.a:
	@echo "Building yaml-cpp"
	@$(MAKE) -C 3rd-party/yaml-cpp

# CLEAN

clean:
	@-rm -rv -- $(CLEAN) || true

clean-all:
	@-rm -rv -- $(BUILDDIR) || true
	@$(MAKE) -C 3rd-party/optparse clean-all
	@$(MAKE) -C 3rd-party/yaml-cpp clean-all

$(TARGETDIR)/.marker: makefile
	@-mkdir -p $(TARGETDIR)
	@touch $@
	$(MAKE) clean

# includes

-include $(OBJECTS:.o=.d)
-include $(PCHS:.gch=.d)
-include $(TARGETDIR)/.marker
