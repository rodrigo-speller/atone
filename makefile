# project targets

all: atone
atone: build
build: before-build build-core after-build
rebuild: clean build

.PHONY: clean build rebuild atone

# project configuration

LIBRARY+=yaml-cpp

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

#-----------------------------------------------------------------------

DEBUG?=0
VERBOSE?=0

ifneq ($(DEBUG), 0)
	TARGET=debug
    CXXFLAGS+=-g
	CXXFLAGS+=-Og
else
	TARGET=release
    CXXFLAGS+=-O3
endif

ifneq ($(VERBOSE), 0)
    CXXFLAGS+=-v
endif

# default directories

SRCDIR?=src
BUILDDIR?=build
TARGETDIR?=$(BUILDDIR)/$(TARGET)
OUTDIR?=$(TARGETDIR)/bin
OBJDIR?=$(TARGETDIR)/obj
CLEAN+=$(TARGETDIR)

# artifacts

SOURCES=$(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/**/*.cpp)
OBJECTS=$(SOURCES:%.cpp=$(OBJDIR)/%.o)

# internal targets

before-build:
	@echo "Building..."
	@echo "  Target: $(TARGET)"
	@echo "  Output directory: $(OUTDIR)"
	@-mkdir -p $(OUTDIR) $(dir $(OBJECTS))

build-core: $(OUTDIR)/atone

$(OUTDIR)/atone: $(OBJECTS)
	@echo "  Linking $@"
	@$(CXX) $(LDFLAGS) -o $(OUTDIR)/atone $(OBJECTS) $(LDLIBS)

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	@echo "  Compiling $<"
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

after-build:
	@echo "Build completed"

clean:
	@-rm -rv -- $(CLEAN) || true

clean-all:
	@-rm -rv -- $(BUILDDIR) || true

-include $(OBJECTS:.o=.d)
