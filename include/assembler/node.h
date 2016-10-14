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

#ifndef CC65C_ASSEMBLER_NODE_H_
#define CC65C_ASSEMBLER_NODE_H_

#include <vector>
#include "token.h"

namespace cc65c {

	namespace assembler {

		#define NODE_UNDEFINED UUID_INVALID

		enum {
			NODE_METADATA_TOKEN = 0,
			NODE_METADATA_PARENT,
			NODE_METADATA_CHILD,
		};

		typedef std::tuple<cc65c::core::uuid_t *, cc65c::core::uuid_t *, std::vector<cc65c::core::uuid_t> *> node_metadata_t;

		class node :
				public cc65c::core::object {

			public:

				node(
					__in cc65c::core::uuid_t id,
					__in_opt cc65c::core::uuid_t parent = NODE_UNDEFINED,
					__in_opt const std::vector<cc65c::core::uuid_t> &child = std::vector<cc65c::core::uuid_t>()
					);

				node(
					__in cc65c::core::uuid_t id,
					__in const cc65c::assembler::node_metadata_t &metadata
					);

				node(
					__in const node &other
					);

				virtual ~node(void);

				node &operator=(
					__in const node &other
					);

				bool operator==(
					__in const node &other
					);

				bool operator!=(
					__in const node &other
					);

				void add(
					__in cc65c::core::uuid_t id
					);

				static std::string as_string(
					__in const node &reference,
					__in_opt bool verbose = false
					);

				static cc65c::assembler::token as_token(
					__in const node &reference
					);

				cc65c::core::uuid_t at(
					__in size_t index
					);

				cc65c::core::uuid_t at(
					__in size_t index,
					__inout cc65c::assembler::node_metadata_t &metadata
					);

				bool contains(
					__in cc65c::core::uuid_t id
					);

				void insert(
					__in cc65c::core::uuid_t id,
					__in size_t index
					);

				cc65c::core::uuid_t parent(void);

				cc65c::core::uuid_t parent(
					__inout cc65c::assembler::node_metadata_t &metadata
					);

				void remove(
					__in cc65c::core::uuid_t id
					);

				void remove_all(void);

				void set_parent(
					__in cc65c::core::uuid_t id
					);

				size_t size(void);

				virtual std::string to_string(
					__in_opt bool verbose = false
					);

				cc65c::assembler::token token(void);

			protected:				

				static void as_string(
					__in const node &reference,
					__in size_t tabs,
					__inout std::stringstream &stream,
					__in_opt bool verbose = false
					);

				void decrement(void);

				void generate(
					__in cc65c::core::uuid_t id,
					__in_opt cc65c::core::uuid_t parent = NODE_UNDEFINED,
					__in_opt const std::vector<cc65c::core::uuid_t> &child = std::vector<cc65c::core::uuid_t>()
					);

				void increment(void);

				std::vector<cc65c::core::uuid_t> *m_node_child;

				cc65c::core::uuid_t *m_node_parent;

				cc65c::core::uuid_t *m_token_id;

				std::mutex m_node_mutex;
		};
	}
}

#endif // CC65C_ASSEMBLER_NODE_H_
