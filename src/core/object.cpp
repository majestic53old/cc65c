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

#include "../../include/core/object.h"
#include "object_type.h"
#include "../../include/core/trace.h"

namespace cc65c {

	namespace core {

		static const std::string OBJECT_STR[] = {
			"UNDEFINED", "NODE", "TOKEN", "TREE",
			};

		#define OBJECT_STRING(_TYPE_) \
			((_TYPE_) > OBJECT_MAX ? STRING_UNKNOWN : STRING_CHECK(OBJECT_STR[_TYPE_]))

		object::object(
			__in cc65c::core::object_t type,
			__in_opt uint32_t subtype
			) :
				m_id(UUID_INVALID),
				m_subtype(subtype),
				m_type(type)
		{
			TRACE_ENTRY();

			if(type > OBJECT_MAX) {
				THROW_CC65C_CORE_OBJECT_EXCEPTION_FORMAT(CC65C_CORE_OBJECT_EXCEPTION_INVALID,
					"Type=%x", type);
			}

			generate();

			TRACE_EXIT();
		}

		object::object(
			__in cc65c::core::uuid_t id,
			__in cc65c::core::object_t type,
			__in_opt uint32_t subtype
			) :
				m_id(id),
				m_subtype(subtype),
				m_type(type)
		{
			TRACE_ENTRY();

			if(type > OBJECT_MAX) {
				THROW_CC65C_CORE_OBJECT_EXCEPTION_FORMAT(CC65C_CORE_OBJECT_EXCEPTION_INVALID,
					"Type=%x", type);
			}

			increment();

			TRACE_EXIT();
		}

		object::object(
			__in const object &other
			) :
				m_id(other.m_id),
				m_subtype(other.m_subtype),
				m_type(other.m_type)
		{
			TRACE_ENTRY();

			increment();

			TRACE_EXIT();
		}

		object::~object(void)
		{
			TRACE_ENTRY();

			decrement();

			TRACE_EXIT();
		}

		object &
		object::operator=(
			__in const object &other
			)
		{
			TRACE_ENTRY();

			if(this != &other) {
				decrement();
				m_id = other.m_id;
				m_subtype = other.m_subtype;
				m_type = other.m_type;
				increment();
			}

			TRACE_EXIT_FORMAT("Result=%p", this);
			return *this;
		}

		bool 
		object::operator==(
			__in const object &other
			)
		{
			bool result;

			TRACE_ENTRY();

			result = ((this == &other)
				|| ((m_id == other.m_id)
				&& (m_subtype == other.m_subtype)
				&& (m_type == other.m_type)));

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		bool 
		object::operator!=(
			__in const object &other
			)
		{
			bool result;

			TRACE_ENTRY();

			result = ((this != &other)
				&& ((m_id != other.m_id)
				|| (m_subtype != other.m_subtype)
				|| (m_type != other.m_type)));

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		cc65c::core::uuid_t 
		object::as_id(
			__in const object &reference
			)
		{
			cc65c::core::uuid_t result;

			TRACE_ENTRY();

			result = reference.m_id;

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		std::string 
		object::as_string(
			__in const object &reference,
			__in_opt bool verbose
			)
		{
			std::stringstream result;

			TRACE_ENTRY();

			result << "[" << OBJECT_STRING(reference.m_type);

			if(reference.m_subtype != SUBTYPE_UNDEFINED) {
				result << ", " << SCALAR_AS_HEX(uint32_t, reference.m_subtype);
			}

			result << "]";

			if(verbose) {
				result << " Id=" << SCALAR_AS_HEX(cc65c::core::uuid_t, reference.m_id);
			}

			TRACE_EXIT();
			return result.str();
		}

		void 
		object::decrement(void)
		{
			TRACE_ENTRY();

			cc65c::core::uuid &instance = cc65c::core::uuid::acquire();
			if(instance.is_initialized() && instance.contains(m_id)) {
				instance.decrement(m_id);
			}

			TRACE_EXIT();
		}

		void 
		object::generate(void)
		{
			TRACE_ENTRY();

			cc65c::core::uuid &instance = cc65c::core::uuid::acquire();
			if(instance.is_initialized()) {
				m_id = instance.generate();
			}

			TRACE_EXIT();
		}

		cc65c::core::uuid_t 
		object::id(void)
		{
			TRACE_ENTRY();
			TRACE_EXIT_FORMAT("Result=%x", m_id);
			return m_id;
		}

		void 
		object::increment(void)
		{
			TRACE_ENTRY();

			cc65c::core::uuid &instance = cc65c::core::uuid::acquire();
			if(instance.is_initialized() && instance.contains(m_id)) {
				instance.increment(m_id);
			}

			TRACE_EXIT();
		}

		uint32_t 
		object::subtype(void)
		{
			TRACE_ENTRY();
			TRACE_EXIT_FORMAT("Result=%x", m_subtype);
			return m_subtype;
		}

		std::string 
		object::to_string(
			__in_opt bool verbose
			)
		{
			std::string result;

			TRACE_ENTRY();

			result = cc65c::core::object::as_string(*this, verbose);

			TRACE_EXIT();
			return result;
		}

		cc65c::core::object_t 
		object::type(void)
		{
			TRACE_ENTRY();
			TRACE_EXIT_FORMAT("Result=%x", m_type);
			return m_type;
		}
	}
}
