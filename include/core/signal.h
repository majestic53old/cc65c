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

#ifndef CC65C_CORE_SIGNAL_H_
#define CC65C_CORE_SIGNAL_H_

#include <condition_variable>
#include "../define.h"

namespace cc65c {

	namespace core {

		typedef enum {
			SIGNAL_ABORT = 0,
			SIGNAL_CLEAR,
			SIGNAL_SET,
			SIGNAL_TIMEOUT,
		} signal_t;

		#define SIGNAL_MAX SIGNAL_TIMEOUT
		#define SIGNAL_TIMEOUT_INFINITE SCALAR_INVALID(uint32_t)

		class signal {

			public:

				signal(
					__in_opt bool set = false,
					__in_opt bool resetting = true
					);

				virtual ~signal(void);

				static std::string as_string(
					__in const signal &reference,
					__in_opt bool verbose = false
					);

				void clear(void);

				void set(void);

				cc65c::core::signal_t state(void);

				virtual std::string to_string(
					__in_opt bool verbose = false
					);

				cc65c::core::signal_t wait(
					__in_opt uint32_t timeout = SIGNAL_TIMEOUT_INFINITE
					);

			protected:

				signal(
					__in const signal &other
					) = delete;

				signal &operator=(
					__in const signal &other
					) = delete;

				void abort(void);

				std::condition_variable m_condition;

				std::mutex m_mutex;

				bool m_resetting;

				cc65c::core::signal_t m_state;
		};
	}
}

#endif // CC65C_CORE_SIGNAL_H_
