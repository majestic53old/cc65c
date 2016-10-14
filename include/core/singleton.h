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

#ifndef CC65C_CORE_SINGLETON_H_
#define CC65C_CORE_SINGLETON_H_

#include <mutex>
#include "../define.h"

namespace cc65c {

	namespace core {

		template <class T> class singleton {

			public:

				virtual ~singleton(void)
				{
					return;
				}

				static T &acquire(void)
				{
					static T instance;
					return instance;
				}

				static std::string as_string(
					__in const singleton<T> &reference,
					__in_opt bool verbose = false
					)
				{
					return (reference.m_initialized ? "Initialized" : "Uninitialized"); 
				}

				virtual void initialize(void) = 0;

				bool is_initialized(void)
				{
					std::lock_guard<std::mutex> lock(m_mutex);
					return m_initialized;
				}

				virtual std::string to_string(
					__in_opt bool verbose = false
					)
				{
					return cc65c::core::singleton<T>::as_string(*this, verbose);
				}

				virtual void uninitialize(void) = 0;

			protected:

				singleton(void) :
					m_initialized(false)
				{
					return;
				}

				singleton(
					__in const singleton &other
					) = delete;

				singleton &operator=(
					__in const singleton &other
					) = delete;

				void set_initialized(
					__in bool initialized
					)
				{
					std::lock_guard<std::mutex> lock(m_mutex);
					m_initialized = initialized;
				}

				bool m_initialized;

				std::mutex m_mutex;
		};
	};
}

#endif // CC65C_CORE_SINGLETON_H_
