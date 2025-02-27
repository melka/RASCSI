.DEFAULT_GOAL: all

## Optional build flags:
##   CROSS_COMPILE : Specify which compiler toolchain to use.
##                   To cross compile set this accordingly, e.g. to:
##                   arm-linux-gnueabihf-
CROSS_COMPILE =

CXX = $(CROSS_COMPILE)g++
AR = $(CROSS_COMPILE)ar
RANLIB = $(CROSS_COMPILE)ranlib

##   DEBUG=1  : A Debug build includes the debugger symbols
##              and disables compiler optimization. Typically,
##              this is only used by developers.
DEBUG ?= 0
ifeq ($(DEBUG), 1)
	# Debug compiler flags
	CXXFLAGS += -O0 -g -Wall -Wextra -DDEBUG
	BUILD_TYPE = Debug
else
	# Release compiler flags
	CXXFLAGS += -O3 -Wall -Werror -Wextra -DNDEBUG
	BUILD_TYPE = Release
endif
ifeq ("$(shell uname -s)","Linux")
	# -Wno-psabi might not work on non-Linux platforms
	CXXFLAGS += -Wno-psabi
endif

# Depending on the GCC version the compilation flags differ
GCCVERSION10 := $(shell expr `$(CXX) -dumpversion` \>= 10)

CXXFLAGS += -std=c++17 -iquote . -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -MD -MP

## EXTRA_FLAGS : Can be used to pass special purpose flags
CXXFLAGS += $(EXTRA_FLAGS)

ifeq "$(GCCVERSION10)" "1"
	CXXFLAGS += -DFMT_HEADER_ONLY
endif


##   CONNECT_TYPE=FULLSPEC : Specify the type of PiSCSI board type
##              that you are using. The typical options are 
##              STANDARD or FULLSPEC. The default is FULLSPEC
##              * THIS IS TYPICALLY THE ONLY COMPILE OPTION YOU 
##              * NEED TO SPECIFY
# If its not specified, build for FULLSPEC configuration
CONNECT_TYPE ?= FULLSPEC

ifdef CONNECT_TYPE
CXXFLAGS += -DCONNECT_TYPE_$(CONNECT_TYPE)
endif

PISCSI = piscsi
SCSICTL = scsictl
SCSIDUMP = scsidump
SCSIMON = scsimon
PISCSI_TEST = piscsi_test
SCSILOOP = scsiloop

SYSTEMD_CONF = /etc/systemd/system/piscsi.service
RSYSLOG_CONF = /etc/rsyslog.d/piscsi.conf
RSYSLOG_LOG  = /var/log/piscsi.log

USR_LOCAL_BIN = /usr/local/bin
MAN_PAGE_DIR  = /usr/local/man/man1
DOC_DIR = ../doc
COVERAGE_DIR = coverage
COVERAGE_FILE = piscsi.dat
OS_FILES = ../os_integration

OBJDIR := obj/$(shell echo $(CONNECT_TYPE) | tr '[:upper:]' '[:lower:]')
BINDIR := bin/$(shell echo $(CONNECT_TYPE) | tr '[:upper:]' '[:lower:]')

BIN_ALL = \
	$(BINDIR)/$(PISCSI) \
	$(BINDIR)/$(SCSICTL) \
	$(BINDIR)/$(SCSIMON) \
	$(BINDIR)/$(SCSIDUMP) \
	$(BINDIR)/$(SCSILOOP)

SRC_PROTOC = piscsi_interface.proto

SRC_GENERATED = $(GENERATED_DIR)/piscsi_interface.pb.cpp

SRC_PROTOBUF = \
	shared/protobuf_util.cpp \
	shared/protobuf_serializer.cpp

SRC_SHARED = \
	shared/piscsi_version.cpp \
	shared/piscsi_util.cpp

SRC_PISCSI_CORE = $(shell find ./piscsi -name '*.cpp')
SRC_PISCSI_CORE += $(shell find ./controllers -name '*.cpp')
SRC_PISCSI_CORE += $(shell find ./devices -name '*.cpp')
SRC_PISCSI_CORE += $(shell find ./hal -name '*.cpp')

SRC_PISCSI = piscsi.cpp

SRC_SCSIMON = scsimon.cpp
SRC_SCSIMON += $(shell find ./monitor -name '*.cpp')
SRC_SCSIMON += $(shell find ./hal -name '*.cpp')

SRC_SCSICTL_CORE = $(shell find ./scsictl -name '*.cpp')

SRC_SCSICTL = scsictl.cpp

SRC_SCSIDUMP = scsidump.cpp
SRC_SCSIDUMP += $(shell find ./scsidump -name '*.cpp')
SRC_SCSIDUMP += $(shell find ./hal -name '*.cpp')

SRC_PISCSI_TEST = $(shell find ./test -name '*.cpp')
SRC_PISCSI_TEST += $(shell find ./scsidump -name '*.cpp')
SRC_PISCSI_TEST += $(shell find ./monitor -name '*.cpp')

SRC_SCSILOOP = scsiloop.cpp
SRC_SCSILOOP += $(shell find ./scsiloop -name '*.cpp')
SRC_SCSILOOP += $(shell find ./hal -name '*.cpp')

vpath %.h ./ ./shared ./controllers ./devices ./monitor ./hal \
	./hal/boards ./hal/pi_defs ./piscsi ./scsictl ./scsidump \
	./scsiloop
vpath %.cpp ./ ./shared ./controllers ./devices ./monitor ./hal \
	./hal/boards ./hal/pi_defs ./piscsi ./scsictl ./scsidump \
	./scsiloop ./test
vpath %.o ./$(OBJDIR)
vpath ./$(BINDIR)


OBJ_PISCSI_CORE := $(addprefix $(OBJDIR)/,$(notdir $(SRC_PISCSI_CORE:%.cpp=%.o)))
OBJ_PISCSI := $(addprefix $(OBJDIR)/,$(notdir $(SRC_PISCSI:%.cpp=%.o)))
OBJ_SCSICTL_CORE := $(addprefix $(OBJDIR)/,$(notdir $(SRC_SCSICTL_CORE:%.cpp=%.o)))
OBJ_SCSICTL := $(addprefix $(OBJDIR)/,$(notdir $(SRC_SCSICTL:%.cpp=%.o)))
OBJ_SCSIDUMP := $(addprefix $(OBJDIR)/,$(notdir $(SRC_SCSIDUMP:%.cpp=%.o)))
OBJ_SCSIMON := $(addprefix $(OBJDIR)/,$(notdir $(SRC_SCSIMON:%.cpp=%.o)))
OBJ_PISCSI_TEST := $(addprefix $(OBJDIR)/,$(notdir $(SRC_PISCSI_TEST:%.cpp=%.o)))
OBJ_SCSILOOP  := $(addprefix $(OBJDIR)/,$(notdir $(SRC_SCSILOOP:%.cpp=%.o)))
OBJ_SHARED := $(addprefix $(OBJDIR)/,$(notdir $(SRC_SHARED:%.cpp=%.o)))
OBJ_PROTOBUF := $(addprefix $(OBJDIR)/,$(notdir $(SRC_PROTOBUF:%.cpp=%.o)))
OBJ_GENERATED := $(addprefix $(OBJDIR)/,$(notdir $(SRC_GENERATED:%.cpp=%.o)))

GENERATED_DIR := generated

# For the unit tests, the following functions will be "wrapped" by the linker, meaning the
# __wrap_xxxx() function will be called instead of the real function. These linker flags
# should only be used for testing purposes!
TEST_WRAPS = -Wl,--wrap=fopen64

# The following will include all of the auto-generated dependency files (*.d)
# if they exist. This will trigger a rebuild of a source file if a header changes
ALL_DEPS := $(patsubst %.o,%.d,$(OBJ_PISCSI_CORE) $(OBJ_SCSICTL_CORE) $(OBJ_PISCSI) $(OBJ_SCSICTL) $(OBJ_SCSIDUMP) $(OBJ_SCSIMON) $(OBJ_SHARED) $(OBJ_PROTOBUF) $(OBJ_PISCSI_TEST) $(OBJ_SCSILOOP))
-include $(ALL_DEPS)

$(OBJDIR) $(BINDIR):
	@echo "-- Creating directory $@"
	mkdir -p $@

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(SRC_GENERATED) : $(SRC_PROTOC)
	@echo "-- Generating protobuf-based source files"
	mkdir -p $(GENERATED_DIR)
	protoc --cpp_out=$(GENERATED_DIR) $(SRC_PROTOC)
	mv $(GENERATED_DIR)/piscsi_interface.pb.cc $@

$(OBJ_GENERATED) : $(SRC_GENERATED)
	$(CXX) $(CXXFLAGS) -c $< -o $@

## Build Targets:
##   all      : Rebuild all of the executable files and re-generate 
##              the text versions of the manpages
##   docs     : Re-generate the text versions of the man pages
##   test     : Build and run unit tests
##   coverage : Build and run unit tests and create coverage SonarQube files.
##   lcov     : Build and run unit tests and create coverage HTML files.
##              Note that you have to run 'make clean' before switching
##              between coverage and non-coverage builds.
.DEFAULT_GOAL := all
.PHONY: all ALL docs test coverage lcov
all: $(BIN_ALL) docs

test: $(BINDIR)/$(PISCSI_TEST)
	$(BINDIR)/$(PISCSI_TEST)

coverage: CXXFLAGS += --coverage
coverage: test

lcov: CXXFLAGS += --coverage
lcov: test
	lcov -q -c -d . --include '*/cpp/*' -o $(COVERAGE_FILE) --exclude '*/test/*' --exclude '*/interfaces/*' --exclude '*/piscsi_interface.pb*'
	genhtml -q -o $(COVERAGE_DIR) --legend $(COVERAGE_FILE)

docs: $(DOC_DIR)/piscsi_man_page.txt $(DOC_DIR)/scsictl_man_page.txt $(DOC_DIR)/scsimon_man_page.txt $(DOC_DIR)/scsidump_man_page.txt $(DOC_DIR)/scsiloop_man_page.txt

$(SRC_PISCSI_CORE) $(SRC_SCSICTL_CORE) : $(OBJ_GENERATED)

$(BINDIR)/$(PISCSI): $(SRC_GENERATED) $(OBJ_PISCSI_CORE) $(OBJ_PISCSI) $(OBJ_SHARED) $(OBJ_PROTOBUF) $(OBJ_GENERATED) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJ_PISCSI_CORE) $(OBJ_PISCSI) $(OBJ_SHARED) $(OBJ_PROTOBUF) $(OBJ_GENERATED) -lpthread -lpcap -lprotobuf -lstdc++fs

$(BINDIR)/$(SCSICTL): $(SRC_GENERATED) $(OBJ_SCSICTL_CORE) $(OBJ_SCSICTL) $(OBJ_SHARED) $(OBJ_PROTOBUF) $(OBJ_GENERATED) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJ_SCSICTL_CORE) $(OBJ_SCSICTL) $(OBJ_SHARED) $(OBJ_PROTOBUF) $(OBJ_GENERATED) -lpthread -lprotobuf

$(BINDIR)/$(SCSIDUMP): $(OBJ_SCSIDUMP) $(OBJ_SHARED) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJ_SCSIDUMP) $(OBJ_SHARED)

$(BINDIR)/$(SCSIMON): $(OBJ_SCSIMON) $(OBJ_SHARED) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJ_SCSIMON) $(OBJ_SHARED)

$(BINDIR)/$(SCSILOOP): $(OBJ_SHARED) $(OBJ_SCSILOOP) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@  $(OBJ_SHARED) $(OBJ_SCSILOOP)

$(BINDIR)/$(PISCSI_TEST): $(SRC_GENERATED) $(OBJ_PISCSI_CORE) $(OBJ_SCSICTL_CORE) $(OBJ_PISCSI_TEST) $(OBJ_SCSICTL_TEST) $(OBJ_SHARED) $(OBJ_PROTOBUF) $(OBJ_GENERATED) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(TEST_WRAPS) -o $@ $(OBJ_PISCSI_CORE) $(OBJ_SCSICTL_CORE) $(OBJ_PISCSI_TEST) $(OBJ_SHARED) $(OBJ_PROTOBUF) $(OBJ_GENERATED) -lpthread -lpcap -lprotobuf -lstdc++fs -lgmock -lgtest

# Phony rules for building individual utilities
.PHONY: $(PISCSI) $(SCSICTL) $(SCSIDUMP) $(SCSIMON) $(PISCSI_TEST) $(SCSILOOP)
$(PISCSI) : $(BINDIR)/$(PISCSI) 
$(SCSICTL) : $(BINDIR)/$(SCSICTL) 
$(SCSIDUMP) : $(BINDIR)/$(SCSIDUMP) 
$(SCSIMON) : $(BINDIR)/$(SCSIMON)
$(PISCSI_TEST): $(BINDIR)/$(PISCSI_TEST)
$(SCSILOOP) : $(BINDIR)/$(SCSILOOP)

##   clean    : Remove all of the object files, intermediate 
##              compiler files and executable files 
.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(BINDIR) $(GENERATED_DIR) $(COVERAGE_DIR) $(COVERAGE_FILE)

##   install  : Copies all of the man pages to the correct location
##              Copies the binaries to a global install location
##              Configures the Systemd and RSyslog services to auto-run PiSCSI
##              * This target needs to be run with sudo (ex: sudo make install)
##              * Before running this, you need to stop the piscsi service if
##              * it is already running:
##              *   sudo systemctl stop piscsi
##              * After running this, you will need to reboot or run:
##              *   sudo systemctl daemon-reload
##              *   sudo systemctl restart rsyslog
##              *   sudo systemctl enable piscsi
##              *   sudo systemctl start piscsi
.PHONY: install
install: \
	$(MAN_PAGE_DIR)/piscsi.1 \
	$(MAN_PAGE_DIR)/scsictl.1 \
	$(MAN_PAGE_DIR)/scsimon.1 \
	$(MAN_PAGE_DIR)/scsiloop.1 \
	$(MAN_PAGE_DIR)/scsidump.1 \
	$(USR_LOCAL_BIN)/$(SCSICTL) \
	$(USR_LOCAL_BIN)/$(PISCSI) \
	$(USR_LOCAL_BIN)/$(SCSIMON) \
	$(USR_LOCAL_BIN)/$(SCSILOOP) \
	$(USR_LOCAL_BIN)/$(SCSIDUMP) \
	$(SYSTEMD_CONF) \
	$(RSYSLOG_CONF) \
	$(RSYSLOG_LOG)
	@echo "-- Done installing!"

$(USR_LOCAL_BIN)% : $(BINDIR)/%
	@echo "-- Copying $@"
	cp $< $@

$(MAN_PAGE_DIR)/%.1 : $(DOC_DIR)/%.1 | $(MAN_PAGE_DIR)/
	@echo "-- Copying $@"
	cp $< $@

$(DOC_DIR)/%_man_page.txt : $(DOC_DIR)/%.1
	@echo "!!   ------ THIS FILE IS AUTO_GENERATED! DO NOT MANUALLY UPDATE!!!" > $@
	@echo "!!   ------ The native file is $(notdir $<). Re-run 'make docs' after updating\n\n" >> $@
	man -l $< | col -bx >> $@

$(SYSTEMD_CONF) : $(OS_FILES)/$(notdir $(SYSTEMD_CONF))
	@echo "-- Copying $@"
	cp $< $@

$(RSYSLOG_CONF) : $(OS_FILES)/$(notdir $(RSYSLOG_CONF))
	@echo "-- Copying $@"
	cp $< $@

$(RSYSLOG_LOG) :
	@echo "-- Creating $@"
	touch /var/log/piscsi.log
	chown root:adm /var/log/piscsi.log

$(MAN_PAGE_DIR)/:
	echo "-- Creating directory $@"
	mkdir -p $@

##   help     : Lists information about how to use the makefile
# The help rule is based upon the approach from:
#    https://swcarpentry.github.io/make-novice/08-self-doc/index.html 
.PHONY: help
help : Makefile
	@sed -n 's/^##//p' $<

##   Debug    : Same as 'all'. Useful when using a debugger.
.PHONY: Debug
Debug: all

