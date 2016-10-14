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

#include "../../include/core/uuid.h"
#include "uuid_type.h"
#include "../../include/core/trace.h"

namespace cc65c {

	namespace core {

		#define UUID_INIT 1

		uuid::uuid(void) :
			m_id_next(UUID_INIT)
		{
			TRACE_ENTRY();
			TRACE_EXIT();
		}

		uuid::~uuid(void)
		{
			TRACE_ENTRY();

			uninitialize();

			TRACE_EXIT();
		}

		std::string 
		uuid::as_string(
			__in const uuid &reference,
			__in_opt bool verbose
			)
		{
			std::stringstream result;
			std::map<cc65c::core::uuid_t, size_t>::const_iterator iter;

			TRACE_ENTRY();

			result << CC65C_CORE_UUID_HEADER
				<< " " << cc65c::core::singleton<cc65c::core::uuid>::as_string(reference, verbose);

			if(verbose) {
				result << ", Instance=" << SCALAR_AS_HEX(uintptr_t, &reference);
			}

			if(reference.m_initialized) {
				result << ", Entry[" << reference.m_id.size() << "]";

				if(verbose) {
					result << "={";

					for(iter = reference.m_id.begin(); iter != reference.m_id.end(); ++iter) {

						if(iter != reference.m_id.begin()) {
							result << ", ";
						}

						result << "{" << SCALAR_AS_HEX(cc65c::core::uuid_t, iter->first)
							<< ", " << iter->second << "}";
					}

					result << "}";
				}

				result << ", Surplus[" << reference.m_id_surplus.size() << "]"
					<< ", Next=" << SCALAR_AS_HEX(cc65c::core::uuid_t, reference.m_id_next);
			}

			TRACE_EXIT();
			return result.str();
		}

		void 
		uuid::clear(void)
		{
			TRACE_ENTRY();

			m_id.clear();
			m_id_next = UUID_INIT;
			m_id_surplus.clear();

			TRACE_EXIT();
		}

		bool 
		uuid::contains(
			__in cc65c::core::uuid_t id
			)
		{
			bool result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_id_mutex);

			if(!cc65c::core::singleton<cc65c::core::uuid>::is_initialized()) {
				THROW_CC65C_CORE_UUID_EXCEPTION(CC65C_CORE_UUID_EXCEPTION_UNINITIALIZED);
			}

			result = (m_id.find(id) != m_id.end());

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		size_t 
		uuid::decrement(
			__in cc65c::core::uuid_t id
			)
		{
			size_t result = 0;
			std::set<cc65c::core::uuid_t>::iterator set_iter;
			std::map<cc65c::core::uuid_t, size_t>::iterator map_iter;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_id_mutex);

			if(!cc65c::core::singleton<cc65c::core::uuid>::is_initialized()) {
				THROW_CC65C_CORE_UUID_EXCEPTION(CC65C_CORE_UUID_EXCEPTION_UNINITIALIZED);
			}

			map_iter = find(id);
			if(map_iter->second <= REFERENCE_INIT) {

				set_iter = m_id_surplus.find(map_iter->first);
				if(set_iter != m_id_surplus.end()) {
					THROW_CC65C_CORE_UUID_EXCEPTION_FORMAT(CC65C_CORE_UUID_EXCEPTION_DUPLICATE,
						"Id=%x", map_iter->first);
				}

				m_id_surplus.insert(map_iter->first);
				m_id.erase(map_iter);
			} else {
				result = --map_iter->second;
			}

			TRACE_EXIT_FORMAT("Result=%u", result);
			return result;
		}

		std::map<cc65c::core::uuid_t, size_t>::iterator 
		uuid::find(
			__in cc65c::core::uuid_t id
			)
		{
			std::map<cc65c::core::uuid_t, size_t>::iterator result;

			TRACE_ENTRY();

			result = m_id.find(id);
			if(result == m_id.end()) {
				THROW_CC65C_CORE_UUID_EXCEPTION_FORMAT(CC65C_CORE_UUID_EXCEPTION_NOT_FOUND,
					"Id=%x", id);
			}

			TRACE_EXIT();
			return result;
		}

		cc65c::core::uuid_t 
		uuid::generate(void)
		{
			cc65c::core::uuid_t result = UUID_INVALID;
			std::set<cc65c::core::uuid_t>::iterator iter;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_id_mutex);

			if(!cc65c::core::singleton<cc65c::core::uuid>::is_initialized()) {
				THROW_CC65C_CORE_UUID_EXCEPTION(CC65C_CORE_UUID_EXCEPTION_UNINITIALIZED);
			}

			if(!m_id_surplus.empty()) {
				iter = m_id_surplus.begin();
				result = *iter;
				m_id_surplus.erase(iter);
			} else if(m_id_next != UUID_INVALID) {
				result = m_id_next++;
			} else {
				THROW_CC65C_CORE_UUID_EXCEPTION(CC65C_CORE_UUID_EXCEPTION_FULL);
			}

			if(m_id.find(result) != m_id.end()) {
				THROW_CC65C_CORE_UUID_EXCEPTION_FORMAT(CC65C_CORE_UUID_EXCEPTION_DUPLICATE,
					"Id=%x", result);
			}

			m_id.insert(std::make_pair(result, REFERENCE_INIT));

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		size_t 
		uuid::increment(
			__in cc65c::core::uuid_t id
			)
		{
			size_t result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_id_mutex);

			if(!cc65c::core::singleton<cc65c::core::uuid>::is_initialized()) {
				THROW_CC65C_CORE_UUID_EXCEPTION(CC65C_CORE_UUID_EXCEPTION_UNINITIALIZED);
			}

			result = ++find(id)->second;

			TRACE_EXIT_FORMAT("Result=%u", result);
			return result;
		}

		void 
		uuid::initialize(void)
		{
			TRACE_ENTRY();

			if(cc65c::core::singleton<cc65c::core::uuid>::is_initialized()) {
				THROW_CC65C_CORE_UUID_EXCEPTION(CC65C_CORE_UUID_EXCEPTION_INITIALIZED);
			}

			cc65c::core::singleton<cc65c::core::uuid>::set_initialized(true);
			TRACE_INFORMATION("Uuid manager initialized");

			TRACE_EXIT();
		}

		size_t 
		uuid::references(
			__in cc65c::core::uuid_t id
			)
		{
			size_t result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_id_mutex);

			if(!cc65c::core::singleton<cc65c::core::uuid>::is_initialized()) {
				THROW_CC65C_CORE_UUID_EXCEPTION(CC65C_CORE_UUID_EXCEPTION_UNINITIALIZED);
			}

			result = find(id)->second;

			TRACE_EXIT_FORMAT("Result=%u", result);
			return result;
		}

		size_t 
		uuid::size(void)
		{
			size_t result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_id_mutex);
			result = m_id.size();

			TRACE_EXIT_FORMAT("Result=%u", result);
			return result;
		}

		std::string 
		uuid::to_string(
			__in_opt bool verbose
			)
		{
			std::string result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_id_mutex);
			result = cc65c::core::uuid::as_string(*this, verbose);

			TRACE_EXIT();
			return result;
		}

		void 
		uuid::uninitialize(void)
		{
			TRACE_ENTRY();

			if(cc65c::core::singleton<cc65c::core::uuid>::is_initialized()) {
				cc65c::core::singleton<cc65c::core::uuid>::set_initialized(false);
				clear();
				TRACE_INFORMATION("Uuid manager uninitialized");
			}

			TRACE_EXIT();
		}
	}
}
