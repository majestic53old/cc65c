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

#include "../../include/core/thread.h"
#include "thread_type.h"

namespace cc65c {

	namespace core {

		thread::thread(void) :
			m_active(false),
			m_detach(false),
			m_freerunning(false)
		{
			return;
		}

		thread::~thread(void)
		{
			deactivate();
		}

		void 
		thread::activate(
			__in_opt bool freerunning,
			__in_opt bool detach,
			__in_opt uint32_t timeout
			)
		{

			if(is_active()) {
				THROW_CC65C_CORE_THREAD_EXCEPTION(CC65C_CORE_THREAD_EXCEPTION_ACTIVATED);
			}

			on_activate();
			m_detach = detach;
			m_freerunning = freerunning;
			m_thread = std::thread(&cc65c::core::thread::loop, this, timeout);

			if(SIGNAL_TIMEOUT == m_signal_entry.wait(timeout)) {
				THROW_CC65C_CORE_THREAD_EXCEPTION_FORMAT(CC65C_CORE_THREAD_EXCEPTION_TIMEOUT,
					"Timeout=%ums", timeout);
			}

			if(m_detach) {
				m_thread.detach();
			}

			set_active(true);
			m_signal_ready.set();
		}

		std::string 
		thread::as_string(
			__in const thread &reference,
			__in_opt bool verbose
			)
		{
			std::stringstream result;

			result << (reference.m_active ? "Active" : "Inactive");

			if(verbose && reference.m_active) {

				if(!reference.m_detach) {
					result << ", Tid=" << reference.m_thread.get_id();
				}

				result << ", " << (reference.m_freerunning ? "Freerunning" : "Signallable")
					<< ", " << (reference.m_detach ? "Detached" : "Attached");	
			}

			return result.str();
		}

		void 
		thread::deactivate(
			__in_opt uint32_t timeout
			)
		{

			if(is_active()) {
				set_active(false);
				m_signal_wait.set();

				if(SIGNAL_TIMEOUT == m_signal_exit.wait(timeout)) {
					THROW_CC65C_CORE_THREAD_EXCEPTION_FORMAT(CC65C_CORE_THREAD_EXCEPTION_TIMEOUT,
						"Timeout=%ums", timeout);
				}

				if(m_thread.joinable()) {
					m_thread.join();
				}

				if(!m_exception.is_empty()) {
					throw m_exception;
				}

				m_detach = false;
				m_exception.clear();
				m_freerunning = false;
				m_signal_entry.clear();
				m_signal_exit.clear();
				m_signal_ready.clear();
				m_signal_wait.clear();
				on_deactivate();
			}
		}

		bool 
		thread::is_active(void)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			return m_active;
		}

		bool 
		thread::is_freerunning(void)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			return m_freerunning;
		}

		void 
		thread::loop(
			__in_opt uint32_t timeout
			)
		{
			m_signal_entry.set();

			try {

				if(SIGNAL_TIMEOUT == m_signal_ready.wait(timeout)) {
					THROW_CC65C_CORE_THREAD_EXCEPTION_FORMAT(CC65C_CORE_THREAD_EXCEPTION_TIMEOUT,
						"Timeout=%ums", timeout);
				}

				if(is_active()) {

					if(!is_freerunning()) {

						for(;;) {
							m_signal_wait.wait();

							if(!is_active()) {
								break;
							}

							on_notify();
						}
					} else {
						on_loop();
					}
				}
			} catch(cc65c::core::exception &exc) {
				m_exception = exc;
			}

			m_signal_exit.set();
		}

		void 
		thread::notify(void)
		{

			if(!is_active()) {
				THROW_CC65C_CORE_THREAD_EXCEPTION(CC65C_CORE_THREAD_EXCEPTION_DEACTIVATED);
			}

			m_signal_wait.set();
		}

		void 
		thread::on_loop(void)
		{

			for(;;) {
				m_signal_wait.wait();

				if(!is_active()) {
					break;
				}

				on_notify();
			}
		}

		void 
		thread::on_notify(void)
		{
			return;
		}

		void 
		thread::set_active(
			__in bool active
			)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_active = active;
		}

		std::string 
		thread::to_string(
			__in_opt bool verbose
			)
		{
			return cc65c::core::thread::as_string(*this, verbose);
		}

		bool 
		thread::wait(
			__in_opt uint32_t timeout
			)
		{
			return (m_signal_exit.wait(timeout) == SIGNAL_SET);
		}
	}
}
