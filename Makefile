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

## Build type - default is Release
BUILD_TYPE := Release
ifeq ($(MAKECMDGOALS),debug)
	BUILD_TYPE := Debug
endif
ifeq ($(MAKECMDGOALS),test)
	BUILD_TYPE := Debug
endif
ifeq ($(MAKECMDGOALS),memcheck)
	BUILD_TYPE := Debug
endif

ifeq ($(MAKECMDGOALS),coverage)
	BUILD_TYPE := Coverage
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

# Option specific for Travis CI - use the environment var to determine the compiler
ifneq (,$(COMPILER))
	CMAKE_OPTS := $(CMAKE_OPTS) -DCMAKE_CXX_COMPILER=$(COMPILER)
endif

# CMAKE build type
CMAKE_OPTS := $(CMAKE_OPTS) -DBUILD_TYPE=$(BUILD_TYPE)
ifeq (Release,$(BUILD_TYPE))
	CMAKE_OPTS := $(CMAKE_OPTS) -DCMAKE_BUILD_TYPE=Release
else
	CMAKE_OPTS := $(CMAKE_OPTS) -DCMAKE_BUILD_TYPE=Debug
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
${1}-lib: ${1}/build/$(BUILD_TYPE)/Makefile
	$$(Q)cd ${1}/build/$(BUILD_TYPE) && $$(MAKE) lib --no-print-directory $$(MAKE_DEBUG)
.PHONY: ${1}-lib

${1}-programs: ${1}-lib
	$$(Q)cd ${1}/build/$(BUILD_TYPE) && $$(MAKE) all --no-print-directory $$(MAKE_DEBUG)
.PHONY: ${1}-programs

${1}-test: ${1}-programs
	$$(Q)cd ${1}/build/$(BUILD_TYPE) && $$(MAKE) CTEST_OUTPUT_ON_FAILURE=1 test --no-print-directory $$(MAKE_DEBUG)
.PHONY: ${1}-test

${1}-cppcheck: ${1}-lib
	$$(Q)cd ${1}/build/$(BUILD_TYPE) && $$(MAKE) cppcheck --no-print-directory $$(MAKE_DEBUG)
.PHONY: ${1}-cppcheck

${1}-memcheck: ${1}-programs
	$$(Q)cd ${1}/build/$(BUILD_TYPE) && $$(MAKE) memcheck --no-print-directory $$(MAKE_DEBUG)
.PHONY: ${1}-memcheck

${1}-coverage: ${1}-programs
	$$(Q)cd ${1}/build/$(BUILD_TYPE) && $$(MAKE) coverage --no-print-directory $$(MAKE_DEBUG)
.PHONY: ${1}-coverage

${1}-doc: 
	$$(Q)if [ -f ${1}/doc/Doxyfile ]; then \
		cd ${1}/doc && doxygen; \
	fi
.PHONY: ${1}-doc

${1}/build/$(BUILD_TYPE)/Makefile : | ${1}/build/$(BUILD_TYPE)
	$$(Q)cd $$(dir $$(@)) && rm -rf *
	$$(Q)cd $$(dir $$(@)) && cmake $$(CMAKE_OPTS) -DTOPDIR=$$(topdir) ../..

${1}/build/$(BUILD_TYPE):
	$$(Q)mkdir -p $$(@)

clean-${1}: 
	$$(Q)rm -rf ${1}/build
	$$(Q)echo cleaned ${1}
.PHONY: clean-${1}
endef


##############################################################################################
# TARGETS
##############################################################################################

all debug release: libs programs
.PHONY: all debug release

lib libs: Hypno 
.PHONY: lib libs

test tests: Hypno-test 
.PHONY: test tests

cppcheck: Hypno-cppcheck 
.PHONY: cppcheck

memcheck: libs Hypno-memcheck
.PHONY: memcheck

coverage: libs tests Hypno-coverage
.PHONY: coverage

doc: Hypno-doc Hypno-doc
.PHONY: doc

topdir := $(shell pwd)
$(info topdir=$(topdir))

clean: clean-programs clean-Hypno
	$(Q)echo All cleaned
.PHONY: clean

##-----------------------------------------------------------------------------	
programs: build/$(BUILD_TYPE)/Makefile libs
	$(Q)cd $(dir $<) && $(MAKE) --no-print-directory $(MAKE_DEBUG)
.PHONY: programs

build/$(BUILD_TYPE)/Makefile : | build/$(BUILD_TYPE)
	$(Q)cd $(dir $@) && rm -rf *
	$(Q)cd $(dir $@) && cmake $(CMAKE_OPTS) -DTOPDIR=$(topdir) ../..

build/$(BUILD_TYPE):
	$(Q)mkdir -p $@

clean-programs:
	$(Q)rm -rf build
	$(Q)echo cleaned programs
.PHONY: clean-programs

prog-cppcheck: build/$(BUILD_TYPE)/Makefile
	$(Q)cd $(dir $<) && $(MAKE) cppcheck --no-print-directory $(MAKE_DEBUG)
.PHONY: prog-cppcheck

##-----------------------------------------------------------------------------	
Hypno: Hypno-lib Hypno-programs
.PHONY: Hypno

$(eval $(call ADD_LIB_TARGETS,Hypno))

