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

#ifndef CC65C_RUNTIME_H_
#define CC65C_RUNTIME_H_

#include "assembler/symbol.h"
#include "core/exception.h"
#include "core/singleton.h"
#include "core/trace.h"
#include "core/uuid.h"

namespace cc65c {

	class runtime :
			public cc65c::core::singleton<cc65c::runtime> {

		public:

			~runtime(void);

			void initialize(void);

			std::string to_string(
				__in_opt bool verbose = false
				);

			void uninitialize(void);

			static std::string version(
				__in_opt bool verbose = false
				);

		protected:

			friend class cc65c::core::singleton<cc65c::runtime>;

			runtime(void);

			static std::string as_string(
				__in const runtime &reference,
				__in_opt bool verbose = false
				);

			void setup(void);

			void teardown(void);

			cc65c::assembler::symbol &m_symbol;

			cc65c::core::trace &m_trace;

			cc65c::core::uuid &m_uuid;
	};
}

#endif // CC65C_RUNTIME_H_
