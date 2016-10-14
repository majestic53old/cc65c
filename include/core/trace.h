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

#ifndef CC65C_CORE_TRACE_H_
#define CC65C_CORE_TRACE_H_

#include <queue>
#include "singleton.h"
#include "thread.h"

namespace cc65c {

	namespace core {

#ifndef NDEBUG
	#define TRACE_PREFIX_ENTRY "+"
	#define TRACE_PREFIX_EXIT "-"

	#define TRACE_MESSAGE(_LEVEL_, _PREFIX_, _FORMAT_, ...) \
		cc65c::core::trace::acquire().send(_LEVEL_, _PREFIX_, __FILE__, __FUNCTION__, __LINE__, \
			_FORMAT_, __VA_ARGS__)

	#if(TRACE == 1) // LEVEL_ERROR
		#define TRACE_ENTRY()
		#define TRACE_ENTRY_FORMAT(_FORMAT_, ...)
		#define TRACE_EXIT()
		#define TRACE_EXIT_FORMAT(_FORMAT_, ...)
		#define TRACE_ERROR(_MESSAGE_) \
			TRACE_MESSAGE(cc65c::core::LEVEL_ERROR, std::string(), "%s", _MESSAGE_)
		#define TRACE_ERROR_FORMAT(_FORMAT_, ...) \
			TRACE_MESSAGE(cc65c::core::LEVEL_ERROR, std::string(), _FORMAT_, __VA_ARGS__)
		#define TRACE_WARNING(_MESSAGE_)
		#define TRACE_WARNING_FORMAT(_FORMAT_, ...)
		#define TRACE_INFORMATION(_MESSAGE_)
		#define TRACE_INFORMATION_FORMAT(_FORMAT_, ...)
		#define TRACE_VERBOSE(_MESSAGE_)
		#define TRACE_VERBOSE_FORMAT(_FORMAT_, ...)
	#elif(TRACE == 2) // LEVEL_WARNING
		#define TRACE_ENTRY()
		#define TRACE_ENTRY_FORMAT(_FORMAT_, ...)
		#define TRACE_EXIT()
		#define TRACE_EXIT_FORMAT(_FORMAT_, ...)
		#define TRACE_ERROR(_MESSAGE_) \
			TRACE_MESSAGE(cc65c::core::LEVEL_ERROR, std::string(), "%s", _MESSAGE_)
		#define TRACE_ERROR_FORMAT(_FORMAT_, ...) \
			TRACE_MESSAGE(cc65c::core::LEVEL_ERROR, std::string(), _FORMAT_, __VA_ARGS__)
		#define TRACE_WARNING(_MESSAGE_) \
			TRACE_MESSAGE(cc65c::core::LEVEL_WARNING, std::string(), "%s", _MESSAGE_)
		#define TRACE_WARNING_FORMAT(_FORMAT_, ...) \
			TRACE_MESSAGE(cc65c::core::LEVEL_WARNING, std::string(), _FORMAT_, __VA_ARGS__)
		#define TRACE_INFORMATION(_MESSAGE_)
		#define TRACE_INFORMATION_FORMAT(_FORMAT_, ...)
		#define TRACE_VERBOSE(_MESSAGE_)
		#define TRACE_VERBOSE_FORMAT(_FORMAT_, ...)
	#elif(TRACE == 3) // LEVEL_INFORMATION
		#define TRACE_ENTRY()
		#define TRACE_ENTRY_FORMAT(_FORMAT_, ...)
		#define TRACE_EXIT()
		#define TRACE_EXIT_FORMAT(_FORMAT_, ...)
		#define TRACE_ERROR(_MESSAGE_) \
			TRACE_MESSAGE(cc65c::core::LEVEL_ERROR, std::string(), "%s", _MESSAGE_)
		#define TRACE_ERROR_FORMAT(_FORMAT_, ...) \
			TRACE_MESSAGE(cc65c::core::LEVEL_ERROR, std::string(), _FORMAT_, __VA_ARGS__)
		#define TRACE_WARNING(_MESSAGE_) \
			TRACE_MESSAGE(cc65c::core::LEVEL_WARNING, std::string(), "%s", _MESSAGE_)
		#define TRACE_WARNING_FORMAT(_FORMAT_, ...) \
			TRACE_MESSAGE(cc65c::core::LEVEL_WARNING, std::string(), _FORMAT_, __VA_ARGS__)
		#define TRACE_INFORMATION(_MESSAGE_) \
			TRACE_MESSAGE(cc65c::core::LEVEL_INFORMATION, std::string(), "%s", _MESSAGE_)
		#define TRACE_INFORMATION_FORMAT(_FORMAT_, ...) \
			TRACE_MESSAGE(cc65c::core::LEVEL_INFORMATION, std::string(), _FORMAT_, __VA_ARGS__)
		#define TRACE_VERBOSE(_MESSAGE_)
		#define TRACE_VERBOSE_FORMAT(_FORMAT_, ...)
	#elif(TRACE == 4) // LEVEL_VERBOSE
		#define TRACE_ENTRY() \
			TRACE_MESSAGE(cc65c::core::LEVEL_VERBOSE, TRACE_PREFIX_ENTRY, "", "")
		#define TRACE_ENTRY_FORMAT(_FORMAT_, ...) \
			TRACE_MESSAGE(cc65c::core::LEVEL_VERBOSE, TRACE_PREFIX_ENTRY, _FORMAT_, __VA_ARGS__)
		#define TRACE_EXIT() \
			TRACE_MESSAGE(cc65c::core::LEVEL_VERBOSE, TRACE_PREFIX_EXIT, "", "")
		#define TRACE_EXIT_FORMAT(_FORMAT_, ...) \
			TRACE_MESSAGE(cc65c::core::LEVEL_VERBOSE, TRACE_PREFIX_EXIT, _FORMAT_, __VA_ARGS__)
		#define TRACE_ERROR(_MESSAGE_) \
			TRACE_MESSAGE(cc65c::core::LEVEL_ERROR, std::string(), "%s", _MESSAGE_)
		#define TRACE_ERROR_FORMAT(_FORMAT_, ...) \
			TRACE_MESSAGE(cc65c::core::LEVEL_ERROR, std::string(), _FORMAT_, __VA_ARGS__)
		#define TRACE_WARNING(_MESSAGE_) \
			TRACE_MESSAGE(cc65c::core::LEVEL_WARNING, std::string(), "%s", _MESSAGE_)
		#define TRACE_WARNING_FORMAT(_FORMAT_, ...) \
			TRACE_MESSAGE(cc65c::core::LEVEL_WARNING, std::string(), _FORMAT_, __VA_ARGS__)
		#define TRACE_INFORMATION(_MESSAGE_) \
			TRACE_MESSAGE(cc65c::core::LEVEL_INFORMATION, std::string(), "%s", _MESSAGE_)
		#define TRACE_INFORMATION_FORMAT(_FORMAT_, ...) \
			TRACE_MESSAGE(cc65c::core::LEVEL_INFORMATION, std::string(), _FORMAT_, __VA_ARGS__)
		#define TRACE_VERBOSE(_MESSAGE_) \
			TRACE_MESSAGE(cc65c::core::LEVEL_VERBOSE, std::string(), "%s", _MESSAGE_)
		#define TRACE_VERBOSE_FORMAT(_FORMAT_, ...) \
			TRACE_MESSAGE(cc65c::core::LEVEL_VERBOSE, std::string(), _FORMAT_, __VA_ARGS__)
	#else
		#define TRACE_ENTRY()
		#define TRACE_ENTRY_FORMAT(_FORMAT_, ...)
		#define TRACE_EXIT()
		#define TRACE_EXIT_FORMAT(_FORMAT_, ...)
		#define TRACE_ERROR(_MESSAGE_)
		#define TRACE_ERROR_FORMAT(_FORMAT_, ...)
		#define TRACE_WARNING(_MESSAGE_)
		#define TRACE_WARNING_FORMAT(_FORMAT_, ...)
		#define TRACE_INFORMATION(_MESSAGE_)
		#define TRACE_INFORMATION_FORMAT(_FORMAT_, ...)
		#define TRACE_VERBOSE(_MESSAGE_)
		#define TRACE_VERBOSE_FORMAT(_FORMAT_, ...)
	#endif // (TRACE == [LEVEL_ERROR - LEVEL_VERBOSE])
#else
	#define TRACE_ENTRY()
	#define TRACE_ENTRY_FORMAT(_FORMAT_, ...)
	#define TRACE_EXIT()
	#define TRACE_EXIT_FORMAT(_FORMAT_, ...)
	#define TRACE_ERROR(_MESSAGE_)
	#define TRACE_ERROR_FORMAT(_FORMAT_, ...)
	#define TRACE_WARNING(_MESSAGE_)
	#define TRACE_WARNING_FORMAT(_FORMAT_, ...)
	#define TRACE_INFORMATION(_MESSAGE_)
	#define TRACE_INFORMATION_FORMAT(_FORMAT_, ...)
	#define TRACE_VERBOSE(_MESSAGE_)
	#define TRACE_VERBOSE_FORMAT(_FORMAT_, ...)
#endif // NDEBUG

		typedef enum {
			LEVEL_NONE = 0,
			LEVEL_ERROR,
			LEVEL_WARNING,
			LEVEL_INFORMATION,
			LEVEL_VERBOSE,
		} level_t;

		#define LEVEL_MAX LEVEL_VERBOSE

		typedef std::tuple<std::string, std::string, cc65c::core::level_t, size_t, std::string, std::string, std::time_t> 
			trace_data;

		class trace :
				public cc65c::core::singleton<cc65c::core::trace>,
				protected cc65c::core::thread {

			public:

				~trace(void);

				void initialize(void);

				void send(
					__in cc65c::core::level_t level,
					__in const std::string &prefix,
					__in const std::string &file,
					__in const std::string &function,
					__in size_t line,
					__in const char *format,
					...
					);

				std::string to_string(
					__in_opt bool verbose = false
					);

				void uninitialize(void);

			protected:

				friend class cc65c::core::singleton<cc65c::core::trace>;

				trace(void);

				trace(
					__in const trace &other
					) = delete;

				trace &operator=(
					__in const trace &other
					) = delete;

				static std::string as_string(
					__in const trace &reference,
					__in_opt bool verbose = false
					);

				void flush(void);

				void on_activate(void);

				void on_deactivate(void);

				void on_notify(void);

				bool pop(
					__inout cc65c::core::trace_data &data
					);

				void process(
					__in const cc65c::core::trace_data &data
					);

				void push(
					__in const cc65c::core::trace_data &data
					);

				std::queue<cc65c::core::trace_data> m_queue;

				std::mutex m_queue_mutex;
		};
	}
}

#endif // CC65C_CORE_TRACE_H_
