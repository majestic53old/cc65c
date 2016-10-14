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

#ifndef CC65C_CORE_EXCEPTION_H_
#define CC65C_CORE_EXCEPTION_H_

#include "../define.h"

namespace cc65c {

	namespace core {

		#define EXCEPTION_MALFORMED "Malformed exception"
		#define EXCEPTION_UNKNOWN "Unknown exception"

		#define THROW_EXCEPTION(_EXCEPT_) THROW_EXCEPTION_FORMAT(_EXCEPT_, "", "")

		#define THROW_EXCEPTION_FORMAT(_EXCEPT_, _FORMAT_, ...) \
			cc65c::core::exception::generate(_EXCEPT_, __FILE__, __FUNCTION__, __LINE__, \
				_FORMAT_, __VA_ARGS__)

		class exception :
				public std::runtime_error {

			public:

				exception(void);

				exception(
					__in const std::string &message,
					__in const std::string &file,
					__in const std::string &function,
					__in size_t line
					);

				exception(
					__in const exception &other
					);

				virtual ~exception(void);

				exception &operator=(
					__in const exception &other
					);

				static std::string as_string(
					__in const exception &reference,
					__in_opt bool verbose = false
					);

				void clear(void);

				const std::string &file(void);

				const std::string &function(void);

				static void generate(
					__in const std::string &message,
					__in const std::string &file,
					__in const std::string &function,
					__in size_t line,
					__in const char *format,
					...
					);

				bool is_empty(void);

				size_t line(void);

				virtual std::string to_string(
					__in_opt bool verbose = false
					);

			protected:

				bool m_empty;

				std::string m_file;

				std::string m_function;

				size_t m_line;
		};
	}
}

#endif // CC65C_CORE_EXCEPTION_H_
