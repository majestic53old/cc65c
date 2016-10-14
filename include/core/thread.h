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

#ifndef CC65C_CORE_THREAD_H_
#define CC65C_CORE_THREAD_H_

#include <thread>
#include "exception.h"
#include "signal.h"

namespace cc65c {

	namespace core {

		class thread {

			public:

				thread(void);

				virtual ~thread(void);

				void activate(
					__in_opt bool freerunning = true,
					__in_opt bool detach = false,
					__in_opt uint32_t timeout = SIGNAL_TIMEOUT_INFINITE
					);

				static std::string as_string(
					__in const thread &reference,
					__in_opt bool verbose = false
					);

				void deactivate(
					__in_opt uint32_t timeout = SIGNAL_TIMEOUT_INFINITE
					);

				bool is_active(void);

				bool is_freerunning(void);

				void notify(void);

				virtual std::string to_string(
					__in_opt bool verbose = false
					);

				bool wait(
					__in_opt uint32_t timeout = SIGNAL_TIMEOUT_INFINITE
					);

			protected:

				thread(
					__in const thread &other
					) = delete;

				thread &operator=(
					__in const thread &other
					) = delete;

				void loop(
					__in_opt uint32_t timeout = SIGNAL_TIMEOUT_INFINITE
					);

				virtual void on_activate(void) = 0;

				virtual void on_deactivate(void) = 0;

				virtual void on_loop(void);

				virtual void on_notify(void);

				void set_active(
					__in bool active
					);

				bool m_active;

				bool m_detach;

				cc65c::core::exception m_exception;

				bool m_freerunning;

				std::mutex m_mutex;

				cc65c::core::signal m_signal_entry;

				cc65c::core::signal m_signal_exit;

				cc65c::core::signal m_signal_ready;

				cc65c::core::signal m_signal_wait;

				std::thread m_thread;
		};
	}
}

#endif // CC65C_CORE_THREAD_H_
