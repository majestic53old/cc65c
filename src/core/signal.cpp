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

#include "../../include/core/signal.h"
#include "signal_type.h"

namespace cc65c {

	namespace core {

		static const std::string SIGNAL_STR[] = {
			"ABORT", "CLEAR", "SET", "TIMEOUT",
			};

		#define SIGNAL_STRING(_TYPE_) \
			((_TYPE_) > SIGNAL_MAX ? STRING_UNKNOWN : STRING_CHECK(SIGNAL_STR[_TYPE_]))

		signal::signal(
			__in_opt bool set,
			__in_opt bool resetting
			) :
				m_resetting(resetting),
				m_state(set ? SIGNAL_SET : SIGNAL_CLEAR)
		{
			return;
		}

		signal::~signal(void)
		{
			abort();
		}

		void 
		signal::abort(void)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			m_state = SIGNAL_ABORT;
			m_condition.notify_all();
		}

		std::string 
		signal::as_string(
			__in const signal &reference,
			__in_opt bool verbose
			)
		{
			std::stringstream result;

			result << SCALAR_AS_HEX(cc65c::core::signal_t, reference.m_state)
				<< "(" << SIGNAL_STRING(reference.m_state) << ")";

			if(verbose) {
				result << ", " << (reference.m_resetting ? "Resetting" : "Non-resetting");
			}

			return result.str();
		}

		void 
		signal::clear(void)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			if((m_state == SIGNAL_ABORT) || (m_state > SIGNAL_MAX)) {
				THROW_CC65C_CORE_SIGNAL_EXCEPTION_FORMAT(CC65C_CORE_SIGNAL_EXCEPTION_INVALID,
					"%x(%s)", m_state, SIGNAL_STRING(m_state));
			}

			m_state = SIGNAL_CLEAR;
			m_condition.notify_all();
		}

		void 
		signal::set(void)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			if((m_state == SIGNAL_ABORT) || (m_state > SIGNAL_MAX)) {
				THROW_CC65C_CORE_SIGNAL_EXCEPTION_FORMAT(CC65C_CORE_SIGNAL_EXCEPTION_INVALID,
					"%x(%s)", m_state, SIGNAL_STRING(m_state));
			}

			m_state = SIGNAL_SET;
			m_condition.notify_all();
		}

		cc65c::core::signal_t 
		signal::state(void)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			return m_state;
		}

		std::string 
		signal::to_string(
			__in_opt bool verbose
			)
		{
			return cc65c::core::signal::as_string(*this, verbose);
		}

		cc65c::core::signal_t 
		signal::wait(
			__in_opt uint32_t timeout
			)
		{
			cc65c::core::signal_t result;

			std::unique_lock<std::mutex> lock(m_mutex);

			result = m_state;
			if((m_state == SIGNAL_ABORT) || (m_state > SIGNAL_MAX)) {
				THROW_CC65C_CORE_SIGNAL_EXCEPTION_FORMAT(CC65C_CORE_SIGNAL_EXCEPTION_INVALID,
					"%x(%s)", m_state, SIGNAL_STRING(m_state));
			} else if(m_state == SIGNAL_CLEAR) {

				if(timeout != SIGNAL_TIMEOUT_INFINITE) {

					if(!m_condition.wait_for(lock, std::chrono::milliseconds(timeout),
							[this](void) { return (m_state != SIGNAL_CLEAR); })) {
						m_state = SIGNAL_TIMEOUT;
					}
				} else {
					m_condition.wait(lock, [this](void) { return (m_state != SIGNAL_CLEAR); });
				}

				result = m_state;

				if(m_resetting && (m_state != SIGNAL_ABORT)) {
					m_state = SIGNAL_CLEAR;
				}
			}

			return result;
		}
	}
}
