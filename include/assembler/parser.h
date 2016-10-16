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

#ifndef CC65C_ASSEMBLER_PARSER_H_
#define CC65C_ASSEMBLER_PARSER_H_

#include "lexer.h"
#include "tree.h"

namespace cc65c {

	namespace assembler {

		class parser :
				protected cc65c::assembler::lexer {

			public:

				parser(void);

				parser(
					__in const std::string &input,
					__in_opt bool is_file = true
					);

				parser(
					__in const parser &other
					);

				virtual ~parser(void);

				parser &operator=(
					__in const parser &other
					);

				virtual std::string as_exception(
					__in_opt bool verbose = false,
					__in_opt size_t tabs = 0
					);

				static std::string as_string(
					__in const parser &reference,
					__in_opt bool verbose = false
					);

				virtual void clear(void);

				virtual size_t enumerate(void);

				bool has_next(void);

				bool has_previous(void);

				cc65c::assembler::tree &move_next(void);

				cc65c::assembler::tree &move_next(
					__in cc65c::assembler::tree_t &type
					);

				cc65c::assembler::tree &move_previous(void);

				cc65c::assembler::tree &move_previous(
					__in cc65c::assembler::tree_t &type
					);

				virtual void reset(void);

				virtual void set(
					__in const std::string &input,
					__in_opt bool is_file = true
					);

				size_t size(void);

				virtual std::string to_string(
					__in_opt bool verbose = false,
					__in_opt cc65c::core::stream_t index = 0,
					__in_opt cc65c::core::stream_t offset = 0
					);

				cc65c::assembler::tree &tree(void);

				std::vector<cc65c::assembler::tree>::const_iterator tree_begin(void);

				std::vector<cc65c::assembler::tree>::const_iterator tree_end(void);

			protected:

				cc65c::assembler::tree &add_tree(
					__in cc65c::assembler::tree_t type
					);

				void add_tree(
					__in cc65c::assembler::tree &tree
					);

				void enumerate_tree(void);

				void enumerate_tree_command(
					__inout cc65c::assembler::tree &tree
					);

				void enumerate_tree_command_accumulator(
					__inout cc65c::assembler::tree &tree,
					__in uint32_t command
					);

				void enumerate_tree_command_immediate(
					__inout cc65c::assembler::tree &tree,
					__in uint32_t command
					);

				void enumerate_tree_command_implied(
					__inout cc65c::assembler::tree &tree,
					__in uint32_t command
					);

				void enumerate_tree_command_index(
					__inout cc65c::assembler::tree &tree,
					__in uint32_t command
					);

				void enumerate_tree_command_indirect(
					__inout cc65c::assembler::tree &tree,
					__in uint32_t command
					);

				void enumerate_tree_condition(
					__inout cc65c::assembler::tree &tree
					);

				void enumerate_tree_condition_expression(
					__inout cc65c::assembler::tree &tree
					);

				void enumerate_tree_condition_if(
					__inout cc65c::assembler::tree &tree
					);

				void enumerate_tree_condition_ifdef(
					__inout cc65c::assembler::tree &tree
					);

				void enumerate_tree_define(
					__inout cc65c::assembler::tree &tree
					);

				void enumerate_tree_expression(
					__inout cc65c::assembler::tree &tree
					);

				void enumerate_tree_expression_compound(
					__inout cc65c::assembler::tree &tree
					);

				void enumerate_tree_expression_factor(
					__inout cc65c::assembler::tree &tree
					);

				void enumerate_tree_expression_list(
					__inout cc65c::assembler::tree &tree
					);

				void enumerate_tree_expression_literal(
					__inout cc65c::assembler::tree &tree
					);

				void enumerate_tree_expression_term_0(
					__inout cc65c::assembler::tree &tree
					);

				void enumerate_tree_expression_term_1(
					__inout cc65c::assembler::tree &tree
					);

				void enumerate_tree_expression_term_2(
					__inout cc65c::assembler::tree &tree
					);

				void enumerate_tree_expression_term_3(
					__inout cc65c::assembler::tree &tree
					);

				void enumerate_tree_expression_term_4(
					__inout cc65c::assembler::tree &tree
					);

				void enumerate_tree_include(
					__inout cc65c::assembler::tree &tree
					);

				void enumerate_tree_label(
					__inout cc65c::assembler::tree &tree
					);

				void enumerate_tree_macro(
					__inout cc65c::assembler::tree &tree
					);

				void enumerate_tree_statement(
					__inout cc65c::assembler::tree &tree
					);

				void enumerate_tree_statement_list(
					__inout cc65c::assembler::tree &tree
					);

				std::recursive_mutex m_parser_mutex;

				std::vector<cc65c::assembler::tree> m_tree;

				size_t m_tree_position;
		};
	}
}

#endif // CC65C_ASSEMBLER_PARSER_H_
