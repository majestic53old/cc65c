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

#include "../../include/assembler/symbol.h"
#include "symbol_type.h"
#include "../../include/core/trace.h"

namespace cc65c {

	namespace assembler {

		symbol::symbol(void)
		{
			TRACE_ENTRY();
			TRACE_EXIT();
		}

		symbol::~symbol(void)
		{
			TRACE_ENTRY();

			uninitialize();

			TRACE_EXIT();
		}

		std::string 
		symbol::as_string(
			__in const symbol &reference,
			__in_opt bool verbose
			)
		{
			std::stringstream result;
			std::vector<cc65c::core::uuid_t>::const_iterator node_child_iter;
			std::map<cc65c::core::uuid_t, std::pair<cc65c::assembler::node_metadata_t, size_t>>::const_iterator node_iter;
			std::map<cc65c::core::uuid_t, std::pair<cc65c::assembler::token_metadata_t, size_t>>::const_iterator token_iter;

			TRACE_ENTRY();

			result << CC65C_ASSEMBLER_SYMBOL_HEADER
				<< " " << cc65c::core::singleton<cc65c::assembler::symbol>::as_string(reference, verbose);

			if(verbose) {
				result << ", Instance=" << SCALAR_AS_HEX(uintptr_t, &reference);
			}

			if(reference.m_initialized) {

				if(!reference.m_token.empty()) {
					result << ", " << std::endl << "Token[" << reference.m_token.size() << "]";

					if(verbose) {
						result << "={";

						for(token_iter = reference.m_token.begin(); token_iter != reference.m_token.end(); ++token_iter) {

							if(token_iter != reference.m_token.begin()) {
								result << ", ";
							}

							if(!std::get<TOKEN_METADATA_KEY>(token_iter->second.first)
									|| !std::get<TOKEN_METADATA_VALUE>(token_iter->second.first)) {
								THROW_CC65C_ASSEMBLER_SYMBOL_EXCEPTION_FORMAT(
									CC65C_ASSEMBLER_SYMBOL_EXCEPTION_INVALID_TOKEN,
									"Id=%x, Key=%p, Value=%p", token_iter->first, 
									std::get<TOKEN_METADATA_KEY>(token_iter->second.first),
									std::get<TOKEN_METADATA_VALUE>(token_iter->second.first));
							}

							result << std::endl << "\t{" << SCALAR_AS_HEX(cc65c::core::uuid_t, token_iter->first)
								<< ", {" << SCALAR_AS_HEX(cc65c::assembler::token_t, 
										std::get<TOKEN_METADATA_TYPE>(token_iter->second.first))
								<< ", " << SCALAR_AS_HEX(uint32_t, 
										std::get<TOKEN_METADATA_SUBTYPE>(token_iter->second.first))
								<< ", [" << std::get<TOKEN_METADATA_KEY>(token_iter->second.first)->size() 
									<< "]\"" << *std::get<TOKEN_METADATA_KEY>(token_iter->second.first) << "\""
								<< " (" << std::get<TOKEN_METADATA_ROW>(token_iter->second.first)
								<< ":" << std::get<TOKEN_METADATA_COLUMN>(token_iter->second.first) << ")"
								<< ", " << *std::get<TOKEN_METADATA_VALUE>(token_iter->second.first) << "}"
								<< ", " << token_iter->second.second << "}";
						}

						result << std::endl << "},";
					}
				}

				if(!reference.m_node.empty()) {
					result << std::endl << "Node[" << reference.m_node.size() << "]";

					if(verbose) {
						result << "={";

						for(node_iter = reference.m_node.begin(); node_iter != reference.m_node.end(); ++node_iter) {

							if(node_iter != reference.m_node.begin()) {
								result << ", ";
							}

							if(!std::get<NODE_METADATA_TOKEN>(node_iter->second.first)
									|| !std::get<NODE_METADATA_PARENT>(node_iter->second.first)
									|| !std::get<NODE_METADATA_CHILD>(node_iter->second.first)) {
								THROW_CC65C_ASSEMBLER_SYMBOL_EXCEPTION_FORMAT(
									CC65C_ASSEMBLER_SYMBOL_EXCEPTION_INVALID_NODE,
									"Id=%x, Token=%p, Parent=%p, Child=%p", node_iter->first, 
									std::get<NODE_METADATA_TOKEN>(node_iter->second.first), 
									std::get<NODE_METADATA_PARENT>(node_iter->second.first),
									std::get<NODE_METADATA_CHILD>(node_iter->second.first));
							}

							result << std::endl << "\t{" << SCALAR_AS_HEX(cc65c::core::uuid_t, node_iter->first)
								<< ", {" << SCALAR_AS_HEX(cc65c::core::uuid_t, *std::get<NODE_METADATA_TOKEN>(
									node_iter->second.first))
								<< "(" << SCALAR_AS_HEX(uintptr_t, std::get<NODE_METADATA_TOKEN>(
									node_iter->second.first)) << ")"
								<< ", " << SCALAR_AS_HEX(cc65c::core::uuid_t, *std::get<NODE_METADATA_PARENT>(
									node_iter->second.first))
								<< "(" << SCALAR_AS_HEX(uintptr_t, std::get<NODE_METADATA_PARENT>(
									node_iter->second.first)) << ")"
								<< ", " << SCALAR_AS_HEX(uintptr_t, std::get<NODE_METADATA_CHILD>(
									node_iter->second.first))
								<< "[" << std::get<NODE_METADATA_CHILD>(node_iter->second.first)->size() << "]={";

							for(node_child_iter = std::get<NODE_METADATA_CHILD>(node_iter->second.first)->begin();
									node_child_iter != std::get<NODE_METADATA_CHILD>(node_iter->second.first)->end();
									++node_child_iter) {

								if(node_child_iter != std::get<NODE_METADATA_CHILD>(node_iter->second.first)->begin()) {
									result << ", ";
								}

								result << SCALAR_AS_HEX(cc65c::core::uuid_t, *node_child_iter);
							}

							result << "}}, " << node_iter->second.second << "}";
						}

						result << std::endl << "}";
					}
				}
			}

			TRACE_EXIT();
			return result.str();
		}

		void 
		symbol::clear(void)
		{
			std::map<cc65c::core::uuid_t, std::pair<cc65c::assembler::node_metadata_t, size_t>>::iterator node_iter;
			std::map<cc65c::core::uuid_t, std::pair<cc65c::assembler::token_metadata_t, size_t>>::iterator token_iter;

			TRACE_ENTRY();

			for(node_iter = m_node.begin(); node_iter != m_node.end(); ++node_iter) {
				destroy_node(node_iter);
			}

			m_node.clear();

			for(token_iter = m_token.begin(); token_iter != m_token.end(); ++token_iter) {
				destroy_token(token_iter);
			}

			m_token.clear();

			TRACE_EXIT();
		}

		bool 
		symbol::contains_node(
			__in cc65c::core::uuid_t id
			)
		{
			bool result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_symbol_mutex);

			if(!cc65c::core::singleton<cc65c::assembler::symbol>::is_initialized()) {
				THROW_CC65C_ASSEMBLER_SYMBOL_EXCEPTION(CC65C_ASSEMBLER_SYMBOL_EXCEPTION_UNINITIALIZED);
			}

			result = (m_node.find(id) != m_node.end());

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		bool 
		symbol::contains_token(
			__in cc65c::core::uuid_t id
			)
		{
			bool result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_symbol_mutex);

			if(!cc65c::core::singleton<cc65c::assembler::symbol>::is_initialized()) {
				THROW_CC65C_ASSEMBLER_SYMBOL_EXCEPTION(CC65C_ASSEMBLER_SYMBOL_EXCEPTION_UNINITIALIZED);
			}

			result = (m_token.find(id) != m_token.end());

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		void 
		symbol::create_node(
			__in cc65c::core::uuid_t id,
			__in cc65c::core::uuid_t token_id,
			__in cc65c::core::uuid_t node_parent,
			__in const std::vector<cc65c::core::uuid_t> &node_child,
			__inout cc65c::core::uuid_t *&reference_id,
			__inout cc65c::core::uuid_t *&reference_parent,
			__inout std::vector<cc65c::core::uuid_t> *&reference_child
			)
		{
			std::map<cc65c::core::uuid_t, std::pair<cc65c::assembler::node_metadata_t, size_t>>::iterator iter;

			TRACE_ENTRY();

			iter = m_node.find(id);
			if(iter != m_node.end()) {
				THROW_CC65C_ASSEMBLER_SYMBOL_EXCEPTION_FORMAT(CC65C_ASSEMBLER_SYMBOL_EXCEPTION_DUPLICATE_NODE,
					"Id=%x", id);
			}

			reference_id = new cc65c::core::uuid_t;
			if(!reference_id) {
				THROW_CC65C_ASSEMBLER_SYMBOL_EXCEPTION_FORMAT(CC65C_ASSEMBLER_SYMBOL_EXCEPTION_NODE_ALLOCATION,
					"Id=%x, Token=%p", id, reference_id);
			}

			*reference_id = token_id;

			reference_parent = new cc65c::core::uuid_t;
			if(!reference_parent) {
				THROW_CC65C_ASSEMBLER_SYMBOL_EXCEPTION_FORMAT(CC65C_ASSEMBLER_SYMBOL_EXCEPTION_NODE_ALLOCATION,
					"Id=%x, Parent=%p", id, reference_parent);
			}

			*reference_parent = node_parent;

			reference_child = new std::vector<cc65c::core::uuid_t>();
			if(!reference_child) {
				THROW_CC65C_ASSEMBLER_SYMBOL_EXCEPTION_FORMAT(CC65C_ASSEMBLER_SYMBOL_EXCEPTION_NODE_ALLOCATION,
					"Id=%x, Child=%p", id, reference_child);
			}

			*reference_child = node_child;
			m_node.insert(std::make_pair(id, std::make_pair(std::make_tuple(reference_id, reference_parent, reference_child), 
				REFERENCE_INIT)));

			TRACE_EXIT();
		}

		void 
		symbol::create_token(
			__in cc65c::core::uuid_t id,
			__in cc65c::assembler::token_t type,
			__in uint32_t subtype,
			__in const std::string &key,
			__in int32_t value,
			__inout std::string *&reference_key,
			__inout int32_t *&reference_value,
			__in_opt cc65c::core::stream_t row,
			__in_opt cc65c::core::stream_t column
			)
		{
			std::map<cc65c::core::uuid_t, std::pair<cc65c::assembler::token_metadata_t, size_t>>::iterator iter;

			TRACE_ENTRY();

			iter = m_token.find(id);
			if(iter != m_token.end()) {
				THROW_CC65C_ASSEMBLER_SYMBOL_EXCEPTION_FORMAT(CC65C_ASSEMBLER_SYMBOL_EXCEPTION_DUPLICATE_TOKEN,
					"Id=%x", id);
			}

			reference_key = new std::string;
			if(!reference_key) {
				THROW_CC65C_ASSEMBLER_SYMBOL_EXCEPTION_FORMAT(CC65C_ASSEMBLER_SYMBOL_EXCEPTION_TOKEN_ALLOCATION,
					"Id=%x, Key=%p", id, reference_key);
			}

			*reference_key = key;

			reference_value = new int32_t;
			if(!reference_value) {
				THROW_CC65C_ASSEMBLER_SYMBOL_EXCEPTION_FORMAT(CC65C_ASSEMBLER_SYMBOL_EXCEPTION_TOKEN_ALLOCATION,
					"Id=%x, Value=%p", id, reference_value);
			}

			*reference_value = value;

			m_token.insert(std::make_pair(id, std::make_pair(std::make_tuple(column, reference_key, row, subtype, type, 
				reference_value), REFERENCE_INIT)));

			TRACE_EXIT();
		}

		size_t 
		symbol::decrement_node(
			__in cc65c::core::uuid_t id
			)
		{
			size_t result = 0;
			std::map<cc65c::core::uuid_t, std::pair<cc65c::assembler::node_metadata_t, size_t>>::iterator iter;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_symbol_mutex);

			if(!cc65c::core::singleton<cc65c::assembler::symbol>::is_initialized()) {
				THROW_CC65C_ASSEMBLER_SYMBOL_EXCEPTION(CC65C_ASSEMBLER_SYMBOL_EXCEPTION_UNINITIALIZED);
			}

			iter = find_node(id);
			if(iter->second.second <= REFERENCE_INIT) {
				destroy_node(iter);
				m_node.erase(iter);
			} else {
				result = --iter->second.second;
			}

			TRACE_EXIT_FORMAT("Result=%u", result);
			return result;
		}

		size_t 
		symbol::decrement_token(
			__in cc65c::core::uuid_t id
			)
		{
			size_t result = 0;
			std::map<cc65c::core::uuid_t, std::pair<cc65c::assembler::token_metadata_t, size_t>>::iterator iter;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_symbol_mutex);

			if(!cc65c::core::singleton<cc65c::assembler::symbol>::is_initialized()) {
				THROW_CC65C_ASSEMBLER_SYMBOL_EXCEPTION(CC65C_ASSEMBLER_SYMBOL_EXCEPTION_UNINITIALIZED);
			}

			iter = find_token(id);
			if(iter->second.second <= REFERENCE_INIT) {
				destroy_token(iter);
				m_token.erase(iter);
			} else {
				result = --iter->second.second;
			}

			TRACE_EXIT_FORMAT("Result=%u", result);
			return result;
		}

		void 
		symbol::destroy_node(
			__in std::map<cc65c::core::uuid_t, std::pair<cc65c::assembler::node_metadata_t, size_t>>::iterator &entry
			)
		{
			TRACE_ENTRY();

			if(std::get<NODE_METADATA_TOKEN>(entry->second.first)) {
				delete std::get<NODE_METADATA_TOKEN>(entry->second.first);
				std::get<NODE_METADATA_TOKEN>(entry->second.first) = nullptr;
			}

			if(std::get<NODE_METADATA_PARENT>(entry->second.first)) {
				delete std::get<NODE_METADATA_PARENT>(entry->second.first);
				std::get<NODE_METADATA_PARENT>(entry->second.first) = nullptr;
			}

			if(std::get<NODE_METADATA_CHILD>(entry->second.first)) {
				delete std::get<NODE_METADATA_CHILD>(entry->second.first);
				std::get<NODE_METADATA_CHILD>(entry->second.first) = nullptr;
			}

			TRACE_EXIT();
		}

		void 
		symbol::destroy_token(
			__in std::map<cc65c::core::uuid_t, std::pair<cc65c::assembler::token_metadata_t, size_t>>::iterator &entry
			)
		{
			TRACE_ENTRY();

			if(std::get<TOKEN_METADATA_KEY>(entry->second.first)) {
				delete std::get<TOKEN_METADATA_KEY>(entry->second.first);
				std::get<TOKEN_METADATA_KEY>(entry->second.first) = nullptr;
			}

			if(std::get<TOKEN_METADATA_VALUE>(entry->second.first)) {
				delete std::get<TOKEN_METADATA_VALUE>(entry->second.first);
				std::get<TOKEN_METADATA_VALUE>(entry->second.first) = nullptr;
			}

			TRACE_EXIT();
		}

		std::map<cc65c::core::uuid_t, std::pair<cc65c::assembler::node_metadata_t, size_t>>::iterator 
		symbol::find_node(
			__in cc65c::core::uuid_t id
			)
		{
			std::map<cc65c::core::uuid_t, std::pair<cc65c::assembler::node_metadata_t, size_t>>::iterator result;

			TRACE_ENTRY();

			result = m_node.find(id);
			if(result == m_node.end()) {
				THROW_CC65C_ASSEMBLER_SYMBOL_EXCEPTION_FORMAT(CC65C_ASSEMBLER_SYMBOL_EXCEPTION_NODE_NOT_FOUND,
					"Id=%x", id);
			}

			TRACE_EXIT();
			return result;
		}

		std::map<cc65c::core::uuid_t, std::pair<cc65c::assembler::token_metadata_t, size_t>>::iterator 
		symbol::find_token(
			__in cc65c::core::uuid_t id
			)
		{
			std::map<cc65c::core::uuid_t, std::pair<cc65c::assembler::token_metadata_t, size_t>>::iterator result;

			TRACE_ENTRY();

			result = m_token.find(id);
			if(result == m_token.end()) {
				THROW_CC65C_ASSEMBLER_SYMBOL_EXCEPTION_FORMAT(CC65C_ASSEMBLER_SYMBOL_EXCEPTION_TOKEN_NOT_FOUND,
					"Id=%x", id);
			}

			TRACE_EXIT();
			return result;
		}

		void 
		symbol::generate_node(
			__in cc65c::core::uuid_t id,
			__in cc65c::core::uuid_t token_id,
			__in cc65c::core::uuid_t node_parent,
			__in const std::vector<cc65c::core::uuid_t> &node_child,
			__inout cc65c::core::uuid_t *&reference_id,
			__inout cc65c::core::uuid_t *&reference_parent,
			__inout std::vector<cc65c::core::uuid_t> *&reference_child
			)
		{
			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_symbol_mutex);

			if(!cc65c::core::singleton<cc65c::assembler::symbol>::is_initialized()) {
				THROW_CC65C_ASSEMBLER_SYMBOL_EXCEPTION(CC65C_ASSEMBLER_SYMBOL_EXCEPTION_UNINITIALIZED);
			}

			create_node(id, token_id, node_parent, node_child, reference_id, reference_parent, reference_child);

			TRACE_EXIT();
		}

		void 
		symbol::generate_token(
			__in cc65c::core::uuid_t id,
			__in cc65c::assembler::token_t type,
			__in uint32_t subtype,
			__inout std::string *&reference_key,
			__inout int32_t *&reference_value,
			__in_opt cc65c::core::stream_t row,
			__in_opt cc65c::core::stream_t column
			)
		{
			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_symbol_mutex);

			if(!cc65c::core::singleton<cc65c::assembler::symbol>::is_initialized()) {
				THROW_CC65C_ASSEMBLER_SYMBOL_EXCEPTION(CC65C_ASSEMBLER_SYMBOL_EXCEPTION_UNINITIALIZED);
			}

			create_token(id, type, subtype, std::string(), 0, reference_key, reference_value, row, column);

			TRACE_EXIT();
		}

		void 
		symbol::generate_token(
			__in cc65c::core::uuid_t id,
			__in cc65c::assembler::token_t type,
			__in const std::string &key,
			__inout std::string *&reference_key,
			__inout int32_t *&reference_value,
			__in_opt cc65c::core::stream_t row,
			__in_opt cc65c::core::stream_t column
			)
		{
			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_symbol_mutex);

			if(!cc65c::core::singleton<cc65c::assembler::symbol>::is_initialized()) {
				THROW_CC65C_ASSEMBLER_SYMBOL_EXCEPTION(CC65C_ASSEMBLER_SYMBOL_EXCEPTION_UNINITIALIZED);
			}

			create_token(id, type, SUBTYPE_UNDEFINED, key, 0, reference_key, reference_value, row, column);

			TRACE_EXIT();
		}

		void 
		symbol::generate_token(
			__in cc65c::core::uuid_t id,
			__in int32_t value,
			__inout std::string *&reference_key,
			__inout int32_t *&reference_value,
			__in_opt cc65c::core::stream_t row,
			__in_opt cc65c::core::stream_t column
			)
		{
			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_symbol_mutex);

			if(!cc65c::core::singleton<cc65c::assembler::symbol>::is_initialized()) {
				THROW_CC65C_ASSEMBLER_SYMBOL_EXCEPTION(CC65C_ASSEMBLER_SYMBOL_EXCEPTION_UNINITIALIZED);
			}

			create_token(id, cc65c::assembler::TOKEN_SCALAR, SUBTYPE_UNDEFINED, std::string(), value, 
				reference_key, reference_value, row, column);

			TRACE_EXIT();
		}

		size_t 
		symbol::increment_node(
			__in cc65c::core::uuid_t id
			)
		{
			size_t result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_symbol_mutex);

			if(!cc65c::core::singleton<cc65c::assembler::symbol>::is_initialized()) {
				THROW_CC65C_ASSEMBLER_SYMBOL_EXCEPTION(CC65C_ASSEMBLER_SYMBOL_EXCEPTION_UNINITIALIZED);
			}

			result = ++find_node(id)->second.second;

			TRACE_EXIT_FORMAT("Result=%u", result);
			return result;
		}

		size_t 
		symbol::increment_token(
			__in cc65c::core::uuid_t id
			)
		{
			size_t result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_symbol_mutex);

			if(!cc65c::core::singleton<cc65c::assembler::symbol>::is_initialized()) {
				THROW_CC65C_ASSEMBLER_SYMBOL_EXCEPTION(CC65C_ASSEMBLER_SYMBOL_EXCEPTION_UNINITIALIZED);
			}

			result = ++find_token(id)->second.second;

			TRACE_EXIT_FORMAT("Result=%u", result);
			return result;
		}

		void 
		symbol::initialize(void)
		{
			TRACE_ENTRY();

			if(cc65c::core::singleton<cc65c::assembler::symbol>::is_initialized()) {
				THROW_CC65C_ASSEMBLER_SYMBOL_EXCEPTION(CC65C_ASSEMBLER_SYMBOL_EXCEPTION_INITIALIZED);
			}

			cc65c::core::singleton<cc65c::assembler::symbol>::set_initialized(true);
			TRACE_INFORMATION("Symbol manager initialized");

			TRACE_EXIT();
		}

		cc65c::assembler::node_metadata_t &
		symbol::node_metadata(
			__in cc65c::core::uuid_t id
			)
		{
			TRACE_ENTRY();

			cc65c::assembler::node_metadata_t &result = find_node(id)->second.first;

			TRACE_EXIT();
			return result;
		}

		size_t 
		symbol::node_references(
			__in cc65c::core::uuid_t id
			)
		{
			size_t result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_symbol_mutex);

			if(!cc65c::core::singleton<cc65c::assembler::symbol>::is_initialized()) {
				THROW_CC65C_ASSEMBLER_SYMBOL_EXCEPTION(CC65C_ASSEMBLER_SYMBOL_EXCEPTION_UNINITIALIZED);
			}

			result = find_node(id)->second.second;

			TRACE_EXIT_FORMAT("Result=%u", result);
			return result;
		}

		size_t 
		symbol::node_count(void)
		{
			size_t result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_symbol_mutex);

			if(!cc65c::core::singleton<cc65c::assembler::symbol>::is_initialized()) {
				THROW_CC65C_ASSEMBLER_SYMBOL_EXCEPTION(CC65C_ASSEMBLER_SYMBOL_EXCEPTION_UNINITIALIZED);
			}

			result = m_node.size();

			TRACE_EXIT_FORMAT("Result=%u", result);
			return result;
		}

		std::string 
		symbol::to_string(
			__in_opt bool verbose
			)
		{
			std::string result;

			TRACE_ENTRY();

			result = cc65c::assembler::symbol::as_string(*this, verbose);

			TRACE_EXIT();
			return result;
		}

		cc65c::assembler::token_metadata_t &
		symbol::token_metadata(
			__in cc65c::core::uuid_t id
			)
		{
			TRACE_ENTRY();

			cc65c::assembler::token_metadata_t &result = find_token(id)->second.first;

			TRACE_EXIT();
			return result;
		}

		size_t 
		symbol::token_references(
			__in cc65c::core::uuid_t id
			)
		{
			size_t result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_symbol_mutex);

			if(!cc65c::core::singleton<cc65c::assembler::symbol>::is_initialized()) {
				THROW_CC65C_ASSEMBLER_SYMBOL_EXCEPTION(CC65C_ASSEMBLER_SYMBOL_EXCEPTION_UNINITIALIZED);
			}

			result = find_token(id)->second.second;

			TRACE_EXIT_FORMAT("Result=%u", result);
			return result;
		}

		size_t 
		symbol::token_count(void)
		{
			size_t result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_symbol_mutex);

			if(!cc65c::core::singleton<cc65c::assembler::symbol>::is_initialized()) {
				THROW_CC65C_ASSEMBLER_SYMBOL_EXCEPTION(CC65C_ASSEMBLER_SYMBOL_EXCEPTION_UNINITIALIZED);
			}

			result = m_token.size();

			TRACE_EXIT_FORMAT("Result=%u", result);
			return result;
		}

		void 
		symbol::uninitialize(void)
		{
			TRACE_ENTRY();

			if(cc65c::core::singleton<cc65c::assembler::symbol>::is_initialized()) {
				cc65c::core::singleton<cc65c::assembler::symbol>::set_initialized(false);
				clear();
				TRACE_INFORMATION("Symbol manager uninitialized");
			}

			TRACE_EXIT();
		}
	}
}
