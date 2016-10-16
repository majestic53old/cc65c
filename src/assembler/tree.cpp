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

#include "../../include/assembler/tree.h"
#include "tree_type.h"
#include "../../include/core/trace.h"

namespace cc65c {

	namespace assembler {

		static const std::string TREE_STR[] = {
			"BEGIN", "END", "STATEMENT",
			};

		#define TREE_STRING(_TYPE_) \
			((_TYPE_) > TREE_MAX ? STRING_UNKNOWN : STRING_CHECK(TREE_STR[_TYPE_]))

		tree::tree(
			__in_opt cc65c::assembler::tree_t type
			) :
				cc65c::core::object(cc65c::core::OBJECT_TREE, type),
				m_node_current(NODE_UNDEFINED),
				m_node_root(NODE_UNDEFINED)
		{
			TRACE_ENTRY();

			if(type > TREE_MAX) {
				THROW_CC65C_ASSEMBLER_TREE_EXCEPTION_FORMAT(CC65C_ASSEMBLER_TREE_EXCEPTION_INVALID_INVALID_TYPE,
					"Type=%x", type);
			}

			TRACE_EXIT();
		}

		tree::tree(
			__in cc65c::assembler::tree_t type,
			__in cc65c::assembler::node &child
			) :
				cc65c::core::object(cc65c::core::OBJECT_TREE, type),
				m_node_current(NODE_UNDEFINED),
				m_node_root(NODE_UNDEFINED)
		{
			TRACE_ENTRY();

			if(type > TREE_MAX) {
				THROW_CC65C_ASSEMBLER_TREE_EXCEPTION_FORMAT(CC65C_ASSEMBLER_TREE_EXCEPTION_INVALID_INVALID_TYPE,
					"Type=%x", type);
			}

			add(child);

			TRACE_EXIT();
		}

		tree::tree(
			__in const tree &other
			) :
				cc65c::core::object(other),
				m_node(other.m_node),
				m_node_current(other.m_node_current),
				m_node_root(other.m_node_root)
		{
			TRACE_ENTRY();
			TRACE_EXIT();
		}

		tree::~tree(void)
		{
			TRACE_ENTRY();
			TRACE_EXIT();
		}

		tree &
		tree::operator=(
			__in const tree &other
			)
		{
			TRACE_ENTRY();

			if(this != &other) {
				cc65c::core::object::operator=(other);
				m_node = other.m_node;
				m_node_current = other.m_node_current;
				m_node_root = other.m_node_root;
			}

			TRACE_EXIT_FORMAT("Result=%p", this);
			return *this;
		}

		bool 
		tree::operator==(
			__in const tree &other
			)
		{
			bool result;
			std::map<cc65c::core::uuid_t, cc65c::assembler::node>::iterator iter;
			std::map<cc65c::core::uuid_t, cc65c::assembler::node>::const_iterator other_iter;

			TRACE_ENTRY();

			result = ((this == &other)
				|| ((m_node.size() == other.m_node.size())
				&& (m_node_current == other.m_node_current)
				&& (m_node_root == other.m_node_root)));

			if(result) {

				for(iter = m_node.begin(); iter != m_node.end(); ++iter) {
					other_iter = other.m_node.find(iter->first);

					result = ((other_iter != other.m_node.end()) && (iter->second == other_iter->second));
					if(!result) {
						break;
					}
				}
			}

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		bool 
		tree::operator!=(
			__in const tree &other
			)
		{
			bool result;
			std::map<cc65c::core::uuid_t, cc65c::assembler::node>::iterator iter;
			std::map<cc65c::core::uuid_t, cc65c::assembler::node>::const_iterator other_iter;

			TRACE_ENTRY();

			result = ((this != &other)
				&& ((m_node.size() != other.m_node.size())
				|| (m_node_current != other.m_node_current)
				|| (m_node_root != other.m_node_root)));

			if(!result) {

				for(iter = m_node.begin(); iter != m_node.end(); ++iter) {
					other_iter = other.m_node.find(iter->first);

					result = ((other_iter == other.m_node.end()) || (iter->second != other_iter->second));
					if(result) {
						break;
					}
				}
			}

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		size_t 
		tree::add(
			__in cc65c::assembler::token &child
			)
		{
			size_t result;
			cc65c::assembler::node entry(child.id());

			TRACE_ENTRY();

			result = add(entry);

			TRACE_EXIT_FORMAT("Result=%u", result);
			return result;
		}

		size_t 
		tree::add(
			__in cc65c::assembler::node &child
			)
		{
			size_t result = 0;
			std::map<cc65c::core::uuid_t, cc65c::assembler::node>::iterator child_iter, parent_iter;

			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_tree_mutex);

			if(!m_node.empty()) {
				m_node.insert(std::make_pair(child.id(), child));
				child_iter = find(child.id());
				parent_iter = find(m_node_current);
				result = parent_iter->second.size();
				parent_iter->second.add(child.id());
				child_iter->second.set_parent(parent_iter->first);
			} else {
				m_node.insert(std::make_pair(child.id(), child));
				find(child.id())->second.set_parent(NODE_UNDEFINED);
				m_node_root = child.id();
				m_node_current = m_node_root;
			}

			TRACE_EXIT_FORMAT("Result=%u", result);
			return result;
		}

		std::string 
		tree::as_string(
			__in const tree &reference,
			__in_opt bool verbose
			)
		{
			std::stringstream result;
			std::map<cc65c::core::uuid_t, cc65c::assembler::node>::const_iterator iter;

			TRACE_ENTRY();

			result << cc65c::core::object::as_string(reference, verbose) 
				<< ", Type=" << TREE_STRING((cc65c::assembler::tree_t) reference.m_subtype);

			if(((cc65c::assembler::tree_t) reference.m_subtype) > TREE_MAX) {
				result << " (" << SCALAR_AS_HEX(cc65c::assembler::tree_t, reference.m_subtype) << ")";
			}

			switch(reference.m_subtype) {
				case TREE_BEGIN:
				case TREE_END:
					break;
				default:
					result << " Size=" << reference.m_node.size()
						<< ", Root=" << SCALAR_AS_HEX(cc65c::core::uuid_t, reference.m_node_current)
						<< ", Current=" << SCALAR_AS_HEX(cc65c::core::uuid_t, reference.m_node_root);
					break;
			}

			if(reference.m_node_current != NODE_UNDEFINED) {

				iter = reference.m_node.find(reference.m_node_current);
				if(iter != reference.m_node.end()) {
					result << ", " << cc65c::assembler::node::as_string(iter->second, verbose);
				}
			}

			TRACE_EXIT();
			return result.str();
		}

		cc65c::assembler::token 
		tree::as_token(
			__in const tree &reference
			)
		{
			std::map<cc65c::core::uuid_t, cc65c::assembler::node>::const_iterator iter;

			TRACE_ENTRY();

			iter = reference.m_node.find(reference.m_node_current);
			if(iter == reference.m_node.end()) {
				THROW_CC65C_ASSEMBLER_TREE_EXCEPTION_FORMAT(CC65C_ASSEMBLER_TREE_EXCEPTION_NOT_FOUND,
					"Id=%x", reference.m_node_current);
			}

			TRACE_EXIT();
			return cc65c::assembler::node::as_token(iter->second);
		}

		cc65c::assembler::tree_t 
		tree::as_type(
			__in const tree &reference
			)
		{
			cc65c::assembler::tree_t result;

			TRACE_ENTRY();

			result = (cc65c::assembler::tree_t) reference.m_subtype;

			TRACE_EXIT_FORMAT("Result=%x(%s)", result, TREE_STRING(result));
			return result;	
		}

		void 
		tree::clear(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_tree_mutex);

			m_node.clear();
			m_node_current = NODE_UNDEFINED;
			m_node_root = NODE_UNDEFINED;

			TRACE_EXIT();
		}

		std::map<cc65c::core::uuid_t, cc65c::assembler::node>::iterator 
		tree::find(
			__in cc65c::core::uuid_t id
			)
		{
			std::map<cc65c::core::uuid_t, cc65c::assembler::node>::iterator result;

			TRACE_ENTRY();

			result = m_node.find(id);
			if(result == m_node.end()) {
				THROW_CC65C_ASSEMBLER_TREE_EXCEPTION_FORMAT(CC65C_ASSEMBLER_TREE_EXCEPTION_NOT_FOUND,
					"Id=%x", id);
			}

			TRACE_EXIT();
			return result;
		}

		bool 
		tree::match(
			__in cc65c::assembler::tree_t type
			)
		{
			bool result;

			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_tree_mutex);

			result = (((cc65c::assembler::tree_t) m_subtype) == type);

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		cc65c::assembler::node &
		tree::move_child(
			__in cc65c::core::uuid_t id
			)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_tree_mutex);

			cc65c::assembler::node &result = find(id)->second;
			m_node_current = id;

			TRACE_EXIT();
			return result;
		}

		cc65c::assembler::node &
		tree::move_child_index(
			__in size_t index
			)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_tree_mutex);

			m_node_current = find(m_node_current)->second.at(index);
			cc65c::assembler::node &result = find(m_node_current)->second;

			TRACE_EXIT();
			return result;
		}

		cc65c::assembler::node &
		tree::move_parent(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_tree_mutex);

			m_node_current = find(m_node_current)->second.parent();
			cc65c::assembler::node &result = find(m_node_current)->second;

			TRACE_EXIT();
			return result;
		}

		cc65c::assembler::node &
		tree::move_root(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_tree_mutex);

			m_node_current = m_node_root;
			cc65c::assembler::node &result = find(m_node_current)->second;

			TRACE_EXIT();
			return result;
		}

		cc65c::assembler::node &
		tree::node(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_tree_mutex);

			cc65c::assembler::node &result = find(m_node_current)->second;

			TRACE_EXIT();
			return result;
		}

		size_t 
		tree::size(void)
		{
			size_t result;

			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_tree_mutex);

			result = m_node.size();

			TRACE_EXIT_FORMAT("Result=%u", result);
			return result;
		}

		std::string 
		tree::to_string(
			__in_opt bool verbose
			)
		{
			std::string result;

			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_tree_mutex);

			result = cc65c::assembler::tree::as_string(*this, verbose);

			TRACE_EXIT();
			return result;
		}

		cc65c::assembler::token 
		tree::token(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_tree_mutex);

			TRACE_EXIT();
			return find(m_node_current)->second.token();
		}

		cc65c::assembler::tree_t 
		tree::type(void)
		{
			cc65c::assembler::tree_t result;

			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_tree_mutex);

			result = (cc65c::assembler::tree_t) m_subtype;

			TRACE_EXIT_FORMAT("Result=%x(%s)", result, TREE_STRING(result));
			return result;
		}
	}
}
