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
CC_FLAGS_EXT=
DIR_BIN=./../bin/
DIR_BUILD=./../build/
DIR_INC=./../include/
DIR_INC_ASM=./../include/assembler/
DIR_INC_CORE=./../include/core/
DIR_INC_TEST=./../include/test/
DIR_SRC=./
DIR_SRC_ASM=./assembler/
DIR_SRC_CORE=./core/
DIR_SRC_TEST=./test/
LIB=libcc65c.a

all: build archive

archive:
	@echo ''
	@echo '--- BUILDING LIBRARY -----------------------'
	ar rcs $(DIR_BIN)$(LIB) $(DIR_BUILD)assembler_lexer.o $(DIR_BUILD)assembler_node.o $(DIR_BUILD)assembler_parser.o \
		$(DIR_BUILD)assembler_stream.o $(DIR_BUILD)assembler_symbol.o $(DIR_BUILD)assembler_token.o $(DIR_BUILD)assembler_tree.o \
		$(DIR_BUILD)base_runtime.o \
		$(DIR_BUILD)core_exception.o $(DIR_BUILD)core_object.o $(DIR_BUILD)core_signal.o $(DIR_BUILD)core_stream.o \
		$(DIR_BUILD)core_thread.o $(DIR_BUILD)core_trace.o $(DIR_BUILD)core_uuid.o \
		$(DIR_BUILD)test_fixture.o $(DIR_BUILD)test_suite.o
	@echo '--- DONE -----------------------------------'
	@echo ''

build: build_assembler build_base build_core build_test

### ASSEMBLER ###

build_assembler: assembler_lexer.o assembler_node.o assembler_parser.o assembler_stream.o assembler_symbol.o assembler_token.o \
	assembler_tree.o

assembler_lexer.o: $(DIR_SRC_ASM)lexer.cpp $(DIR_INC_ASM)lexer.h
	$(CC) $(CC_FLAGS) $(CC_BUILD_FLAGS) $(CC_TRACE_FLAGS) -c $(DIR_SRC_ASM)lexer.cpp -o $(DIR_BUILD)assembler_lexer.o

assembler_node.o: $(DIR_SRC_ASM)node.cpp $(DIR_INC_ASM)node.h
	$(CC) $(CC_FLAGS) $(CC_BUILD_FLAGS) $(CC_TRACE_FLAGS) -c $(DIR_SRC_ASM)node.cpp -o $(DIR_BUILD)assembler_node.o

assembler_parser.o: $(DIR_SRC_ASM)parser.cpp $(DIR_INC_ASM)parser.h
	$(CC) $(CC_FLAGS) $(CC_BUILD_FLAGS) $(CC_TRACE_FLAGS) -c $(DIR_SRC_ASM)parser.cpp -o $(DIR_BUILD)assembler_parser.o

assembler_stream.o: $(DIR_SRC_ASM)stream.cpp $(DIR_INC_ASM)stream.h
	$(CC) $(CC_FLAGS) $(CC_BUILD_FLAGS) $(CC_TRACE_FLAGS) -c $(DIR_SRC_ASM)stream.cpp -o $(DIR_BUILD)assembler_stream.o

assembler_symbol.o: $(DIR_SRC_ASM)symbol.cpp $(DIR_INC_ASM)symbol.h
	$(CC) $(CC_FLAGS) $(CC_BUILD_FLAGS) $(CC_TRACE_FLAGS) -c $(DIR_SRC_ASM)symbol.cpp -o $(DIR_BUILD)assembler_symbol.o

assembler_token.o: $(DIR_SRC_ASM)token.cpp $(DIR_INC_ASM)token.h
	$(CC) $(CC_FLAGS) $(CC_BUILD_FLAGS) $(CC_TRACE_FLAGS) -c $(DIR_SRC_ASM)token.cpp -o $(DIR_BUILD)assembler_token.o

assembler_tree.o: $(DIR_SRC_ASM)tree.cpp $(DIR_INC_ASM)tree.h
	$(CC) $(CC_FLAGS) $(CC_BUILD_FLAGS) $(CC_TRACE_FLAGS) -c $(DIR_SRC_ASM)tree.cpp -o $(DIR_BUILD)assembler_tree.o

### BASE ###

build_base: base_runtime.o

base_runtime.o: $(DIR_SRC)runtime.cpp $(DIR_INC)runtime.h
	$(CC) $(CC_FLAGS) $(CC_BUILD_FLAGS) $(CC_TRACE_FLAGS) -c $(DIR_SRC)runtime.cpp -o $(DIR_BUILD)base_runtime.o

### CORE ###

build_core: core_exception.o core_object.o core_signal.o core_stream.o core_thread.o core_trace.o core_uuid.o

core_exception.o: $(DIR_SRC_CORE)exception.cpp $(DIR_INC_CORE)exception.h
	$(CC) $(CC_FLAGS) $(CC_BUILD_FLAGS) $(CC_TRACE_FLAGS) -c $(DIR_SRC_CORE)exception.cpp -o $(DIR_BUILD)core_exception.o

core_object.o: $(DIR_SRC_CORE)object.cpp $(DIR_INC_CORE)object.h
	$(CC) $(CC_FLAGS) $(CC_BUILD_FLAGS) $(CC_TRACE_FLAGS) -c $(DIR_SRC_CORE)object.cpp -o $(DIR_BUILD)core_object.o

core_signal.o: $(DIR_SRC_CORE)signal.cpp $(DIR_INC_CORE)signal.h
	$(CC) $(CC_FLAGS) $(CC_BUILD_FLAGS) $(CC_TRACE_FLAGS) -c $(DIR_SRC_CORE)signal.cpp -o $(DIR_BUILD)core_signal.o

core_stream.o: $(DIR_SRC_CORE)stream.cpp $(DIR_INC_CORE)stream.h
	$(CC) $(CC_FLAGS) $(CC_BUILD_FLAGS) $(CC_TRACE_FLAGS) -c $(DIR_SRC_CORE)stream.cpp -o $(DIR_BUILD)core_stream.o

core_thread.o: $(DIR_SRC_CORE)thread.cpp $(DIR_INC_CORE)thread.h
	$(CC) $(CC_FLAGS) $(CC_BUILD_FLAGS) $(CC_TRACE_FLAGS) -c $(DIR_SRC_CORE)thread.cpp -o $(DIR_BUILD)core_thread.o

core_trace.o: $(DIR_SRC_CORE)trace.cpp $(DIR_INC_CORE)trace.h
	$(CC) $(CC_FLAGS) $(CC_BUILD_FLAGS) $(CC_TRACE_FLAGS) -c $(DIR_SRC_CORE)trace.cpp -o $(DIR_BUILD)core_trace.o

core_uuid.o: $(DIR_SRC_CORE)uuid.cpp $(DIR_INC_CORE)uuid.h
	$(CC) $(CC_FLAGS) $(CC_BUILD_FLAGS) $(CC_TRACE_FLAGS) -c $(DIR_SRC_CORE)uuid.cpp -o $(DIR_BUILD)core_uuid.o

### TEST ###

build_test: test_fixture.o test_suite.o

test_fixture.o: $(DIR_SRC_TEST)fixture.cpp $(DIR_INC_TEST)fixture.h
	$(CC) $(CC_FLAGS) $(CC_BUILD_FLAGS) $(CC_TRACE_FLAGS) -c $(DIR_SRC_TEST)fixture.cpp -o $(DIR_BUILD)test_fixture.o

test_suite.o: $(DIR_SRC_TEST)suite.cpp $(DIR_INC_TEST)suite.h
	$(CC) $(CC_FLAGS) $(CC_BUILD_FLAGS) $(CC_TRACE_FLAGS) -c $(DIR_SRC_TEST)suite.cpp -o $(DIR_BUILD)test_suite.o
