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

#include "../../include/core/exception.h"
#include "../../include/core/trace.h"

namespace cc65c {

	namespace core {

		#define EXCEPTION_NONE "No exception"

		exception::exception(void) :
			std::runtime_error(EXCEPTION_NONE),
			m_empty(true),
			m_line(0)
		{
			return;
		}

		exception::exception(
			__in const std::string &message,
			__in const std::string &file,
			__in const std::string &function,
			__in size_t line
			) :
				std::runtime_error(message),
				m_empty(false),
				m_file(file),
				m_function(function),
				m_line(line)
		{
			return;
		}

		exception::exception(
			__in const exception &other
			) :
				std::runtime_error(other),
				m_empty(other.m_empty),
				m_file(other.m_file),
				m_function(other.m_function),
				m_line(other.m_line)
		{
			return;
		}

		exception::~exception(void)
		{
			return;
		}

		exception &
		exception::operator=(
			__in const exception &other
			)
		{

			if(this != &other) {
				std::runtime_error::operator=(other);
				m_empty = other.m_empty;
				m_file = other.m_file;
				m_function = other.m_function;
				m_line = other.m_line;
			}

			return *this;
		}

		std::string 
		exception::as_string(
			__in const exception &reference,
			__in_opt bool verbose
			)
		{
			std::stringstream result;

			result << reference.what();

#ifndef NDEBUG
			if(verbose && !reference.m_empty) {

				if(!result.str().empty()) {
					result << " ";
				}

				result << "(";

				if(!reference.m_function.empty()) {
					result << reference.m_function << ":";
				}

				if(!reference.m_file.empty()) {
					result << reference.m_file << "@";
				}

				result << reference.m_line << ")";
			}
#endif // NDEBUG

			return result.str();
		}

		void 
		exception::clear(void)
		{
			std::runtime_error::operator=(std::runtime_error(EXCEPTION_NONE));
			m_empty = true;
			m_file.clear();
			m_function.clear();
			m_line = 0;
		}

		const std::string &
		exception::file(void)
		{
			return m_file;
		}

		const std::string &
		exception::function(void)
		{
			return m_function;
		}

		void 
		exception::generate(
			__in const std::string &message,
			__in const std::string &file,
			__in const std::string &function,
			__in size_t line,
			__in const char *format,
			...
			)
		{
			int length;
			va_list arguments;
			std::string buffer;
			std::stringstream result;

			result << message;

			if(format) {
				va_start(arguments, format);
				length = std::vsnprintf(nullptr, 0, format, arguments);
				va_end(arguments);

				if(length > 0) {
					buffer.resize(++length, 0);
					va_start(arguments, format);
					length = std::vsnprintf(&buffer[0], buffer.size(), format, arguments);
					va_end(arguments);
				}

				if(length < 0) {
					buffer = EXCEPTION_MALFORMED;
					buffer += ": ";
					buffer += format;
				}

				if(!buffer.empty()) {

					if(!result.str().empty()) {
						result << ": ";
					}

					result << buffer;
				}
			}

#ifndef NDEBUG

	#if(TRACE >= 1) // LEVEL_ERROR
			cc65c::core::trace::acquire().send(cc65c::core::LEVEL_ERROR, std::string(), file, function, line, 
				"%s", STRING_CHECK(result.str()));
	#endif // (TRACE >= LEVEL_ERROR)
#endif // NDEBUG
			throw cc65c::core::exception(result.str(), file, function, line);
		}

		bool 
		exception::is_empty(void)
		{
			return m_empty;
		}

		size_t 
		exception::line(void)
		{
			return m_line;
		}

		std::string 
		exception::to_string(
			__in_opt bool verbose
			)
		{
			return cc65c::core::exception::as_string(*this, verbose);
		}
	}
}
