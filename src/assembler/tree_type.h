/**
 * cc65c
 * Copyright (C) 2016 David Jolly
 *
 * cc65c is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cc65c is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CC65C_ASSEMBLER_TREE_TYPE_H_
#define CC65C_ASSEMBLER_TREE_TYPE_H_

#include "../../include/core/exception.h"

namespace cc65c {

	namespace assembler {

		#define CC65C_ASSEMBLER_TREE_HEADER "[CC65C::ASSEMBLER::TREE]"

#ifndef NDEBUG
		#define CC65C_ASSEMBLER_TREE_EXCEPTION_HEADER CC65C_ASSEMBLER_TREE_HEADER " "
#else
		#define CC65C_ASSEMBLER_TREE_EXCEPTION_HEADER
#endif // NDEBUG

		enum {
			CC65C_ASSEMBLER_TREE_EXCEPTION_INVALID_INVALID_TYPE = 0,
			CC65C_ASSEMBLER_TREE_EXCEPTION_NOT_FOUND,
		};

		#define CC65C_ASSEMBLER_TREE_EXCEPTION_MAX CC65C_ASSEMBLER_TREE_EXCEPTION_NOT_FOUND

		static const std::string CC65C_ASSEMBLER_TREE_EXCEPTION_STR[] = {
			CC65C_ASSEMBLER_TREE_EXCEPTION_HEADER "Invalid tree type",
			CC65C_ASSEMBLER_TREE_EXCEPTION_HEADER "Node does not exist",
			};

		#define CC65C_ASSEMBLER_TREE_EXCEPTION_STRING(_TYPE_) \
			((_TYPE_) > CC65C_ASSEMBLER_TREE_EXCEPTION_MAX ? CC65C_ASSEMBLER_TREE_EXCEPTION_HEADER EXCEPTION_UNKNOWN : \
			STRING_CHECK(CC65C_ASSEMBLER_TREE_EXCEPTION_STR[_TYPE_]))

		#define THROW_CC65C_ASSEMBLER_TREE_EXCEPTION(_EXCEPT_) \
			THROW_EXCEPTION(CC65C_ASSEMBLER_TREE_EXCEPTION_STRING(_EXCEPT_))
		#define THROW_CC65C_ASSEMBLER_TREE_EXCEPTION_FORMAT(_EXCEPT_, _FORMAT_, ...) \
			THROW_EXCEPTION_FORMAT(CC65C_ASSEMBLER_TREE_EXCEPTION_STRING(_EXCEPT_), \
				_FORMAT_, __VA_ARGS__)
	}
}

#endif // CC65C_ASSEMBLER_TREE_TYPE_H_
