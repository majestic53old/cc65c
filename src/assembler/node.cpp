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

#include "../../include/assembler/node.h"
#include "node_type.h"
#include "../../include/core/trace.h"
#include "../../include/assembler/symbol.h"

namespace cc65c {

	namespace assembler {

		#define CHARACTER_TAB '\t'
		#define TAB_DELIM "\\---"
		#define TAB_INIT 1

		node::node(
			__in cc65c::core::uuid_t id,
			__in_opt cc65c::core::uuid_t parent,
			__in_opt const std::vector<cc65c::core::uuid_t> &child
			) :
				cc65c::core::object(cc65c::core::OBJECT_NODE),
				m_node_child(nullptr),
				m_node_parent(nullptr),
				m_token_id(nullptr)
		{
			TRACE_ENTRY();

			generate(id, parent, child);

			TRACE_EXIT();
		}

		node::node(
			__in cc65c::core::uuid_t id,
			__in const cc65c::assembler::node_metadata_t &metadata
			) :
				cc65c::core::object(id, cc65c::core::OBJECT_NODE),
				m_node_child(std::get<NODE_METADATA_CHILD>(metadata)),
				m_node_parent(std::get<NODE_METADATA_PARENT>(metadata)),
				m_token_id(std::get<NODE_METADATA_TOKEN>(metadata))
		{
			TRACE_ENTRY();

			increment();

			TRACE_EXIT();	
		}

		node::node(
			__in const node &other
			) :
				cc65c::core::object(other),
				m_node_child(other.m_node_child),
				m_node_parent(other.m_node_parent),
				m_token_id(other.m_token_id)
		{
			TRACE_ENTRY();

			increment();

			TRACE_EXIT();
		}

		node::~node(void)
		{
			TRACE_ENTRY();

			decrement();

			TRACE_EXIT();
		}

		node &
		node::operator=(
			__in const node &other
			)
		{
			TRACE_ENTRY();

			if(this != &other) {
				decrement();
				cc65c::core::object::operator=(other);
				m_node_child = other.m_node_child;
				m_node_parent = other.m_node_parent;
				m_token_id = other.m_token_id;
				increment();
			}

			TRACE_EXIT_FORMAT("Result=%p", this);
			return *this;
		}

		bool 
		node::operator==(
			__in const node &other
			)
		{
			bool result;

			TRACE_ENTRY();

			if(!m_token_id || !m_node_child || !m_node_parent) {
				THROW_CC65C_ASSEMBLER_NODE_EXCEPTION_FORMAT(CC65C_ASSEMBLER_NODE_EXCEPTION_INVALID_ALLOCATION,
					"Id=%p, Child=%p, Parent=%p", m_token_id, m_node_child, m_node_parent);
			}

			result = ((this == &other)
				|| (cc65c::core::object::operator==(other)
				&& (m_node_child == other.m_node_child)
				&& (m_node_parent == other.m_node_parent)
				&& (m_token_id == other.m_token_id)));

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		bool 
		node::operator!=(
			__in const node &other
			)
		{
			bool result;

			TRACE_ENTRY();

			if(!m_token_id || !m_node_child || !m_node_parent) {
				THROW_CC65C_ASSEMBLER_NODE_EXCEPTION_FORMAT(CC65C_ASSEMBLER_NODE_EXCEPTION_INVALID_ALLOCATION,
					"Id=%p, Child=%p, Parent=%p", m_token_id, m_node_child, m_node_parent);
			}

			result = ((this != &other)
				&& (cc65c::core::object::operator!=(other)
				|| (m_node_child != other.m_node_child)
				|| (m_node_parent != other.m_node_parent)
				|| (m_token_id != other.m_token_id)));

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		void 
		node::add(
			__in cc65c::core::uuid_t id
			)
		{
			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_node_mutex);

			if(!m_node_child) {
				THROW_CC65C_ASSEMBLER_NODE_EXCEPTION_FORMAT(CC65C_ASSEMBLER_NODE_EXCEPTION_INVALID_CHILD_ID,
					"Child=%p", m_node_child);
			}

			cc65c::assembler::symbol &instance = cc65c::assembler::symbol::acquire();
			if(instance.is_initialized() && instance.contains_node(id)) {
				instance.increment_node(id);
			}

			m_node_child->push_back(id);

			TRACE_EXIT();
		}

		std::string 
		node::as_string(
			__in const node &reference,
			__in_opt bool verbose
			)
		{
			std::stringstream result;

			TRACE_ENTRY();

			result << cc65c::core::object::as_string(reference, verbose);
			as_string(reference, TAB_INIT, result, verbose);

			TRACE_EXIT();
			return result.str();
		}

		void 
		node::as_string(
			__in const node &reference,
			__in size_t tabs,
			__inout std::stringstream &stream,
			__in_opt bool verbose
			)
		{
			size_t tab_iter = 0;
			std::vector<cc65c::core::uuid_t>::const_iterator iter;

			TRACE_ENTRY();

			if(!reference.m_token_id || !reference.m_node_child || !reference.m_node_parent) {
				THROW_CC65C_ASSEMBLER_NODE_EXCEPTION_FORMAT(CC65C_ASSEMBLER_NODE_EXCEPTION_INVALID_ALLOCATION,
					"Id=%p, Child=%p, Parent=%p", reference.m_token_id, reference.m_node_child, reference.m_node_parent);
			}

			stream << std::endl;

			for(; tab_iter < tabs; ++tab_iter) {
				stream << CHARACTER_TAB;
			}

			stream << TAB_DELIM << " " << cc65c::assembler::node::as_token(reference).to_string(verbose)
				<< ", Size=" << reference.m_node_child->size() 
				<< ", Parent=" << SCALAR_AS_HEX(cc65c::core::uuid_t, *reference.m_node_parent);

			cc65c::assembler::symbol &instance = cc65c::assembler::symbol::acquire();
			if(instance.is_initialized()) {

				for(iter = reference.m_node_child->begin(); iter != reference.m_node_child->end(); 
						++iter) {

					if(instance.contains_node(*iter)) {
						as_string(node(*iter, instance.node_metadata(*iter)), tabs + 1, stream, verbose);
					}
				}
			}

			TRACE_EXIT();
		}

		cc65c::assembler::token 
		node::as_token(
			__in const node &reference
			)
		{
			cc65c::core::uuid_t id;
			cc65c::assembler::token result;

			TRACE_ENTRY();

			if(!reference.m_token_id) {
				THROW_CC65C_ASSEMBLER_NODE_EXCEPTION_FORMAT(CC65C_ASSEMBLER_NODE_EXCEPTION_INVALID_ID,
					"Id=%p", reference.m_token_id);
			}

			id = *reference.m_token_id;

			cc65c::assembler::symbol &instance = cc65c::assembler::symbol::acquire();
			if(instance.is_initialized() && instance.contains_token(id)) {
				result = cc65c::assembler::token(id, instance.token_metadata(id));
			}

			TRACE_EXIT();
			return result;
		}

		cc65c::core::uuid_t 
		node::at(
			__in size_t index
			)
		{
			cc65c::core::uuid_t result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_node_mutex);

			if(!m_node_child) {
				THROW_CC65C_ASSEMBLER_NODE_EXCEPTION_FORMAT(CC65C_ASSEMBLER_NODE_EXCEPTION_INVALID_CHILD_ID,
					"Child=%p", m_node_child);
			}

			if(index >= m_node_child->size()) {
				THROW_CC65C_ASSEMBLER_NODE_EXCEPTION_FORMAT(CC65C_ASSEMBLER_NODE_EXCEPTION_INVALID_CHILD_ID_INDEX,
					"Index=%u", index);
			}

			result = m_node_child->at(index);

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		cc65c::core::uuid_t 
		node::at(
			__in size_t index,
			__inout cc65c::assembler::node_metadata_t &metadata
			)
		{
			cc65c::core::uuid_t result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_node_mutex);

			if(!m_node_child) {
				THROW_CC65C_ASSEMBLER_NODE_EXCEPTION_FORMAT(CC65C_ASSEMBLER_NODE_EXCEPTION_INVALID_CHILD_ID,
					"Child=%p", m_node_child);
			}

			if(index >= m_node_child->size()) {
				THROW_CC65C_ASSEMBLER_NODE_EXCEPTION_FORMAT(CC65C_ASSEMBLER_NODE_EXCEPTION_INVALID_CHILD_ID_INDEX,
					"Index=%u", index);
			}

			result = m_node_child->at(index);

			cc65c::assembler::symbol &instance = cc65c::assembler::symbol::acquire();
			if(instance.is_initialized() && instance.contains_node(result)) {
				metadata = instance.node_metadata(result);
			}

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		bool 
		node::contains(
			__in cc65c::core::uuid_t id
			)
		{
			bool result = false;
			std::vector<cc65c::core::uuid_t>::iterator iter;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_node_mutex);

			if(!m_node_child) {
				THROW_CC65C_ASSEMBLER_NODE_EXCEPTION_FORMAT(CC65C_ASSEMBLER_NODE_EXCEPTION_INVALID_CHILD_ID,
					"Child=%p", m_node_child);
			}

			for(iter = m_node_child->begin(); iter != m_node_child->end(); ++iter) {

				if(*iter == id) {
					result = true;
					break;
				}
			}

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		void 
		node::decrement(void)
		{
			TRACE_ENTRY();

			cc65c::assembler::symbol &instance = cc65c::assembler::symbol::acquire();

			if(instance.is_initialized()) {

				if(m_token_id && instance.contains_token(*m_token_id)) {
					instance.decrement_token(*m_token_id);
				}

				if(m_node_parent && instance.contains_node(*m_node_parent)) {
					instance.decrement_node(*m_node_parent);
				}

				if(instance.contains_node(m_id)) {
					instance.decrement_node(m_id);
				}
			}

			TRACE_EXIT();
		}

		void 
		node::generate(
			__in cc65c::core::uuid_t id,
			__in_opt cc65c::core::uuid_t parent,
			__in_opt const std::vector<cc65c::core::uuid_t> &child
			)
		{
			std::vector<cc65c::core::uuid_t>::const_iterator iter;

			TRACE_ENTRY();

			cc65c::assembler::symbol &instance = cc65c::assembler::symbol::acquire();
			if(instance.is_initialized()) {
				instance.generate_node(m_id, id, parent, child, m_token_id, m_node_parent, m_node_child);

				if(instance.contains_token(id)) {
					instance.increment_token(id);
				}

				if(instance.contains_node(parent)) {
					instance.increment_node(parent);
				}

				for(iter = child.begin(); iter != child.end(); ++iter) {

					if(instance.contains_node(*iter)) {
						instance.increment_node(*iter);
					}
				}
			}

			TRACE_EXIT();
		}

		void 
		node::increment(void)
		{
			TRACE_ENTRY();

			cc65c::assembler::symbol &instance = cc65c::assembler::symbol::acquire();

			if(instance.is_initialized()) {

				if(m_token_id && instance.contains_token(*m_token_id)) {
					instance.increment_token(*m_token_id);
				}

				if(m_node_parent && instance.contains_node(*m_node_parent)) {
					instance.increment_node(*m_node_parent);
				}

				if(instance.contains_node(m_id)) {
					instance.increment_node(m_id);
				}
			}

			TRACE_EXIT();
		}

		void 
		node::insert(
			__in cc65c::core::uuid_t id,
			__in size_t index
			)
		{
			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_node_mutex);

			if(!m_node_child) {
				THROW_CC65C_ASSEMBLER_NODE_EXCEPTION_FORMAT(CC65C_ASSEMBLER_NODE_EXCEPTION_INVALID_CHILD_ID,
					"Child=%p", m_node_child);
			}

			if(index < m_node_child->size()) {
				m_node_child->insert(m_node_child->begin() + index, id);
			} else if(index == m_node_child->size()) {
				m_node_child->push_back(id);
			} else {
				THROW_CC65C_ASSEMBLER_NODE_EXCEPTION_FORMAT(CC65C_ASSEMBLER_NODE_EXCEPTION_INVALID_CHILD_ID_INDEX,
					"Id=%x, Index=%u", id, index);
			}

			cc65c::assembler::symbol &instance = cc65c::assembler::symbol::acquire();
			if(instance.is_initialized() && instance.contains_node(id)) {
				instance.increment_node(id);
			}

			TRACE_EXIT();
		}

		cc65c::core::uuid_t 
		node::parent(void)
		{
			cc65c::core::uuid_t result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_node_mutex);

			if(!m_node_parent) {
				THROW_CC65C_ASSEMBLER_NODE_EXCEPTION_FORMAT(CC65C_ASSEMBLER_NODE_EXCEPTION_INVALID_PARENT_ID,
					"Parent=%p", m_node_parent);
			}

			result = *m_node_parent;
			if(result == NODE_UNDEFINED) {
				result = m_id;
			}

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		cc65c::core::uuid_t 
		node::parent(
			__inout cc65c::assembler::node_metadata_t &metadata
			)
		{
			cc65c::core::uuid_t result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_node_mutex);

			if(!m_node_parent) {
				THROW_CC65C_ASSEMBLER_NODE_EXCEPTION_FORMAT(CC65C_ASSEMBLER_NODE_EXCEPTION_INVALID_PARENT_ID,
					"Parent=%p", m_node_parent);
			}

			result = *m_node_parent;
			if(result == NODE_UNDEFINED) {
				result = m_id;
			}

			cc65c::assembler::symbol &instance = cc65c::assembler::symbol::acquire();
			if(instance.is_initialized() && instance.contains_node(result)) {
				metadata = instance.node_metadata(result);
			}

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		void 
		node::remove(
			__in cc65c::core::uuid_t id
			)
		{
			std::vector<cc65c::core::uuid_t>::iterator iter;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_node_mutex);

			if(!m_node_child) {
				THROW_CC65C_ASSEMBLER_NODE_EXCEPTION_FORMAT(CC65C_ASSEMBLER_NODE_EXCEPTION_INVALID_CHILD_ID,
					"Child=%p", m_node_child);
			}

			for(iter = m_node_child->begin(); iter != m_node_child->end(); ++iter) {

				if(*iter == id) {

					cc65c::assembler::symbol &instance = cc65c::assembler::symbol::acquire();
					if(instance.is_initialized() && instance.contains_node(*iter)) {
						instance.decrement_node(*iter);
					}

					break;
				}
			}

			if(iter == m_node_child->end()) {
				THROW_CC65C_ASSEMBLER_NODE_EXCEPTION_FORMAT(CC65C_ASSEMBLER_NODE_EXCEPTION_NOT_FOUND,
					"Id=%x", id);
			}

			m_node_child->erase(iter);

			TRACE_EXIT();
		}

		void 
		node::remove_all(void)
		{
			size_t result = 0;
			std::vector<cc65c::core::uuid_t>::iterator iter;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_node_mutex);

			if(!m_node_child) {
				THROW_CC65C_ASSEMBLER_NODE_EXCEPTION_FORMAT(CC65C_ASSEMBLER_NODE_EXCEPTION_INVALID_CHILD_ID,
					"Child=%p", m_node_child);
			}

			cc65c::assembler::symbol &instance = cc65c::assembler::symbol::acquire();
			if(instance.is_initialized()) {

				for(iter = m_node_child->begin(); iter != m_node_child->end(); ++iter) {

					if(instance.contains_node(*iter)) {
						result = instance.decrement_node(*iter);
					}
				}
			}

			if(result) {
				m_node_child->clear();
			}

			TRACE_EXIT();
		}

		void 
		node::set_parent(
			__in cc65c::core::uuid_t id
			)
		{
			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_node_mutex);

			if(!m_node_parent) {
				THROW_CC65C_ASSEMBLER_NODE_EXCEPTION_FORMAT(CC65C_ASSEMBLER_NODE_EXCEPTION_INVALID_PARENT_ID,
					"Parent=%p", m_node_parent);
			}

			cc65c::assembler::symbol &instance = cc65c::assembler::symbol::acquire();
			if(instance.is_initialized()) {

				if(instance.contains_node(*m_node_parent)) {
					instance.decrement_node(*m_node_parent);
				}

				*m_node_parent = id;

				if(instance.contains_node(*m_node_parent)) {
					instance.increment_node(*m_node_parent);
				}
			}

			TRACE_EXIT();
		}

		size_t 
		node::size(void)
		{
			size_t result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_node_mutex);

			if(!m_node_child) {
				THROW_CC65C_ASSEMBLER_NODE_EXCEPTION_FORMAT(CC65C_ASSEMBLER_NODE_EXCEPTION_INVALID_CHILD_ID,
					"Child=%p", m_node_child);
			}

			result = m_node_child->size();

			TRACE_EXIT_FORMAT("Result=%u", result);
			return result;
		}

		std::string 
		node::to_string(
			__in_opt bool verbose
			)
		{
			std::string result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_node_mutex);

			result = cc65c::assembler::node::as_string(*this, verbose);

			TRACE_EXIT();
			return result;
		}

		cc65c::assembler::token 
		node::token(void)
		{
			cc65c::assembler::token result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_node_mutex);

			result = cc65c::assembler::node::as_token(*this);

			TRACE_EXIT();
			return result;
		}
	}
}
