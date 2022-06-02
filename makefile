# project targets

all: atone
atone: build
build: before-build build-core after-build
rebuild: clean build

.PHONY: clean build rebuild atone

# project configuration

PRECOMPILE+=src/atone.h

LIBRARY+=optparse
LIBRARY+=yaml-cpp

LIBRARY_PATH+=3rd-party/optparse/lib
LIBRARY_PATH+=3rd-party/yaml-cpp/lib

INCLUDE_PATH+=$(SRCDIR)
INCLUDE_PATH+=3rd-party/optparse/include
INCLUDE_PATH+=3rd-party/yaml-cpp/include

CXXFLAGS+=-Wall
CXXFLAGS+=-fdiagnostics-color=always
CXXFLAGS+=-std=c++17
CXXFLAGS+=-MMD
CXXFLAGS+=$(foreach d, $(INCLUDE_PATH), -I$d)
LDFLAGS+=$(foreach d, $(LIBRARY_PATH), -L$d)
LDLIBS+=$(foreach d, $(LIBRARY), -l$d)
PCHINCS+=$(foreach d, $(PCHDIR)/$(PRECOMPILE), -include $d)
PCHFLAGS+=$(CXXFLAGS) -x c++-header

#-----------------------------------------------------------------------

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

# artifacts

SOURCES=$(shell find "$(SRCDIR)" -name *.cpp)
OBJECTS=$(SOURCES:%.cpp=$(OBJDIR)/%.o)
PCHS=$(PRECOMPILE:%=$(PCHDIR)/%.gch)

# internal targets
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

clean:
	@-rm -rv -- $(CLEAN) || true

clean-all:
	@-rm -rv -- $(BUILDDIR) || true

$(TARGETDIR)/.marker: makefile
	@touch $@
	$(MAKE) clean

-include $(OBJECTS:.o=.d)
-include $(PCHS:.gch=.d)
-include $(TARGETDIR)/.marker
