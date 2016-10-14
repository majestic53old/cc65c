# cc65c
# Copyright (C) 2016 David Jolly
#
# cc65c is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# cc65c is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

BUILD_FLAGS_DBG=CC_BUILD_FLAGS=-g
BUILD_FLAGS_REL=CC_BUILD_FLAGS=-O3\ -DNDEBUG
DIR_BIN=./bin/
DIR_BUILD=./build/
DIR_EXE=./tool/
DIR_INC=./include/
DIR_LOG=./log/
DIR_ROOT=./
DIR_SRC=./src/
EXE=cc65c
JOB_SLOTS=4
LOG_MEM=val_err.log
LOG_STAT=stat_err.log
LOG_CLOC=cloc_stat.log
TRACE?=0
TRACE_FLAGS_DBG=CC_TRACE_FLAGS=-DTRACE_COLOR\ -DTRACE=
TRACE_FLAGS_REL=CC_TRACE_FLAGS=-DTRACE_COLOR\ -DTRACE=0

all: debug

debug: clean init lib_debug exe_debug

release: clean init lib_release exe_release

testing: debug test

statistics: clean init stat

clean:
	rm -rf $(DIR_BIN)
	rm -rf $(DIR_BUILD)
	rm -rf $(DIR_LOG)

exe_debug:
	@echo ''
	@echo '============================================'
	@echo 'BUILDING EXECUTABLES (DEBUG)'
	@echo '============================================'
	cd $(DIR_EXE) && make $(BUILD_FLAGS_DBG) $(TRACE_FLAGS_DBG)$(TRACE)

exe_release:
	@echo ''
	@echo '============================================'
	@echo 'BUILDING EXECUTABLES (RELEASE)'
	@echo '============================================'
	cd $(DIR_EXE) && make $(BUILD_FLAGS_REL) $(TRACE_FLAGS_REL)

init:
	mkdir $(DIR_BIN)
	mkdir $(DIR_BUILD)
	mkdir $(DIR_LOG)

lib_debug: 
	@echo ''
	@echo '============================================'
	@echo 'BUILDING LIBRARIES (DEBUG)'
	@echo '============================================'
	cd $(DIR_SRC) && make $(BUILD_FLAGS_DBG) $(TRACE_FLAGS_DBG)$(TRACE) build -j $(JOB_SLOTS)
	cd $(DIR_SRC) && make archive

lib_release: 
	@echo ''
	@echo '============================================'
	@echo 'BUILDING LIBRARIES (RELEASE)'
	@echo '============================================'
	cd $(DIR_SRC) && make $(BUILD_FLAGS_REL) $(TRACE_FLAGS_REL) build -j $(JOB_SLOTS)
	cd $(DIR_SRC) && make archive

### TESTING ###

test: static mem

mem:
	@echo ''
	@echo '============================================'
	@echo 'RUNNING MEMORY TEST'
	@echo '============================================'
	valgrind --leak-check=full $(DIR_BIN)$(EXE) 2> $(DIR_LOG)$(LOG_MEM)

static:
	@echo ''
	@echo '============================================'
	@echo 'RUNNING STATIC ANALYSIS'
	@echo '============================================'
	cppcheck --enable=all --std=c++11 $(DIR_SRC) 2> $(DIR_LOG)$(LOG_STAT)

### STATISTICS ###

stat: lines

lines:
	@echo ''
	@echo '============================================'
	@echo 'CALCULATING LINE COUNT'
	@echo '============================================'
	cloc $(DIR_ROOT) > $(DIR_LOG)$(LOG_CLOC)
