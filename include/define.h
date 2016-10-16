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

#ifndef CC65C_DEFINE_H_
#define CC65C_DEFINE_H_

#include <iomanip>
#include <iostream>
#include <sstream>

namespace cc65c {

	#define __in
	#define __in_opt
	#define __inout
	#define __inout_opt

	#define CC65C "cc65c"

	#define REFERENCE_INIT 1

	#define SCALAR_AS_HEX(_TYPE_, _VAL_) \
		std::setw(sizeof(_TYPE_) * 2) << std::setfill('0') << std::hex \
			<< (uintmax_t) ((_TYPE_) (_VAL_)) << std::dec

	#define SCALAR_INVALID(_TYPE_) ((_TYPE_) -1)

	#define STRING_EMPTY "EMPTY"
	#define STRING_INVALID "INVALID"
	#define STRING_UNKNOWN "UNKNOWN"

	#define STRING_CHECK(_STR_) (_STR_.empty() ? STRING_EMPTY : _STR_.c_str())

	#define _STRING_CONCAT(_STR_) # _STR_
	#define STRING_CONCAT(_STR_) _STRING_CONCAT(_STR_)

	#define VERSION_MAJOR 0
	#define VERSION_MINOR 1
	#define VERSION_RELEASE "alpha"
	#define VERSION_REVISION 3
	#define VERSION_WEEK 1642
}

#endif // CC65C_DEFINE_H_
