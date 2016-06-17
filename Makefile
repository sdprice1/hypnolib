## Copyright (c) 2016 Steve Price
##
## This file is part of Hypnocube
##
## Hypnocube is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## Hypnocube is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with Hypnocube.  If not, see <http://www.gnu.org/licenses/>.

##############################################################################################
# SETTINGS - Used by makefile targets
##############################################################################################

## Executables
ECHO=ECHO
MD5SUM=md5sum

BUILD_VERBOSITY ?= 0
ifneq ($(V),)
BUILD_VERBOSITY := $(V)
endif

# Levels of verbosity
# V=0 is quiet
# V=1 show commands prefixed by $(Q), keep $(QQ) commands silent
# V=2 show all commands
CMAKE_DEBUG :=
MAKE_DEBUG :=
ifeq (0,$(BUILD_VERBOSITY))
    Q := @
    QQ := @
else
	MAKE_DEBUG := VERBOSE=$(BUILD_VERBOSITY)
	ifeq (1,$(BUILD_VERBOSITY))
	    Q := 
	    QQ := @
	else
		CMAKE_DEBUG := -DDEBUG_CMAKELIST=1
	    Q :=
	    QQ :=
	endif
endif


##############################################################################################
# TARGETS
##############################################################################################

all: LibHypno LibHypnoQuartz programs
.PHONY: all

test tests: LibHypno-test LibHypnoQuartz-test
.PHONY: test tests

cppcheck: LibHypno-cppcheck LibHypnoQuartz-cppcheck
.PHONY: cppcheck

topdir := $(shell pwd)
$(info topdir=$(topdir))

clean: clean-programs clean-LibHypno clean-LibHypnoQuartz
	$(Q)echo All cleaned
.PHONY: clean

##-----------------------------------------------------------------------------	
## COVERITY
coverity: clean cov-build
.PHONY: coverity

cov-build: hypnolib.lzma
.PHONY: cov-build

hypnolib.lzma:
	$(Q)-rm -rf cov-int 2>/dev/null
	$(Q)cov-build --dir cov-int make
	$(Q)tar caf hypnolib.lzma cov-int
	
coverity-submit: cov-build
	$(Q)curl --form token=B0QLd2JFgq8-6jqLUOUqYA \
		  --form email=linux@quartz-net.co.uk \
		  --form file=@hypnolib.lzma \
		  --form version="Version" \
		  --form description="Description" \
		  https://scan.coverity.com/builds?project=sdprice1%2Fhypnolib

##-----------------------------------------------------------------------------	
programs: build/Makefile
	$(Q)cd $(dir $<) && $(MAKE) --no-print-directory $(MAKE_DEBUG)
.PHONY: programs

build/Makefile : | build
	$(Q)cd $(dir $@) && rm -rf *
	$(Q)cd $(dir $@) && cmake $(CMAKE_DEBUG) -DTOPDIR=$(topdir) ..

build:
	$(Q)mkdir -p $@

clean-programs:
	$(Q)rm -rf build
	$(Q)echo cleaned programs
.PHONY: clean-programs

prog-cppcheck: build/Makefile
	$(Q)cd $(dir $<) && $(MAKE) cppcheck --no-print-directory $(MAKE_DEBUG)
.PHONY: prog-cppcheck

##-----------------------------------------------------------------------------	
LibHypno: LibHypno-lib LibHypno-programs
.PHONY: LibHypno

LibHypno_BUILD_DIR := LibHypno/build
LibHypno-lib: $(LibHypno_BUILD_DIR)/Makefile
	$(Q)cd $(LibHypno_BUILD_DIR) && $(MAKE) lib --no-print-directory $(MAKE_DEBUG)
.PHONY: LibHypno-lib

LibHypno-programs: LibHypno-lib
	$(Q)cd $(LibHypno_BUILD_DIR) && $(MAKE) all --no-print-directory $(MAKE_DEBUG)
.PHONY: LibHypno-programs

LibHypno-test: LibHypno-programs
	$(Q)cd $(LibHypno_BUILD_DIR) && $(MAKE) test --no-print-directory $(MAKE_DEBUG)
.PHONY: LibHypno-test

LibHypno-cppcheck: LibHypno-lib
	$(Q)cd $(LibHypno_BUILD_DIR) && $(MAKE) cppcheck --no-print-directory $(MAKE_DEBUG)
.PHONY: LibHypno-cppcheck

$(LibHypno_BUILD_DIR)/Makefile : | $(LibHypno_BUILD_DIR)
	$(Q)cd $(dir $@) && rm -rf *
	$(Q)cd $(dir $@) && cmake $(CMAKE_DEBUG) -DTOPDIR=$(topdir) ..

$(LibHypno_BUILD_DIR):
	$(Q)mkdir -p $@

clean-LibHypno: 
	$(Q)rm -rf $(LibHypno_BUILD_DIR)
	$(Q)echo cleaned LibHypno
.PHONY: clean-LibHypno

##-----------------------------------------------------------------------------	
LibHypnoQuartz: LibHypno LibHypnoQuartz-lib LibHypnoQuartz-programs
.PHONY: LibHypnoQuartz

LibHypnoQuartz_BUILD_DIR := LibHypnoQuartz/build
LibHypnoQuartz-lib: $(LibHypnoQuartz_BUILD_DIR)/Makefile
	$(Q)cd $(LibHypnoQuartz_BUILD_DIR) && $(MAKE) lib --no-print-directory $(MAKE_DEBUG)
.PHONY: LibHypnoQuartz-lib

LibHypnoQuartz-programs: LibHypnoQuartz-lib
	$(Q)cd $(LibHypnoQuartz_BUILD_DIR) && $(MAKE) all --no-print-directory $(MAKE_DEBUG)
.PHONY: LibHypnoQuartz-programs

LibHypnoQuartz-test: LibHypnoQuartz-programs
	$(Q)cd $(LibHypnoQuartz_BUILD_DIR) && $(MAKE) test --no-print-directory $(MAKE_DEBUG)
.PHONY: LibHypnoQuartz-test

LibHypnoQuartz-cppcheck: LibHypnoQuartz-programs
	$(Q)cd $(LibHypnoQuartz_BUILD_DIR) && $(MAKE) cppcheck --no-print-directory $(MAKE_DEBUG)
.PHONY: LibHypnoQuartz-cppcheck

$(LibHypnoQuartz_BUILD_DIR)/Makefile : | $(LibHypnoQuartz_BUILD_DIR)
	$(Q)cd $(dir $@) && rm -rf *
	$(Q)cd $(dir $@) && cmake $(CMAKE_DEBUG) -DTOPDIR=$(topdir) ..

$(LibHypnoQuartz_BUILD_DIR):
	$(Q)mkdir -p $@

clean-LibHypnoQuartz: 
	$(Q)rm -rf $(LibHypnoQuartz_BUILD_DIR)
	$(Q)echo cleaned LibHypnoQuartz
.PHONY: clean-LibHypnoQuartz

