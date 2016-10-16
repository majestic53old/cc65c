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

CC=clang++
CC_FLAGS=-march=native -std=gnu++11 -Wall -Werror
CC_FLAGS_EXT=-pthread
DIR_BIN=./../../bin/
DIR_BUILD=./../../build/
DIR_INC=./
DIR_SRC=./src/
EXE=cc65c_parser_test
LIB=libcc65c.a

all: exe

### EXECUTABLES ###

exe:
	@echo ''
	@echo '--- BUILDING TOOL --------------------------' 
	$(CC) $(CC_FLAGS) $(CC_BUILD_FLAGS) $(CC_TRACE_FLAGS) $(CC_FLAGS_EXT) main.cpp $(DIR_BIN)$(LIB) -o $(DIR_BIN)$(EXE)
	@echo '--- DONE -----------------------------------'
	@echo ''
