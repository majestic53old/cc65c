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

#ifndef CC65C_CORE_UUID_H_
#define CC65C_CORE_UUID_H_

#include <map>
#include <set>
#include "singleton.h"

namespace cc65c {

	namespace core {

		typedef uint32_t uuid_t;

		#define UUID_INVALID 0

		class uuid :
				public cc65c::core::singleton<cc65c::core::uuid> {

			public:

				~uuid(void);

				bool contains(
					__in cc65c::core::uuid_t id
					);

				size_t decrement(
					__in cc65c::core::uuid_t id
					);

				cc65c::core::uuid_t generate(void);

				size_t increment(
					__in cc65c::core::uuid_t id
					);

				void initialize(void);

				size_t references(
					__in cc65c::core::uuid_t id
					);

				size_t size(void);

				std::string to_string(
					__in_opt bool verbose = false
					);

				void uninitialize(void);

			protected:

				friend class cc65c::core::singleton<cc65c::core::uuid>;

				uuid(void);

				uuid(
					__in const uuid &other
					) = delete;

				uuid &operator=(
					__in const uuid &other
					) = delete;

				static std::string as_string(
					__in const uuid &reference,
					__in_opt bool verbose = false
					);

				void clear(void);

				std::map<cc65c::core::uuid_t, size_t>::iterator find(
					__in cc65c::core::uuid_t id
					);

				std::map<cc65c::core::uuid_t, size_t> m_id;

				std::mutex m_id_mutex;

				cc65c::core::uuid_t m_id_next;

				std::set<cc65c::core::uuid_t> m_id_surplus;
		};
	}
}

#endif // CC65C_CORE_UUID_H_
