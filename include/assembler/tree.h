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

#ifndef CC65C_ASSEMBLER_TREE_H_
#define CC65C_ASSEMBLER_TREE_H_

#include "node.h"

namespace cc65c {

	namespace assembler {

		typedef enum {
			TREE_BEGIN = 0,
			TREE_END,
			TREE_STATEMENT,
		} tree_t;

		#define TREE_MAX TREE_STATEMENT

		class tree :
				public cc65c::core::object {

			public:

				tree(
					__in cc65c::assembler::tree_t type
					);

				tree(
					__in cc65c::assembler::tree_t type,
					__in cc65c::assembler::node &child
					);

				tree(
					__in const tree &other
					);

				virtual ~tree(void);

				tree &operator=(
					__in const tree &other
					);

				bool operator==(
					__in const tree &other
					);

				bool operator!=(
					__in const tree &other
					);

				size_t add(
					__in cc65c::assembler::token &child
					);

				size_t add(
					__in cc65c::assembler::node &child
					);

				static std::string as_string(
					__in const tree &reference,
					__in_opt bool verbose = false
					);

				static cc65c::assembler::token as_token(
					__in const tree &reference
					);

				static cc65c::assembler::tree_t as_type(
					__in const tree &reference
					);

				void clear(void);

				cc65c::assembler::node &move_child(
					__in cc65c::core::uuid_t id
					);

				cc65c::assembler::node &move_child_index(
					__in size_t index
					);

				cc65c::assembler::node &move_parent(void);

				cc65c::assembler::node &move_root(void);

				cc65c::assembler::node &node(void);

				size_t size(void);

				virtual std::string to_string(
					__in_opt bool verbose = false
					);

				cc65c::assembler::token token(void);

				cc65c::assembler::tree_t type(void);

			protected:

				std::map<cc65c::core::uuid_t, cc65c::assembler::node>::iterator find(
					__in cc65c::core::uuid_t id
					);

				std::map<cc65c::core::uuid_t, cc65c::assembler::node> m_node;

				cc65c::core::uuid_t m_node_current;

				cc65c::core::uuid_t m_node_root;

				std::mutex m_tree_mutex;
		};
	};
}

#endif // CC65C_ASSEMBLER_TREE_H_
