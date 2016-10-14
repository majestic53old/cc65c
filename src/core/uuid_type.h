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

#ifndef CC65C_CORE_UUID_TYPE_H_
#define CC65C_CORE_UUID_TYPE_H_

#include "../../include/core/exception.h"

namespace cc65c {

	namespace core {

		#define CC65C_CORE_UUID_HEADER "[CC65C::CORE::UUID]"

#ifndef NDEBUG
		#define CC65C_CORE_UUID_EXCEPTION_HEADER CC65C_CORE_UUID_HEADER " "
#else
		#define CC65C_CORE_UUID_EXCEPTION_HEADER
#endif // NDEBUG

		enum {
			CC65C_CORE_UUID_EXCEPTION_DUPLICATE = 0,
			CC65C_CORE_UUID_EXCEPTION_FULL,
			CC65C_CORE_UUID_EXCEPTION_INITIALIZED,
			CC65C_CORE_UUID_EXCEPTION_NOT_FOUND,
			CC65C_CORE_UUID_EXCEPTION_UNINITIALIZED,
		};

		#define CC65C_CORE_UUID_EXCEPTION_MAX CC65C_CORE_UUID_EXCEPTION_UNINITIALIZED

		static const std::string CC65C_CORE_UUID_EXCEPTION_STR[] = {
			CC65C_CORE_UUID_EXCEPTION_HEADER "Duplicate uuid",
			CC65C_CORE_UUID_EXCEPTION_HEADER "Uuid manager is full",
			CC65C_CORE_UUID_EXCEPTION_HEADER "Uuid manager is initialized",
			CC65C_CORE_UUID_EXCEPTION_HEADER "Uuid does not exist",
			CC65C_CORE_UUID_EXCEPTION_HEADER "Uuid manager is uninitialized",
			};

		#define CC65C_CORE_UUID_EXCEPTION_STRING(_TYPE_) \
			((_TYPE_) > CC65C_CORE_UUID_EXCEPTION_MAX ? CC65C_CORE_UUID_EXCEPTION_HEADER EXCEPTION_UNKNOWN : \
			STRING_CHECK(CC65C_CORE_UUID_EXCEPTION_STR[_TYPE_]))

		#define THROW_CC65C_CORE_UUID_EXCEPTION(_EXCEPT_) \
			THROW_EXCEPTION(CC65C_CORE_UUID_EXCEPTION_STRING(_EXCEPT_))
		#define THROW_CC65C_CORE_UUID_EXCEPTION_FORMAT(_EXCEPT_, _FORMAT_, ...) \
			THROW_EXCEPTION_FORMAT(CC65C_CORE_UUID_EXCEPTION_STRING(_EXCEPT_), \
				_FORMAT_, __VA_ARGS__)
	}
}

#endif // CC65C_CORE_UUID_TYPE_H_
