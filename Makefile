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
CMAKE_OPTS :=
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
		CMAKE_OPTS := -DDEBUG_CMAKELIST=1
	    Q :=
	    QQ :=
	endif
endif

TEST_TTY ?= 0
ifeq (1,$(TEST_TTY))
	CMAKE_OPTS := $(CMAKE_OPTS) -DTEST_TTY=1
endif

ifneq (,$(COMPILER))
	CMAKE_OPTS := $(CMAKE_OPTS) -DCMAKE_CXX_COMPILER=$(COMPILER)
endif


##############################################################################################
# FUNCTIONS
##############################################################################################

# ADD_LIB_TARGETS - Parameterized "function" that adds the targets for creating the library
#
#	${1} = Library name
#
# Creates targets:
#   ${1}-lib: Make lib
#   ${1}-programs: Make library test programs
#   ${1}-test: Run library tests
#   ${1}-cppcheck: Run cppcheck on library source
#   ${1}-memcheck: Run valgrind on library tests
#
#   USE WITH EVAL
#
define ADD_LIB_TARGETS
${1}-lib: ${1}/build/Makefile
	$$(Q)cd ${1}/build && $$(MAKE) lib --no-print-directory $$(MAKE_DEBUG)
.PHONY: ${1}-lib

${1}-programs: ${1}-lib
	$$(Q)cd ${1}/build && $$(MAKE) all --no-print-directory $$(MAKE_DEBUG)
.PHONY: ${1}-programs

${1}-test: ${1}-programs
	$$(Q)cd ${1}/build && $$(MAKE) CTEST_OUTPUT_ON_FAILURE=1 test --no-print-directory $$(MAKE_DEBUG)
.PHONY: ${1}-test

${1}-cppcheck: ${1}-lib
	$$(Q)cd ${1}/build && $$(MAKE) cppcheck --no-print-directory $$(MAKE_DEBUG)
.PHONY: ${1}-cppcheck

${1}-memcheck: ${1}-programs
	$$(Q)cd ${1}/build && $$(MAKE) memcheck --no-print-directory $$(MAKE_DEBUG)
.PHONY: ${1}-memcheck

${1}/build/Makefile : | ${1}/build
	$$(Q)cd $$(dir $$(@)) && rm -rf *
	$$(Q)cd $$(dir $$(@)) && cmake $$(CMAKE_OPTS) -DTOPDIR=$$(topdir) ..

${1}/build:
	$$(Q)mkdir -p $$(@)

clean-${1}: 
	$$(Q)rm -rf ${1}/build
	$$(Q)echo cleaned ${1}
.PHONY: clean-${1}
endef


##############################################################################################
# TARGETS
##############################################################################################

all: LibHypno LibHypnoQuartz programs
.PHONY: all

test tests: LibHypno-test LibHypnoQuartz-test
.PHONY: test tests

cppcheck: LibHypno-cppcheck LibHypnoQuartz-cppcheck
.PHONY: cppcheck

#memcheck: LibHypno-memcheck LibHypnoQuartz-memcheck
memcheck: LibHypnoQuartz-memcheck
.PHONY: memcheck

topdir := $(shell pwd)
$(info topdir=$(topdir))

clean: clean-programs clean-LibHypno clean-LibHypnoQuartz
	$(Q)echo All cleaned
.PHONY: clean

##-----------------------------------------------------------------------------	
programs: build/Makefile
	$(Q)cd $(dir $<) && $(MAKE) --no-print-directory $(MAKE_DEBUG)
.PHONY: programs

build/Makefile : | build
	$(Q)cd $(dir $@) && rm -rf *
	$(Q)cd $(dir $@) && cmake $(CMAKE_OPTS) -DTOPDIR=$(topdir) ..

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

$(eval $(call ADD_LIB_TARGETS,LibHypno))

##-----------------------------------------------------------------------------	
LibHypnoQuartz: LibHypno LibHypnoQuartz-lib LibHypnoQuartz-programs
.PHONY: LibHypnoQuartz

$(eval $(call ADD_LIB_TARGETS,LibHypnoQuartz))

