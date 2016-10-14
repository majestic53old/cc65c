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

#include "../../include/core/trace.h"
#include "trace_type.h"

namespace cc65c {

	namespace core {

		enum {
			TRACE_DATA_FILE,
			TRACE_DATA_FUNCTION,
			TRACE_DATA_LEVEL,
			TRACE_DATA_LINE,
			TRACE_DATA_MESSAGE,
			TRACE_DATA_PREFIX,
			TRACE_DATA_TIMESTAMP,
		};

		static const std::string LEVEL_STR[] = {
			"", "ERROR", "WARNING", "INFORMATION", "VERBOSE",
			};

		#define LEVEL_STRING(_TYPE_) \
			((_TYPE_) > LEVEL_MAX ? STRING_UNKNOWN : STRING_CHECK(LEVEL_STR[_TYPE_]))

		#define TRACE_COLOR_RESET "\033[0m"
		#define TRACE_COLOR_ERROR "\033[31m"
		#define TRACE_COLOR_INFORMATION "\033[32m"
		#define TRACE_COLOR_VERBOSE "\033[39m"
		#define TRACE_COLOR_WARNING "\033[33m"

		#define TRACE_MALFORMED "Malformed trace"
		#define TRACE_TIMESTAMP_FORMAT "%Y-%m-%d %H:%M:%S"
		#define TRACE_TIMESTAMP_LENGTH 32

		trace::trace(void)
		{
			return;
		}

		trace::~trace(void)
		{
			uninitialize();
		}

		std::string 
		trace::as_string(
			__in const trace &reference,
			__in_opt bool verbose
			)
		{
			std::stringstream result;

			result << CC65C_CORE_TRACE_HEADER
				<< " State={" << cc65c::core::singleton<cc65c::core::trace>::as_string(reference, verbose)
				<< ", " << cc65c::core::thread::as_string(reference, verbose) << "}";

			if(verbose) {
				result << ", Instance=" << SCALAR_AS_HEX(uintptr_t, &reference);

				if(reference.m_initialized && reference.m_active) {
					result << ", Pending=" << reference.m_queue.size();
				}
			}

			return result.str();
		}

		void 
		trace::flush(void)
		{
			cc65c::core::trace_data data;

			while(pop(data)) {
				process(data);
			}
		}

		void 
		trace::initialize(void)
		{

			if(is_initialized()) {
				THROW_CC65C_CORE_TRACE_EXCEPTION(CC65C_CORE_TRACE_EXCEPTION_INITIALIZED);
			}

			cc65c::core::thread::activate(false);
			set_initialized(true);
		}

		void 
		trace::on_activate(void)
		{
			return;
		}

		void 
		trace::on_deactivate(void)
		{
			flush();
		}

		void 
		trace::on_notify(void)
		{
			flush();
		}

		bool 
		trace::pop(
			__inout cc65c::core::trace_data &data
			)
		{
			bool result;

			std::lock_guard<std::mutex> lock(m_queue_mutex);

			result = !m_queue.empty();
			if(result) {
				data = m_queue.front();
				m_queue.pop();
			}

			return result;
		}

		void 
		trace::process(
			__in const cc65c::core::trace_data &data
			)
		{
			std::string buffer;
			std::stringstream result;
			cc65c::core::level_t level;

			level = std::get<TRACE_DATA_LEVEL>(data);
			buffer.resize(TRACE_TIMESTAMP_LENGTH, 0);
			std::strftime(&buffer[0], buffer.size(), TRACE_TIMESTAMP_FORMAT, 
				std::localtime(&std::get<TRACE_DATA_TIMESTAMP>(data)));
			result << "[" << buffer << "] " << "[" << LEVEL_STRING(level) << "] ";

			buffer = std::get<TRACE_DATA_PREFIX>(data);
			if(!buffer.empty()) {
				result << buffer;
			}

			buffer = std::get<TRACE_DATA_FUNCTION>(data);
			if(!buffer.empty()) {
				result << buffer;
			}

			buffer = std::get<TRACE_DATA_MESSAGE>(data);
			if(!buffer.empty()) {

				if(!result.str().empty()) {
					result << ": ";
				}

#ifdef TRACE_COLOR
				switch(level) {
					case LEVEL_ERROR:
						result << TRACE_COLOR_ERROR;
						break;
					case LEVEL_INFORMATION:
						result << TRACE_COLOR_INFORMATION;
						break;
					case LEVEL_VERBOSE:
						result << TRACE_COLOR_VERBOSE;
						break;
					case LEVEL_WARNING:
						result << TRACE_COLOR_WARNING;
						break;
					default:
						break;
				}
#endif // TRACE_COLOR
				result << buffer;
#ifdef TRACE_COLOR
				result << TRACE_COLOR_RESET;
#endif // TRACE_COLOR
			}

#ifndef NDEBUG
			result << " (";

			buffer = std::get<TRACE_DATA_FILE>(data);
			if(!buffer.empty()) {
				result << buffer << "@";
			}

			result << std::get<TRACE_DATA_LINE>(data) << ")";
#endif // NDEBUG

			switch(level) {
				case LEVEL_ERROR:
					std::cerr << result.str() << std::endl;
					break;
				case LEVEL_INFORMATION:
				case LEVEL_VERBOSE:
				case LEVEL_WARNING:
					std::cout << result.str() << std::endl;
					break;
				default:
					break;
			}
		}

		void 
		trace::push(
			__in const cc65c::core::trace_data &data
			)
		{
			std::lock_guard<std::mutex> lock(m_queue_mutex);
			m_queue.push(data);
		}

		void 
		trace::send(
			__in cc65c::core::level_t level,
			__in const std::string &prefix,
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

			if(is_initialized()) {

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
						buffer = TRACE_MALFORMED;
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

				push(std::make_tuple(file, function, level, line, result.str(), prefix, std::time(nullptr)));
				cc65c::core::thread::notify();
			}
		}

		std::string 
		trace::to_string(
			__in_opt bool verbose
			)
		{
			return cc65c::core::trace::as_string(*this, verbose);
		}

		void 
		trace::uninitialize(void)
		{

			if(is_initialized()) {
				set_initialized(false);
				cc65c::core::thread::deactivate();
			}
		}
	}
}
