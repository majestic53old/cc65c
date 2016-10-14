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

#ifndef CC65C_ASSEMBLER_SYMBOL_H_
#define CC65C_ASSEMBLER_SYMBOL_H_

#include "node.h"
#include "../core/singleton.h"

namespace cc65c {

	namespace assembler {

		class symbol :
				public cc65c::core::singleton<cc65c::assembler::symbol> {

			public:

				~symbol(void);

				bool contains_node(
					__in cc65c::core::uuid_t id
					);

				bool contains_token(
					__in cc65c::core::uuid_t id
					);

				size_t decrement_node(
					__in cc65c::core::uuid_t id
					);

				size_t decrement_token(
					__in cc65c::core::uuid_t id
					);

				void generate_node(
					__in cc65c::core::uuid_t id,
					__in cc65c::core::uuid_t token_id,
					__in cc65c::core::uuid_t node_parent,
					__in const std::vector<cc65c::core::uuid_t> &node_child,
					__inout cc65c::core::uuid_t *&reference_id,
					__inout cc65c::core::uuid_t *&reference_parent,
					__inout std::vector<cc65c::core::uuid_t> *&reference_child
					);

				void generate_token(
					__in cc65c::core::uuid_t id,
					__in cc65c::assembler::token_t type,
					__in uint32_t subtype,
					__inout std::string *&reference_key,
					__inout int32_t *&reference_value,
					__in_opt cc65c::core::stream_t row = 0,
					__in_opt cc65c::core::stream_t column = 0
					);

				void generate_token(
					__in cc65c::core::uuid_t id,
					__in cc65c::assembler::token_t type,
					__in const std::string &key,
					__inout std::string *&reference_key,
					__inout int32_t *&reference_value,
					__in_opt cc65c::core::stream_t row = 0,
					__in_opt cc65c::core::stream_t column = 0
					);

				void generate_token(
					__in cc65c::core::uuid_t id,
					__in int32_t value,
					__inout std::string *&reference_key,
					__inout int32_t *&reference_value,
					__in_opt cc65c::core::stream_t row = 0,
					__in_opt cc65c::core::stream_t column = 0
					);

				size_t increment_node(
					__in cc65c::core::uuid_t id
					);

				size_t increment_token(
					__in cc65c::core::uuid_t id
					);

				void initialize(void);

				cc65c::assembler::node_metadata_t &node_metadata(
					__in cc65c::core::uuid_t id
					);

				size_t node_references(
					__in cc65c::core::uuid_t id
					);

				size_t node_count(void);

				std::string to_string(
					__in_opt bool verbose = false
					);

				cc65c::assembler::token_metadata_t &token_metadata(
					__in cc65c::core::uuid_t id
					);

				size_t token_references(
					__in cc65c::core::uuid_t id
					);

				size_t token_count(void);

				void uninitialize(void);

			protected:

				friend class cc65c::core::singleton<cc65c::assembler::symbol>;

				symbol(void);

				symbol(
					__in const symbol &other
					) = delete;

				symbol &operator=(
					__in const symbol &other
					) = delete;

				static std::string as_string(
					__in const symbol &reference,
					__in_opt bool verbose = false
					);

				void clear(void);

				void create_node(
					__in cc65c::core::uuid_t id,
					__in cc65c::core::uuid_t token_id,
					__in cc65c::core::uuid_t node_parent,
					__in const std::vector<cc65c::core::uuid_t> &node_child,
					__inout cc65c::core::uuid_t *&reference_id,
					__inout cc65c::core::uuid_t *&reference_parent,
					__inout std::vector<cc65c::core::uuid_t> *&reference_child
					);

				void create_token(
					__in cc65c::core::uuid_t id,
					__in cc65c::assembler::token_t type,
					__in uint32_t subtype,
					__in const std::string &key,
					__in int32_t value,
					__inout std::string *&reference_key,
					__inout int32_t *&reference_value,
					__in_opt cc65c::core::stream_t row = 0,
					__in_opt cc65c::core::stream_t column = 0
					);

				void destroy_node(
					__in std::map<cc65c::core::uuid_t, std::pair<cc65c::assembler::node_metadata_t, size_t>>::iterator 
						&entry
						);

				void destroy_token(
					__in std::map<cc65c::core::uuid_t, std::pair<cc65c::assembler::token_metadata_t, size_t>>::iterator 
						&entry
						);

				std::map<cc65c::core::uuid_t, std::pair<cc65c::assembler::node_metadata_t, size_t>>::iterator find_node(
					__in cc65c::core::uuid_t id
					);

				std::map<cc65c::core::uuid_t, std::pair<cc65c::assembler::token_metadata_t, size_t>>::iterator find_token(
					__in cc65c::core::uuid_t id
					);

				std::map<cc65c::core::uuid_t, std::pair<cc65c::assembler::node_metadata_t, size_t>> m_node;

				std::mutex m_symbol_mutex;

				std::map<cc65c::core::uuid_t, std::pair<cc65c::assembler::token_metadata_t, size_t>> m_token;
		};
	}
}

#endif // CC65C_ASSEMBLER_SYMBOL_H_
