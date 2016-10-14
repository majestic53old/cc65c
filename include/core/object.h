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

#ifndef CC65C_CORE_OBJECT_H_
#define CC65C_CORE_OBJECT_H_

#include "uuid.h"

namespace cc65c {

	namespace core {

		typedef enum {
			OBJECT_UNDEFINED = 0,
			OBJECT_NODE,
			OBJECT_TOKEN,
			OBJECT_TREE,
		} object_t;

		#define OBJECT_MAX OBJECT_TREE
		#define SUBTYPE_UNDEFINED SCALAR_INVALID(uint32_t)

		class object {

			public:

				object(
					__in cc65c::core::object_t type,
					__in_opt uint32_t subtype = SUBTYPE_UNDEFINED
					);

				object(
					__in cc65c::core::uuid_t id,
					__in cc65c::core::object_t type,
					__in_opt uint32_t subtype = SUBTYPE_UNDEFINED
					);

				object(
					__in const object &other
					);

				virtual ~object(void);

				object &operator=(
					__in const object &other
					);

				bool operator==(
					__in const object &other
					);

				bool operator!=(
					__in const object &other
					);

				static cc65c::core::uuid_t as_id(
					__in const object &reference
					);

				static std::string as_string(
					__in const object &reference,
					__in_opt bool verbose = false
					);

				cc65c::core::uuid_t id(void);

				uint32_t subtype(void);

				virtual std::string to_string(
					__in_opt bool verbose = false
					);

				cc65c::core::object_t type(void);

			protected:

				void decrement(void);

				void generate(void);

				void increment(void);

				cc65c::core::uuid_t m_id;

				uint32_t m_subtype;

				cc65c::core::object_t m_type;
		};
	}
}

#endif // CC65C_CORE_OBJECT_H_
