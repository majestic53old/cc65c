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

#include "../../include/assembler/parser.h"
#include "parser_type.h"
#include "../../include/core/trace.h"

namespace cc65c {

	namespace assembler {

		#define EXCEPTION_TAB_COUNT 1

		#define TREE_SENTINEL_COUNT 2

		#define THROW_LEXER_EXCEPTION(_EXCEPT_, _VERB_) \
			THROW_CC65C_ASSEMBLER_PARSER_EXCEPTION_FORMAT(_EXCEPT_, "\n%s", STRING_CHECK( \
				cc65c::assembler::lexer::as_exception(_VERB_, EXCEPTION_TAB_COUNT)))

		#define THROW_LEXER_EXCEPTION_FORMAT(_EXCEPT_, _ROW_, _COLUMN_, _VERB_) \
			THROW_CC65C_ASSEMBLER_PARSER_EXCEPTION_FORMAT(_EXCEPT_, "\n%s", STRING_CHECK( \
				cc65c::assembler::lexer::as_exception(_ROW_, _COLUMN_, _VERB_, EXCEPTION_TAB_COUNT)))

		parser::parser(void) :
			m_tree_position(0)
		{
			TRACE_ENTRY();

			clear();

			TRACE_EXIT();
		}

		parser::parser(
			__in const std::string &input,
			__in_opt bool is_file
			) :
				cc65c::assembler::lexer(input, is_file),
				m_tree_position(0)
		{
			TRACE_ENTRY();

			clear();

			TRACE_EXIT();
		}

		parser::parser(
			__in const parser &other
			) :
				cc65c::assembler::lexer(other),
				m_tree(other.m_tree),
				m_tree_position(other.m_tree_position)
		{
			TRACE_ENTRY();
			TRACE_EXIT();
		}

		parser::~parser(void)
		{
			TRACE_ENTRY();
			TRACE_EXIT();
		}

		parser &
		parser::operator=(
			__in const parser &other
			)
		{
			TRACE_ENTRY();

			if(this != &other) {
				cc65c::assembler::lexer::operator=(other);
				m_tree = other.m_tree;
				m_tree_position = other.m_tree_position;
			}

			TRACE_EXIT_FORMAT("Result=%p", this);
			return *this;
		}

		cc65c::assembler::tree &
		parser::add_tree(
			__in cc65c::assembler::tree_t type
			)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_parser_mutex);

			m_tree.insert(m_tree.begin() + m_tree_position + 1, cc65c::assembler::tree(type));
			cc65c::assembler::tree &result = m_tree.at(m_tree_position + 1);

			TRACE_EXIT();
			return result;
		}

		void 
		parser::add_tree(
			__in cc65c::assembler::tree &tree
			)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_parser_mutex);

			m_tree.insert(m_tree.begin() + m_tree_position + 1, tree);

			TRACE_EXIT();
		}

		std::string 
		parser::as_exception(
			__in_opt bool verbose,
			__in_opt size_t tabs
			)
		{
			std::string result;

			TRACE_ENTRY();

			result = cc65c::assembler::lexer::as_exception(verbose, tabs);

			TRACE_EXIT();
			return result;
		}

		std::string 
		parser::as_string(
			__in const parser &reference,
			__in_opt bool verbose
			)
		{
			std::stringstream result;

			TRACE_ENTRY();

			result << cc65c::assembler::tree::as_string(reference.m_tree.at(reference.m_tree_position), verbose);

			switch(cc65c::assembler::tree::as_type(reference.m_tree.at(reference.m_tree_position))) {
				case TOKEN_BEGIN:
				case TOKEN_END:
					break;
				default:
					result << ", Tree=" << reference.m_tree_position 
						<< "/" << (reference.m_tree.size() - TREE_SENTINEL_COUNT);
					break;
			}

			TRACE_EXIT();
			return result.str();
		}

		void 
		parser::clear(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_parser_mutex);

			cc65c::assembler::lexer::reset();
			m_tree.clear();
			m_tree_position = 0;
			m_tree.push_back(cc65c::assembler::tree(TREE_BEGIN));
			m_tree.push_back(cc65c::assembler::tree(TREE_END));

			TRACE_EXIT();
		}

		size_t 
		parser::enumerate(void)
		{
			size_t result;

			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_parser_mutex);

			while(has_next()) {
				move_next();
			}

			reset();
			result = m_tree.size();

			TRACE_EXIT_FORMAT("Result=%u", result);
			return result;
		}

		void 
		parser::enumerate_tree(void)
		{
			cc65c::assembler::tree statement(TREE_STATEMENT);

			TRACE_ENTRY();

			enumerate_tree_statement(statement);
			add_tree(statement);

			TRACE_EXIT();
		}

		void 
		parser::enumerate_tree_command(
			__inout cc65c::assembler::tree &tree
			)
		{
			TRACE_ENTRY();

			if(!cc65c::assembler::lexer::match(TOKEN_KEYWORD_COMMAND)) {
				THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_EXPECTING_COMMAND, true);
			}

			// TODO
			if(cc65c::assembler::lexer::match(TOKEN_KEYWORD_COMMAND)) {
				THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_EXPECTING_COMMAND, true);
			}
			// ---

			TRACE_EXIT();
		}

		void 
		parser::enumerate_tree_condition(
			__inout cc65c::assembler::tree &tree
			)
		{
			TRACE_ENTRY();

			if(!cc65c::assembler::lexer::match(TOKEN_KEYWORD_CONDITION)) {
				THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_EXPECTING_CONDITION, true);
			}

			switch(cc65c::assembler::lexer::token().subtype()) {
				case KEYWORD_CONDITION_IF:
					enumerate_tree_condition_if(tree);
					break;
				case KEYWORD_CONDITION_IF_DEFINE:
					enumerate_tree_condition_ifdef(tree);
					break;
				default:
					THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_INVALID_CONDITION, true);
			}

			TRACE_EXIT();
		}

		void 
		parser::enumerate_tree_condition_expression(
			__inout cc65c::assembler::tree &tree
			)
		{
			size_t index;

			TRACE_ENTRY();

			// TODO
			enumerate_tree_expression(tree);

			if(cc65c::assembler::lexer::match(TOKEN_OPERATOR_BINARY)) {
				index = tree.add(cc65c::assembler::lexer::token());

				if(!cc65c::assembler::lexer::has_next()) {
					THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_UNTERMINATED_EXPRESSION, true);
				}

				cc65c::assembler::lexer::move_next();
				tree.move_child_index(index);
				enumerate_tree_condition_expression(tree);
				tree.move_parent();
			}
			// ---

			TRACE_EXIT();
		}

		void 
		parser::enumerate_tree_condition_if(
			__inout cc65c::assembler::tree &tree
			)
		{
			size_t index;

			TRACE_ENTRY();

			if(!cc65c::assembler::lexer::match(TOKEN_KEYWORD_CONDITION, KEYWORD_CONDITION_IF)) {
				THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_EXPECTING_CONDITION_IF, true);
			}

			tree.add(cc65c::assembler::lexer::token());

			if(!cc65c::assembler::lexer::has_next()) {
				THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_UNTERMINATED_IF, true);
			}

			cc65c::assembler::lexer::move_next();
			enumerate_tree_condition_expression(tree);
			enumerate_tree_statement_list(tree);

			while(cc65c::assembler::lexer::match(TOKEN_KEYWORD_CONDITION, KEYWORD_CONDITION_ELSE_IF)) {
				index = tree.add(cc65c::assembler::lexer::token());

				if(!cc65c::assembler::lexer::has_next()) {
					THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_UNTERMINATED_IF, true);
				}

				cc65c::assembler::lexer::move_next();
				tree.move_child_index(index);
				enumerate_tree_condition_expression(tree);
				enumerate_tree_statement_list(tree);
				tree.move_parent();
			}

			if(cc65c::assembler::lexer::match(TOKEN_KEYWORD_CONDITION, KEYWORD_CONDITION_ELSE)) {
				index = tree.add(cc65c::assembler::lexer::token());

				if(!cc65c::assembler::lexer::has_next()) {
					THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_UNTERMINATED_IF, true);
				}

				cc65c::assembler::lexer::move_next();
				tree.move_child_index(index);
				enumerate_tree_statement_list(tree);
				tree.move_parent();
			}

			if(!cc65c::assembler::lexer::match(TOKEN_KEYWORD_CONDITION, KEYWORD_CONDITION_END_IF)) {
				THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_UNTERMINATED_IF, true);
			}

			if(cc65c::assembler::lexer::has_next()) {
				cc65c::assembler::lexer::move_next();
			}

			TRACE_EXIT();
		}

		void 
		parser::enumerate_tree_condition_ifdef(
			__inout cc65c::assembler::tree &tree
			)
		{
			size_t index;

			TRACE_ENTRY();

			if(!cc65c::assembler::lexer::match(TOKEN_KEYWORD_CONDITION, KEYWORD_CONDITION_IF_DEFINE)) {
				THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_EXPECTING_CONDITION_IF_DEFINE, true);
			}

			tree.add(cc65c::assembler::lexer::token());

			if(!cc65c::assembler::lexer::has_next()) {
				THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_UNTERMINATED_IF_DEFINE, true);
			}

			cc65c::assembler::lexer::move_next();

			if(!cc65c::assembler::lexer::match(TOKEN_IDENTIFIER)) {
				THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_EXPECTING_IDENTIFIER, true);
			}

			tree.add(cc65c::assembler::lexer::token());

			if(!cc65c::assembler::lexer::has_next()) {
				THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_UNTERMINATED_IF_DEFINE, true);
			}

			cc65c::assembler::lexer::move_next();
			enumerate_tree_statement_list(tree);

			while(cc65c::assembler::lexer::match(TOKEN_KEYWORD_CONDITION, KEYWORD_CONDITION_ELSE_IF_DEFINE)) {
				index = tree.add(cc65c::assembler::lexer::token());

				if(!cc65c::assembler::lexer::has_next()) {
					THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_UNTERMINATED_IF_DEFINE, true);
				}

				cc65c::assembler::lexer::move_next();
				tree.move_child_index(index);

				if(!cc65c::assembler::lexer::match(TOKEN_IDENTIFIER)) {
					THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_EXPECTING_IDENTIFIER, true);
				}

				tree.add(cc65c::assembler::lexer::token());

				if(!cc65c::assembler::lexer::has_next()) {
					THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_UNTERMINATED_IF_DEFINE, true);
				}

				cc65c::assembler::lexer::move_next();
				enumerate_tree_statement_list(tree);
				tree.move_parent();
			}

			if(cc65c::assembler::lexer::match(TOKEN_KEYWORD_CONDITION, KEYWORD_CONDITION_ELSE)) {
				index = tree.add(cc65c::assembler::lexer::token());

				if(!cc65c::assembler::lexer::has_next()) {
					THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_UNTERMINATED_IF_DEFINE, true);
				}

				cc65c::assembler::lexer::move_next();
				tree.move_child_index(index);
				enumerate_tree_statement_list(tree);
				tree.move_parent();
			}

			if(!cc65c::assembler::lexer::match(TOKEN_KEYWORD_CONDITION, KEYWORD_CONDITION_END_IF)) {
				THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_UNTERMINATED_IF_DEFINE, true);
			}

			if(cc65c::assembler::lexer::has_next()) {
				cc65c::assembler::lexer::move_next();
			}

			TRACE_EXIT();
		}

		void 
		parser::enumerate_tree_define(
			__inout cc65c::assembler::tree &tree
			)
		{
			size_t index = 0;
			uint32_t subtype;

			TRACE_ENTRY();

			if(!cc65c::assembler::lexer::match(TOKEN_KEYWORD_DEFINE)) {
				THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_EXPECTING_DEFINE, true);
			}

			subtype = cc65c::assembler::lexer::token().subtype();
			switch(subtype) {
				case KEYWORD_DEFINE_DATA_BYTE:
				case KEYWORD_DEFINE_DATA_WORD:
				case KEYWORD_DEFINE_DEFINE_BYTE:
				case KEYWORD_DEFINE_ORIGIN:
				case KEYWORD_DEFINE_RESERVE:
				case KEYWORD_DEFINE_SEGMENT:
				case KEYWORD_DEFINE_UNDEFINE:
					index = tree.add(cc65c::assembler::lexer::token());
					break;
				default:
					THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_INVALID_DEFINE, true);
			}

			if(!cc65c::assembler::lexer::has_next()) {
				THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_UNTERMINATED_DEFINE, true);
			}

			cc65c::assembler::lexer::move_next();

			switch(subtype) {
				case KEYWORD_DEFINE_DATA_BYTE:
				case KEYWORD_DEFINE_DATA_WORD:
					enumerate_tree_expression_list(tree);
					break;
				case KEYWORD_DEFINE_DEFINE_BYTE:

					if(!cc65c::assembler::lexer::match(TOKEN_IDENTIFIER)) {
						THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_EXPECTING_IDENTIFIER, true);
					}

					tree.add(cc65c::assembler::lexer::token());

					if(!cc65c::assembler::lexer::has_next()) {
						THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_EXPECTING_EXPRESSION, true);
					}

					cc65c::assembler::lexer::move_next();
					enumerate_tree_expression(tree);
					break;
				case KEYWORD_DEFINE_ORIGIN:
					enumerate_tree_expression(tree);
					break;
				case KEYWORD_DEFINE_RESERVE:
					enumerate_tree_expression(tree);

					if(!cc65c::assembler::lexer::match(TOKEN_SYMBOL_SEPERATOR)) {
						THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_EXPECTING_SEPERATOR, true);
					}

					if(!cc65c::assembler::lexer::has_next()) {
						THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_EXPECTING_EXPRESSION, true);
					}

					cc65c::assembler::lexer::move_next();
					enumerate_tree_expression(tree);
					break;
				case KEYWORD_DEFINE_SEGMENT:
				case KEYWORD_DEFINE_UNDEFINE:

					if(!cc65c::assembler::lexer::match(TOKEN_IDENTIFIER)) {
						THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_EXPECTING_IDENTIFIER, true);
					}

					tree.add(cc65c::assembler::lexer::token());

					if(cc65c::assembler::lexer::has_next()) {
						cc65c::assembler::lexer::move_next();
					}
					break;
				default:
					THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_INVALID_DEFINE, true);
			}

			TRACE_EXIT();
		}

		void 
		parser::enumerate_tree_expression(
			__inout cc65c::assembler::tree &tree
			)
		{
			TRACE_ENTRY();

			enumerate_tree_expression_term_0(tree);

			TRACE_EXIT();
		}

		void 
		parser::enumerate_tree_expression_compound(
			__inout cc65c::assembler::tree &tree
			)
		{
			TRACE_ENTRY();

			if(!cc65c::assembler::lexer::match(TOKEN_SYMBOL_BRACKET, SYMBOL_BRACKET_OPEN)) {
				THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_EXPECTING_BRACKET, true);
			}

			if(!cc65c::assembler::lexer::has_next()) {
				THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_UNTERMINATED_BRACKET, true);
			}

			cc65c::assembler::lexer::move_next();
			enumerate_tree_expression(tree);

			if(!cc65c::assembler::lexer::match(TOKEN_SYMBOL_BRACKET, SYMBOL_BRACKET_CLOSE)) {
				THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_UNTERMINATED_BRACKET, true);
			}

			if(cc65c::assembler::lexer::has_next()) {
				cc65c::assembler::lexer::move_next();
			}

			TRACE_EXIT();
		}

		void 
		parser::enumerate_tree_expression_factor(
			__inout cc65c::assembler::tree &tree
			)
		{
			TRACE_ENTRY();

			switch(cc65c::assembler::lexer::token().type()) {
				case TOKEN_IDENTIFIER:
				case TOKEN_LITERAL:
					enumerate_tree_expression_literal(tree);
					break;
				case TOKEN_KEYWORD_MACRO:
					enumerate_tree_macro(tree);
					break;
				case TOKEN_SCALAR:
				case TOKEN_SYMBOL_POSITION:
					tree.add(cc65c::assembler::lexer::token());

					if(cc65c::assembler::lexer::has_next()) {
						cc65c::assembler::lexer::move_next();
					}
					break;
				case TOKEN_SYMBOL_BRACKET:
					enumerate_tree_expression_compound(tree);
					break;
				default:
					THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_EXPECTING_EXPRESSION, true);
			}

			TRACE_EXIT();
		}

		void 
		parser::enumerate_tree_expression_list(
			__inout cc65c::assembler::tree &tree
			)
		{
			TRACE_ENTRY();

			enumerate_tree_expression(tree);

			if(cc65c::assembler::lexer::match(TOKEN_SYMBOL_SEPERATOR)) {

				if(!cc65c::assembler::lexer::has_next()) {
					THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_EXPECTING_EXPRESSION, true);
				}

				cc65c::assembler::lexer::move_next();
				enumerate_tree_expression_list(tree);
			}

			TRACE_EXIT();
		}

		void 
		parser::enumerate_tree_expression_literal(
			__inout cc65c::assembler::tree &tree
			)
		{
			size_t index;

			TRACE_ENTRY();

			if(!cc65c::assembler::lexer::match(TOKEN_IDENTIFIER)
					&& !cc65c::assembler::lexer::match(TOKEN_LITERAL)) {
				THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_EXPECTING_LITERAL, true);
			}

			index = tree.add(cc65c::assembler::lexer::token());

			if(cc65c::assembler::lexer::has_next()) {
				cc65c::assembler::lexer::move_next();

				if(cc65c::assembler::lexer::match(TOKEN_SYMBOL_BRACE, SYMBOL_BRACE_OPEN)) {

					if(!cc65c::assembler::lexer::has_next()) {
						THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_UNTERMINATED_BRACE, true);
					}

					cc65c::assembler::lexer::move_next();
					tree.move_child_index(index);
					enumerate_tree_expression(tree);
					tree.move_parent();

					if(!cc65c::assembler::lexer::match(TOKEN_SYMBOL_BRACE, SYMBOL_BRACE_CLOSE)) {
						THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_UNTERMINATED_BRACE, true);
					}

					if(cc65c::assembler::lexer::has_next()) {
						cc65c::assembler::lexer::move_next();
					}
				}
			}

			TRACE_EXIT();
		}

		void 
		parser::enumerate_tree_expression_term_0(
			__inout cc65c::assembler::tree &tree
			)
		{
			TRACE_ENTRY();

			enumerate_tree_expression_term_1(tree);

			// TODO

			TRACE_EXIT();
		}

		void 
		parser::enumerate_tree_expression_term_1(
			__inout cc65c::assembler::tree &tree
			)
		{
			TRACE_ENTRY();

			enumerate_tree_expression_term_2(tree);

			// TODO

			TRACE_EXIT();
		}

		void 
		parser::enumerate_tree_expression_term_2(
			__inout cc65c::assembler::tree &tree
			)
		{
			TRACE_ENTRY();

			enumerate_tree_expression_term_3(tree);

			// TODO

			TRACE_EXIT();
		}

		void 
		parser::enumerate_tree_expression_term_3(
			__inout cc65c::assembler::tree &tree
			)
		{
			TRACE_ENTRY();

			enumerate_tree_expression_term_4(tree);

			// TODO

			TRACE_EXIT();
		}

		void 
		parser::enumerate_tree_expression_term_4(
			__inout cc65c::assembler::tree &tree
			)
		{
			size_t index;
			bool move = false;

			TRACE_ENTRY();

			if(cc65c::assembler::lexer::match(TOKEN_OPERATOR_UNARY, OPERATOR_UNARY_NOT_BINARY)
					|| cc65c::assembler::lexer::match(TOKEN_OPERATOR_UNARY, OPERATOR_UNARY_NOT_LOGICAL)) {
				move = (tree.size() > 0);
				index = tree.add(cc65c::assembler::lexer::token());

				if(move) {
					tree.move_child_index(index);
				}

				if(!cc65c::assembler::lexer::has_next()) {
					THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_UNTERMINATED_EXPRESSION, true);
				}

				cc65c::assembler::lexer::move_next();
			}

			enumerate_tree_expression_factor(tree);

			if(move) {
				tree.move_parent();
			}

			TRACE_EXIT();
		}

		void 
		parser::enumerate_tree_include(
			__inout cc65c::assembler::tree &tree
			)
		{
			TRACE_ENTRY();

			if(!cc65c::assembler::lexer::match(TOKEN_KEYWORD_INCLUDE)) {
				THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_EXPECTING_INCLUDE, true);
			}

			tree.add(cc65c::assembler::lexer::token());

			if(!cc65c::assembler::lexer::has_next()) {
				THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_EXPECTING_LITERAL, true);
			}

			cc65c::assembler::lexer::move_next();

			if(!cc65c::assembler::lexer::match(TOKEN_LITERAL)) {
				THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_EXPECTING_LITERAL, true);
			}

			tree.add(cc65c::assembler::lexer::token());

			if(cc65c::assembler::lexer::has_next()) {
				cc65c::assembler::lexer::move_next();
			}

			TRACE_EXIT();
		}

		void 
		parser::enumerate_tree_label(
			__inout cc65c::assembler::tree &tree
			)
		{
			TRACE_ENTRY();

			if(!cc65c::assembler::lexer::match(TOKEN_LABEL)) {
				THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_EXPECTING_LABEL, true);
			}

			tree.add(cc65c::assembler::lexer::token());

			if(cc65c::assembler::lexer::has_next()) {
				cc65c::assembler::lexer::move_next();
			}

			TRACE_EXIT();
		}

		void 
		parser::enumerate_tree_macro(
			__inout cc65c::assembler::tree &tree
			)
		{
			size_t index = 0;
			uint32_t subtype;

			TRACE_ENTRY();

			if(!cc65c::assembler::lexer::match(TOKEN_KEYWORD_MACRO)) {
				THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_EXPECTING_MACRO, true);
			}

			subtype = cc65c::assembler::lexer::token().subtype();
			switch(subtype) {
				case KEYWORD_MACRO_BYTE:
				case KEYWORD_MACRO_LOW:
				case KEYWORD_MACRO_HIGH:
				case KEYWORD_MACRO_WORD:
					index = tree.add(cc65c::assembler::lexer::token());
					break;
				default:
					THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_INVALID_MACRO, true);
			}

			if(!cc65c::assembler::lexer::has_next()) {
				THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_EXPECTING_BRACKET, true);
			}

			cc65c::assembler::lexer::move_next();

			if(!cc65c::assembler::lexer::match(TOKEN_SYMBOL_BRACKET, SYMBOL_BRACKET_OPEN)) {
				THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_EXPECTING_BRACKET, true);
			}

			if(!cc65c::assembler::lexer::has_next()) {
				THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_UNTERMINATED_MACRO, true);
			}

			cc65c::assembler::lexer::move_next();
			tree.move_child_index(index);

			switch(subtype) {
				case KEYWORD_MACRO_BYTE:
				case KEYWORD_MACRO_LOW:
				case KEYWORD_MACRO_HIGH:
					enumerate_tree_expression(tree);
					break;
				case KEYWORD_MACRO_WORD:
					enumerate_tree_expression(tree);

					if(!cc65c::assembler::lexer::match(TOKEN_SYMBOL_SEPERATOR)) {
						THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_EXPECTING_SEPERATOR, true);
					}

					if(!cc65c::assembler::lexer::has_next()) {
						THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_EXPECTING_EXPRESSION, true);
					}

					cc65c::assembler::lexer::move_next();
					enumerate_tree_expression(tree);
					break;
				default:
					THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_INVALID_MACRO, true);
			}

			tree.move_parent();

			if(!cc65c::assembler::lexer::match(TOKEN_SYMBOL_BRACKET, SYMBOL_BRACKET_CLOSE)) {
				THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_UNTERMINATED_MACRO, true);
			}

			if(cc65c::assembler::lexer::has_next()) {
				cc65c::assembler::lexer::move_next();
			}

			TRACE_EXIT();
		}

		void 
		parser::enumerate_tree_statement(
			__inout cc65c::assembler::tree &tree
			)
		{
			TRACE_ENTRY();

			switch(cc65c::assembler::lexer::token().type()) {
				case TOKEN_KEYWORD_COMMAND:
					enumerate_tree_command(tree);
					break;
				case TOKEN_KEYWORD_CONDITION:
					enumerate_tree_condition(tree);
					break;
				case TOKEN_KEYWORD_DEFINE:
					enumerate_tree_define(tree);
					break;
				case TOKEN_KEYWORD_INCLUDE:
					enumerate_tree_include(tree);
					break;
				case TOKEN_LABEL:
					enumerate_tree_label(tree);
					break;
				default:
					THROW_LEXER_EXCEPTION(CC65C_ASSEMBLER_PARSER_EXCEPTION_EXPECTING_STATEMENT, true);
					break;
			}

			TRACE_EXIT();
		}

		void 
		parser::enumerate_tree_statement_list(
			__inout cc65c::assembler::tree &tree
			)
		{
			TRACE_ENTRY();

			while(cc65c::assembler::lexer::match(TOKEN_KEYWORD_COMMAND)
					|| cc65c::assembler::lexer::match(TOKEN_KEYWORD_CONDITION, KEYWORD_CONDITION_IF)
					|| cc65c::assembler::lexer::match(TOKEN_KEYWORD_CONDITION, KEYWORD_CONDITION_IF_DEFINE)
					|| cc65c::assembler::lexer::match(TOKEN_KEYWORD_DEFINE)
					|| cc65c::assembler::lexer::match(TOKEN_KEYWORD_INCLUDE)
					|| cc65c::assembler::lexer::match(TOKEN_LABEL)) {
				enumerate_tree_statement(tree);
			}

			TRACE_EXIT();
		}

		bool 
		parser::has_next(void)
		{
			bool result;

			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_parser_mutex);

			result = (m_tree.at(m_tree_position).type() != TREE_END);

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		bool 
		parser::has_previous(void)
		{
			bool result;

			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_parser_mutex);

			result = (m_tree_position > 0);

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		cc65c::assembler::tree &
		parser::move_next(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_parser_mutex);

			if(!has_next()) {
				THROW_CC65C_ASSEMBLER_PARSER_EXCEPTION_FORMAT(CC65C_ASSEMBLER_PARSER_EXCEPTION_NO_NEXT_TREE,
					"Position=%u", m_tree_position);
			}

			if(cc65c::assembler::lexer::match(TOKEN_BEGIN)) {
				cc65c::assembler::lexer::move_next();
			}

			if(cc65c::assembler::lexer::has_next() 
					&& (m_tree_position <= (m_tree.size() - TREE_SENTINEL_COUNT))) {
				enumerate_tree();
			}

			++m_tree_position;
			cc65c::assembler::tree &result = m_tree.at(m_tree_position);

			TRACE_EXIT();
			return result;
		}

		cc65c::assembler::tree &
		parser::move_next(
			__in cc65c::assembler::tree_t &type
			)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_parser_mutex);

			cc65c::assembler::tree &result = move_next();
			type = result.type();

			TRACE_EXIT();
			return result;
		}

		cc65c::assembler::tree &
		parser::move_previous(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_parser_mutex);

			if(!has_previous()) {
				THROW_CC65C_ASSEMBLER_PARSER_EXCEPTION_FORMAT(CC65C_ASSEMBLER_PARSER_EXCEPTION_NO_PREVIOUS_TREE,
					"Position=%u", m_tree_position);
			}

			--m_tree_position;
			cc65c::assembler::tree &result = m_tree.at(m_tree_position);

			TRACE_EXIT();
			return result;
		}

		cc65c::assembler::tree &
		parser::move_previous(
			__in cc65c::assembler::tree_t &type
			)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_parser_mutex);

			cc65c::assembler::tree &result = move_previous();
			type = result.type();

			TRACE_EXIT();
			return result;
		}

		void 
		parser::reset(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_parser_mutex);

			m_tree_position = 0;

			TRACE_EXIT();
		}

		void 
		parser::set(
			__in const std::string &input,
			__in_opt bool is_file
			)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_parser_mutex);

			cc65c::assembler::lexer::set(input, is_file);
			clear();

			TRACE_EXIT();
		}

		size_t 
		parser::size(void)
		{
			size_t result;

			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_parser_mutex);

			result = m_tree.size();

			TRACE_EXIT_FORMAT("Result=%u", result);
			return result;
		}

		std::string 
		parser::to_string(
			__in_opt bool verbose,
			__in_opt cc65c::core::stream_t index,
			__in_opt cc65c::core::stream_t offset
			)
		{
			std::string result;

			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_parser_mutex);

			result = cc65c::assembler::parser::as_string(*this, verbose);

			TRACE_EXIT();
			return result;
		}

		cc65c::assembler::tree &
		parser::tree(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_lexer_mutex);

			cc65c::assembler::tree &result = m_tree.at(m_tree_position);

			TRACE_EXIT();
			return result;
		}

		std::vector<cc65c::assembler::tree>::const_iterator 
		parser::tree_begin(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_lexer_mutex);

			std::vector<cc65c::assembler::tree>::const_iterator result = m_tree.begin();

			TRACE_EXIT();
			return result;
		}

		std::vector<cc65c::assembler::tree>::const_iterator 
		parser::tree_end(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_lexer_mutex);

			std::vector<cc65c::assembler::tree>::const_iterator result = m_tree.end();

			TRACE_EXIT();
			return result;
		}
	}
}
