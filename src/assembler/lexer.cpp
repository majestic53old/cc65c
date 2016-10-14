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

#include "../../include/assembler/lexer.h"
#include "lexer_type.h"
#include "../../include/core/trace.h"

namespace cc65c {

	namespace assembler {

		#define CHARACTER_COMMENT ';'
		#define CHARACTER_DIGIT_MIN '0'
		#define CHARACTER_NEWLINE '\n'
		#define CHARACTER_LABEL_TERMINATOR ':'
		#define CHARACTER_LITERAL_CHARACTER '\''
		#define CHARACTER_LITERAL_ESCAPE '\\'
		#define CHARACTER_LITERAL_STRING '\"'
		#define CHARACTER_SCALAR_BINARY 'b'
		#define CHARACTER_SCALAR_HEXIDECIMAL 'x'
		#define CHARACTER_SCALAR_MAX_BINARY '1'
		#define CHARACTER_SCALAR_MAX_OCTAL '7'
		#define CHARACTER_SCALAR_OCTAL 'c'
		#define CHARACTER_SIGN '-'
		#define CHARACTER_UNDERSCORE '_'

		#define EXCEPTION_TAB_COUNT 1

		#define TOKEN_SENTINEL_COUNT 2

		#define THROW_STREAM_EXCEPTION(_EXCEPT_, _VERB_) { \
			cc65c::core::stream_t column, row; \
			cc65c::assembler::stream::metadata(row, column); \
			THROW_CC65C_ASSEMBLER_LEXER_EXCEPTION_FORMAT(_EXCEPT_, "\n%s", STRING_CHECK( \
				cc65c::assembler::stream::as_exception(row, column, _VERB_, EXCEPTION_TAB_COUNT))); \
			}

		#define THROW_STREAM_EXCEPTION_FORMAT(_EXCEPT_, _ROW_, _COLUMN_, _VERB_) \
			THROW_CC65C_ASSEMBLER_LEXER_EXCEPTION_FORMAT(_EXCEPT_, "\n%s", STRING_CHECK( \
				cc65c::assembler::stream::as_exception(_ROW_, _COLUMN_, _VERB_, EXCEPTION_TAB_COUNT)))

		enum {
			SCALAR_BINARY = 0,
			SCALAR_DECIMAL,
			SCALAR_HEXIDECIMAL,
			SCALAR_OCTAL,
		};

		lexer::lexer(void) :
			m_token_position(0)
		{
			TRACE_ENTRY();

			clear();

			TRACE_EXIT();
		}

		lexer::lexer(
			__in const std::string &input,
			__in_opt bool is_file
			) :
				cc65c::assembler::stream(input, is_file),
				m_token_position(0)
		{
			TRACE_ENTRY();

			clear();

			TRACE_EXIT();
		}

		lexer::lexer(
			__in const lexer &other
			) :
				cc65c::assembler::stream(other),
				m_token(other.m_token),
				m_token_position(other.m_token_position)
		{
			TRACE_ENTRY();
			TRACE_EXIT();
		}

		lexer::~lexer(void)
		{
			TRACE_ENTRY();
			TRACE_EXIT();
		}

		lexer &
		lexer::operator=(
			__in const lexer &other
			)
		{
			TRACE_ENTRY();

			if(this != &other) {
				clear();
				cc65c::assembler::stream::operator=(other);
				m_token = other.m_token;
				m_token_position = other.m_token_position;
			}

			TRACE_EXIT_FORMAT("Result=%p", this);
			return *this;
		}

		void 
		lexer::add_token(
			__in cc65c::assembler::token_t type,
			__in_opt uint32_t subtype,
			__in_opt cc65c::core::stream_t row,
			__in_opt cc65c::core::stream_t column
			)
		{
			TRACE_ENTRY();

			m_token.insert(m_token.begin() + m_token_position + 1, cc65c::assembler::token(type, subtype, row, column));

			TRACE_EXIT();
		}

		void 
		lexer::add_token(
			__in cc65c::assembler::token_t type,
			__in const std::string &key,
			__in_opt cc65c::core::stream_t row,
			__in_opt cc65c::core::stream_t column
			)
		{
			TRACE_ENTRY();

			m_token.insert(m_token.begin() + m_token_position + 1, cc65c::assembler::token(type, key, row, column));

			TRACE_EXIT();
		}

		void 
		lexer::add_token(
			__in int32_t value,
			__in_opt cc65c::core::stream_t row,
			__in_opt cc65c::core::stream_t column
			)
		{
			TRACE_ENTRY();

			m_token.insert(m_token.begin() + m_token_position + 1, cc65c::assembler::token(value, row, column));

			TRACE_EXIT();
		}

		std::string 
		lexer::as_exception(
			__in_opt bool verbose,
			__in_opt size_t tabs					
			)
		{
			std::string result;

			TRACE_ENTRY();

			cc65c::assembler::token &tok = m_token.at(m_token_position);
			result = as_exception(tok.row(), tok.column(), verbose, tabs);

			TRACE_EXIT();
			return result;
		}

		std::string 
		lexer::as_exception(
			__in cc65c::core::stream_t row,
			__in cc65c::core::stream_t column,
			__in_opt bool verbose,
			__in_opt size_t tabs					
			)
		{
			std::string result;

			TRACE_ENTRY();

			result = cc65c::assembler::stream::as_exception(row, column, verbose, tabs);

			TRACE_EXIT();
			return result;
		}

		std::string 
		lexer::as_string(
			__in const lexer &reference,
			__in_opt bool verbose,
			__in_opt cc65c::core::stream_t index,
			__in_opt cc65c::core::stream_t offset
			)
		{
			std::stringstream result;

			TRACE_ENTRY();

			result << cc65c::assembler::token::as_string(reference.m_token.at(reference.m_token_position), verbose);

			switch(cc65c::assembler::token::as_type(reference.m_token.at(reference.m_token_position))) {
				case TOKEN_BEGIN:
				case TOKEN_END:
					break;
				default:
					result << ", Token=" << reference.m_token_position 
						<< "/" << (reference.m_token.size() - TOKEN_SENTINEL_COUNT);
					break;
			}

			TRACE_EXIT();
			return result.str();
		}

		void 
		lexer::clear(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_lexer_mutex);

			cc65c::assembler::stream::reset();
			m_token.clear();
			m_token.push_back(cc65c::assembler::token(TOKEN_BEGIN));
			m_token.push_back(cc65c::assembler::token(TOKEN_END));
			skip_whitespace();
			m_token.front().set_metadata(cc65c::assembler::stream::row(), cc65c::assembler::stream::column());
			m_token.back().set_metadata(cc65c::assembler::stream::row(), cc65c::assembler::stream::column());
			m_token_position = 0;

			TRACE_EXIT();
		}

		size_t 
		lexer::enumerate(void)
		{
			size_t result;

			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_lexer_mutex);

			while(has_next()) {
				move_next();
			}

			result = size();
			reset();

			TRACE_EXIT_FORMAT("Result=%u", result);
			return result;
		}

		bool 
		lexer::has_next(void)
		{
			bool result;

			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_lexer_mutex);

			result = (m_token.at(m_token_position).type() != TOKEN_END);

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		void 
		lexer::enumerate_token(void)
		{
			cc65c::assembler::character_t type;

			TRACE_ENTRY();

			type = cc65c::assembler::stream::type();
			switch(type) {
				case CHARACTER_ALPHA:
					enumerate_token_alpha();
					break;
				case CHARACTER_DIGIT:
					enumerate_token_digit();
					break;
				case CHARACTER_SYMBOL:

					if(cc65c::assembler::stream::match(CHARACTER_SYMBOL, CHARACTER_UNDERSCORE)) {
						enumerate_token_alpha();
					} else {

						if(cc65c::assembler::stream::match(CHARACTER_SYMBOL, CHARACTER_SIGN)) {

							if(cc65c::assembler::stream::has_next()) {
								cc65c::assembler::stream::move_next(type);
								cc65c::assembler::stream::move_previous();

								if(type == CHARACTER_DIGIT) {
									enumerate_token_digit();
									break;
								}
							}
						}

						enumerate_token_symbol();
					}
					break;
				default:
					THROW_STREAM_EXCEPTION(CC65C_ASSEMBLER_LEXER_EXCEPTION_EXPECTING_TOKEN, true);
					break;
			}

			TRACE_EXIT();
		}

		void 
		lexer::enumerate_token_alpha(void)
		{			
			std::string key;
			bool label = false;
			cc65c::core::stream_t column, row;
			uint32_t subtype = SUBTYPE_UNDEFINED;
			cc65c::assembler::token_t type = TOKEN_IDENTIFIER;

			TRACE_ENTRY();

			cc65c::assembler::stream::metadata(row, column);

			if(!cc65c::assembler::stream::match(CHARACTER_ALPHA)
					&& !cc65c::assembler::stream::match(CHARACTER_SYMBOL, CHARACTER_UNDERSCORE)) {
				THROW_STREAM_EXCEPTION_FORMAT(CC65C_ASSEMBLER_LEXER_EXCEPTION_EXPECTING_ALPHA, 
					row, column, true);
			}

			for(;;) {
				key += cc65c::assembler::stream::character();

				if(!cc65c::assembler::stream::has_next()) {
					break;
				}

				cc65c::assembler::stream::move_next();

				if(!cc65c::assembler::stream::match(CHARACTER_ALPHA)
						&& !cc65c::assembler::stream::match(CHARACTER_DIGIT)
						&& !cc65c::assembler::stream::match(CHARACTER_SYMBOL, CHARACTER_UNDERSCORE)) {

					if(cc65c::assembler::stream::match(CHARACTER_SYMBOL, CHARACTER_LABEL_TERMINATOR)) {
						label = true;

						if(cc65c::assembler::stream::has_next()) {
							cc65c::assembler::stream::move_next();
						}
					}

					break;
				}
			}

			cc65c::assembler::token::determine_type(key, type, subtype);

			switch(type) {
				case TOKEN_IDENTIFIER:

					if(label) {
						type = TOKEN_LABEL;
					}

					add_token(type, key, row, column);
					break;
				case TOKEN_KEYWORD_COMMAND:
				case TOKEN_KEYWORD_CONDITION:
				case TOKEN_KEYWORD_DEFINE:
				case TOKEN_KEYWORD_INCLUDE:
				case TOKEN_KEYWORD_MACRO:
				case TOKEN_KEYWORD_REGISTER:

					if(label) {
						THROW_STREAM_EXCEPTION_FORMAT(CC65C_ASSEMBLER_LEXER_EXCEPTION_INVALID_LABEL, 
							row, column, true);
					}

					add_token(type, subtype, row, column);
					break;
				default:
					THROW_STREAM_EXCEPTION_FORMAT(CC65C_ASSEMBLER_LEXER_EXCEPTION_EXPECTING_ALPHA, 
						row, column, true);
			}

			TRACE_EXIT();
		}

		void 
		lexer::enumerate_token_digit(void)
		{
			int32_t value;
			bool negative = false;
			std::stringstream stream;
			uint32_t type = SCALAR_DECIMAL;
			cc65c::core::stream_t column, row;

			TRACE_ENTRY();

			cc65c::assembler::stream::metadata(row, column);

			if(cc65c::assembler::stream::match(CHARACTER_SYMBOL, CHARACTER_SIGN)) {

				if(!cc65c::assembler::stream::has_next()) {
					THROW_STREAM_EXCEPTION_FORMAT(CC65C_ASSEMBLER_LEXER_EXCEPTION_UNTERMINATED_SCALAR, 
						row, column, true);
				}

				cc65c::assembler::stream::move_next();
				negative = true;
			}

			if(!cc65c::assembler::stream::match(CHARACTER_DIGIT)) {
				THROW_STREAM_EXCEPTION_FORMAT(CC65C_ASSEMBLER_LEXER_EXCEPTION_EXPECTING_SCALAR, 
					row, column, true);
			}

			if(cc65c::assembler::stream::match(CHARACTER_DIGIT_MIN)) {
				
				if(cc65c::assembler::stream::has_next()) {

					cc65c::assembler::stream::move_next();
					if(cc65c::assembler::stream::match(CHARACTER_ALPHA, CHARACTER_SCALAR_BINARY)) {
						type = SCALAR_BINARY;
					} else if(cc65c::assembler::stream::match(CHARACTER_ALPHA, CHARACTER_SCALAR_HEXIDECIMAL)) {
						type = SCALAR_HEXIDECIMAL;
					} else if(cc65c::assembler::stream::match(CHARACTER_ALPHA, CHARACTER_SCALAR_OCTAL)) {
						type = SCALAR_OCTAL;
					} else if(!cc65c::assembler::stream::match(CHARACTER_DIGIT)) {
						cc65c::assembler::stream::move_previous();
					}

					if(type != SCALAR_DECIMAL) {

						if(!cc65c::assembler::stream::has_next()) {
							THROW_STREAM_EXCEPTION_FORMAT(CC65C_ASSEMBLER_LEXER_EXCEPTION_UNTERMINATED_SCALAR,
								row, column, true);
						}

						cc65c::assembler::stream::move_next();
					}
				}
			}

			switch(type) {
				case SCALAR_BINARY:
					value = enumerate_token_scalar_binary();
					break;
				case SCALAR_DECIMAL:
					value = enumerate_token_scalar_decimal();
					break;
				case SCALAR_HEXIDECIMAL:
					value = enumerate_token_scalar_hexidecimal();
					break;
				case SCALAR_OCTAL:
					value = enumerate_token_scalar_octal();
					break;
				default:
					THROW_STREAM_EXCEPTION_FORMAT(CC65C_ASSEMBLER_LEXER_EXCEPTION_INVALID_SCALAR, 
						row, column, true);
			}

			add_token(negative ? (value * -1) : value, row, column);

			TRACE_EXIT();
		}

		std::string 
		lexer::enumerate_token_literal_character(void)
		{
			std::string result;
			cc65c::core::stream_t column, row;

			TRACE_ENTRY();

			cc65c::assembler::stream::metadata(row, column);

			if(!cc65c::assembler::stream::match(CHARACTER_SYMBOL, CHARACTER_LITERAL_CHARACTER)) {
				THROW_STREAM_EXCEPTION_FORMAT(CC65C_ASSEMBLER_LEXER_EXCEPTION_EXPECTING_LITERAL,
					row, column, true);
			}

			if(!cc65c::assembler::stream::has_next()) {
				THROW_STREAM_EXCEPTION_FORMAT(CC65C_ASSEMBLER_LEXER_EXCEPTION_UNTERMINATED_LITERAL_CHARACTER,
					row, column, true);
			}

			cc65c::assembler::stream::move_next();

			if(!cc65c::assembler::stream::match(CHARACTER_SYMBOL, CHARACTER_LITERAL_CHARACTER)) {

				if(cc65c::assembler::stream::match(CHARACTER_SYMBOL, CHARACTER_LITERAL_ESCAPE)) {
					result += enumerate_token_literal_character_escape();
				} else {
					result += cc65c::assembler::stream::character();

					if(cc65c::assembler::stream::has_next()) {
						cc65c::assembler::stream::move_next();
					}
				}
			} else {
				THROW_STREAM_EXCEPTION_FORMAT(CC65C_ASSEMBLER_LEXER_EXCEPTION_INVALID_LITERAL_CHARACTER,
					row, column, true);
			}

			if(!cc65c::assembler::stream::match(CHARACTER_SYMBOL, CHARACTER_LITERAL_CHARACTER)) {
				THROW_STREAM_EXCEPTION_FORMAT(CC65C_ASSEMBLER_LEXER_EXCEPTION_UNTERMINATED_LITERAL_CHARACTER,
					row, column, true);
			}

			if(cc65c::assembler::stream::has_next()) {
				cc65c::assembler::stream::move_next();
			}

			TRACE_EXIT_FORMAT("Result[%u]=%s", result.size(), STRING_CHECK(result));
			return result;
		}

		std::string 
		lexer::enumerate_token_literal_character_escape(void)
		{
			size_t iter = 0;
			std::string result;
			cc65c::core::stream_t column, row;

			TRACE_ENTRY();

			cc65c::assembler::stream::metadata(row, column);

			if(!cc65c::assembler::stream::match(CHARACTER_SYMBOL, CHARACTER_LITERAL_ESCAPE)) {
				THROW_STREAM_EXCEPTION_FORMAT(CC65C_ASSEMBLER_LEXER_EXCEPTION_EXPECTING_LITERAL_CHARACTER_ESCAPE,
					row, column, true);
			}

			if(!cc65c::assembler::stream::has_next()) {
				THROW_STREAM_EXCEPTION_FORMAT(CC65C_ASSEMBLER_LEXER_EXCEPTION_UNTERMINATED_LITERAL_CHARACTER_ESCAPE,
					row, column, true);
			}

			cc65c::assembler::stream::move_next();

			for(; iter <= CHARACTER_ESCAPE_MAX; ++iter) {

				if(cc65c::assembler::stream::character() == CHARACTER_ESCAPE_CHARACTER(iter)) {
					break;
				}
			}

			if(iter > CHARACTER_ESCAPE_MAX) {
				result += (uint8_t) enumerate_token_scalar_octal();
			} else if(iter == CHARACTER_ESCAPE_SCALAR_HEXIDECIMAL) {

				if(!cc65c::assembler::stream::has_next()) {
					THROW_STREAM_EXCEPTION_FORMAT(CC65C_ASSEMBLER_LEXER_EXCEPTION_UNTERMINATED_LITERAL_CHARACTER_ESCAPE,
						row, column, true);
				}

				cc65c::assembler::stream::move_next();
				result += (uint8_t) enumerate_token_scalar_hexidecimal();
			} else {
				result += CHARACTER_ESCAPE_CHARACTER_CONVERSION(iter);

				if(cc65c::assembler::stream::has_next()) {
					cc65c::assembler::stream::move_next();
				}
			}

			TRACE_EXIT_FORMAT("Result[%u]=%s", result.size(), STRING_CHECK(result));
			return result;
		}

		std::string 
		lexer::enumerate_token_literal_string(void)
		{
			std::string result;
			cc65c::core::stream_t column, row;

			TRACE_ENTRY();

			cc65c::assembler::stream::metadata(row, column);

			if(!cc65c::assembler::stream::match(CHARACTER_SYMBOL, CHARACTER_LITERAL_STRING)) {
				THROW_STREAM_EXCEPTION_FORMAT(CC65C_ASSEMBLER_LEXER_EXCEPTION_EXPECTING_LITERAL,
					row, column, true);
			}

			if(!cc65c::assembler::stream::has_next()) {
				THROW_STREAM_EXCEPTION_FORMAT(CC65C_ASSEMBLER_LEXER_EXCEPTION_UNTERMINATED_LITERAL_STRING,
					row, column, true);
			}

			cc65c::assembler::stream::move_next();

			for(;;) {

				if(cc65c::assembler::stream::match(CHARACTER_SYMBOL, CHARACTER_LITERAL_STRING)) {
					break;
				}

				if(cc65c::assembler::stream::match(CHARACTER_SYMBOL, CHARACTER_LITERAL_ESCAPE)) {
					result += enumerate_token_literal_character_escape();

					if(!cc65c::assembler::stream::has_next()) {
						break;
					}
				} else {
					result += cc65c::assembler::stream::character();

					if(!cc65c::assembler::stream::has_next()) {
						break;
					}

					cc65c::assembler::stream::move_next();
				}
			}

			if(!cc65c::assembler::stream::match(CHARACTER_SYMBOL, CHARACTER_LITERAL_STRING)) {
				THROW_STREAM_EXCEPTION_FORMAT(CC65C_ASSEMBLER_LEXER_EXCEPTION_UNTERMINATED_LITERAL_STRING,
					row, column, true);
			}

			if(cc65c::assembler::stream::has_next()) {
				cc65c::assembler::stream::move_next();
			}

			TRACE_EXIT_FORMAT("Result[%u]=%s", result.size(), STRING_CHECK(result));
			return result;
		}

		int32_t 
		lexer::enumerate_token_scalar_binary(void)
		{
			int32_t result;
			std::stringstream stream;

			TRACE_ENTRY();

			if(!cc65c::assembler::stream::match(CHARACTER_DIGIT)
					|| (cc65c::assembler::stream::character() > CHARACTER_SCALAR_MAX_BINARY)) {
				THROW_STREAM_EXCEPTION(CC65C_ASSEMBLER_LEXER_EXCEPTION_UNTERMINATED_SCALAR_BINARY, true);
			}

			for(;;) {
				stream << cc65c::assembler::stream::character();

				if(!cc65c::assembler::stream::has_next()) {
					break;
				}

				cc65c::assembler::stream::move_next();

				if(cc65c::assembler::stream::match(CHARACTER_DIGIT)) {

					if(cc65c::assembler::stream::character() > CHARACTER_SCALAR_MAX_BINARY) {
						THROW_STREAM_EXCEPTION(CC65C_ASSEMBLER_LEXER_EXCEPTION_INVALID_SCALAR_BINARY, true);
					}
				} else {
					break;
				}
			}

			result = cc65c::assembler::token::as_scalar(stream.str(), SCALAR_BASE_BINARY);

			TRACE_EXIT_FORMAT("Result=%i", result);
			return result;
		}

		int32_t 
		lexer::enumerate_token_scalar_decimal(void)
		{
			int32_t result;
			std::stringstream stream;

			TRACE_ENTRY();

			if(!cc65c::assembler::stream::match(CHARACTER_DIGIT)) {
				THROW_STREAM_EXCEPTION(CC65C_ASSEMBLER_LEXER_EXCEPTION_UNTERMINATED_SCALAR_DECIMAL, true);
			}

			for(;;) {
				stream << cc65c::assembler::stream::character();

				if(!cc65c::assembler::stream::has_next()) {
					break;
				}

				cc65c::assembler::stream::move_next();

				if(!cc65c::assembler::stream::match(CHARACTER_DIGIT)) {
					break;
				}
			}

			result = cc65c::assembler::token::as_scalar(stream.str(), SCALAR_BASE_DECIMAL);

			TRACE_EXIT_FORMAT("Result=%i", result);
			return result;
		}

		int32_t 
		lexer::enumerate_token_scalar_hexidecimal(void)
		{
			int32_t result;
			std::stringstream stream;

			TRACE_ENTRY();

			if(!cc65c::assembler::stream::match(CHARACTER_DIGIT_HEXIDECIMAL)) {
				THROW_STREAM_EXCEPTION(CC65C_ASSEMBLER_LEXER_EXCEPTION_UNTERMINATED_SCALAR_HEXIDECIMAL, true);
			}

			for(;;) {
				stream << cc65c::assembler::stream::character();

				if(!cc65c::assembler::stream::has_next()) {
					break;
				}

				cc65c::assembler::stream::move_next();

				if(!cc65c::assembler::stream::match(CHARACTER_DIGIT_HEXIDECIMAL)) {
					break;
				}
			}

			result = cc65c::assembler::token::as_scalar(stream.str(), SCALAR_BASE_HEXIDECIMAL);

			TRACE_EXIT_FORMAT("Result=%i", result);
			return result;
		}

		int32_t 
		lexer::enumerate_token_scalar_octal(void)
		{
			int32_t result;
			std::stringstream stream;

			TRACE_ENTRY();

			if(!cc65c::assembler::stream::match(CHARACTER_DIGIT)
					|| (cc65c::assembler::stream::character() > CHARACTER_SCALAR_MAX_OCTAL)) {
				THROW_STREAM_EXCEPTION(CC65C_ASSEMBLER_LEXER_EXCEPTION_UNTERMINATED_SCALAR_OCTAL, true);
			}

			for(;;) {
				stream << cc65c::assembler::stream::character();

				if(!cc65c::assembler::stream::has_next()) {
					break;
				}

				cc65c::assembler::stream::move_next();

				if(cc65c::assembler::stream::match(CHARACTER_DIGIT)) {

					if(cc65c::assembler::stream::character() > CHARACTER_SCALAR_MAX_OCTAL) {
						THROW_STREAM_EXCEPTION(CC65C_ASSEMBLER_LEXER_EXCEPTION_INVALID_SCALAR_OCTAL, true);
					}
				} else {
					break;
				}
			}

			result = cc65c::assembler::token::as_scalar(stream.str(), SCALAR_BASE_OCTAL);

			TRACE_EXIT_FORMAT("Result=%i", result);
			return result;
		}

		void 
		lexer::enumerate_token_symbol(void)
		{
			std::string key;
			cc65c::assembler::token_t type;
			cc65c::core::stream_t column, row;
			uint32_t subtype = SUBTYPE_UNDEFINED;

			TRACE_ENTRY();

			cc65c::assembler::stream::metadata(row, column);

			if(!cc65c::assembler::stream::match(CHARACTER_SYMBOL)) {
				THROW_STREAM_EXCEPTION_FORMAT(CC65C_ASSEMBLER_LEXER_EXCEPTION_EXPECTING_SYMBOL, 
					row, column, true);
			}

			if(cc65c::assembler::stream::match(CHARACTER_LITERAL_CHARACTER)) {
				add_token(TOKEN_LITERAL, enumerate_token_literal_character(), row, column);
			} else if(cc65c::assembler::stream::match(CHARACTER_LITERAL_STRING)) {
				add_token(TOKEN_LITERAL, enumerate_token_literal_string(), row, column);
			} else {

				key += cc65c::assembler::stream::character();

				if(cc65c::assembler::stream::has_next()) {
					cc65c::assembler::stream::move_next();

					if(cc65c::assembler::stream::match(CHARACTER_SYMBOL)) {
						key += cc65c::assembler::stream::character();

						if(!cc65c::assembler::token::determine_type(key, type, subtype)) {
							key = key.substr(0, key.size() - 1);
							cc65c::assembler::stream::move_previous();
						}
					} else {
						cc65c::assembler::stream::move_previous();
					}
				}

				if(!cc65c::assembler::token::determine_type(key, type, subtype)) {
					THROW_STREAM_EXCEPTION_FORMAT(CC65C_ASSEMBLER_LEXER_EXCEPTION_INVALID_SYMBOL, 
						row, column, true);
				}

				if(cc65c::assembler::stream::has_next()) {
					cc65c::assembler::stream::move_next();
				}

				add_token(type, subtype, row, column);
			}

			TRACE_EXIT();
		}

		bool 
		lexer::has_previous(void)
		{
			bool result;

			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_lexer_mutex);

			result = (m_token_position > 0);

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		bool 
		lexer::match(
			__in cc65c::assembler::token_t type,
			__in_opt uint32_t subtype
			)
		{
			bool result;

			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_lexer_mutex);

			result = m_token.at(m_token_position).match(type, subtype);

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		bool 
		lexer::match(
			__in cc65c::assembler::token_t type,
			__in const std::string &key
			)
		{
			bool result;

			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_lexer_mutex);

			result = m_token.at(m_token_position).match(type, key);

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		bool 
		lexer::match(
			__in int32_t value
			)
		{
			bool result;

			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_lexer_mutex);

			result = m_token.at(m_token_position).match(value);

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		cc65c::assembler::token &
		lexer::move_next(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_lexer_mutex);

			if(!has_next()) {
				THROW_CC65C_ASSEMBLER_LEXER_EXCEPTION_FORMAT(CC65C_ASSEMBLER_LEXER_EXCEPTION_NO_NEXT_TOKEN,
					"Position=%u", m_token_position);
			}

			skip_whitespace();

			if(cc65c::assembler::stream::has_next() 
					&& (m_token_position <= (m_token.size() - TOKEN_SENTINEL_COUNT))) {
				enumerate_token();
				m_token.back().set_metadata(cc65c::assembler::stream::row(), cc65c::assembler::stream::column());
			}

			++m_token_position;

			TRACE_EXIT();
			return m_token.at(m_token_position);
		}

		cc65c::assembler::token &
		lexer::move_next(
			__inout cc65c::assembler::token_t &type
			)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_stream_mutex);

			cc65c::assembler::token &result = move_next();
			type = result.type();

			TRACE_EXIT();
			return result;
		}

		cc65c::assembler::token &
		lexer::move_previous(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_lexer_mutex);

			if(!has_previous()) {
				THROW_CC65C_ASSEMBLER_LEXER_EXCEPTION_FORMAT(CC65C_ASSEMBLER_LEXER_EXCEPTION_NO_PREVIOUS_TOKEN,
					"Position=%u", m_token_position);
			}

			--m_token_position;

			TRACE_EXIT();
			return m_token.at(m_token_position);
		}

		cc65c::assembler::token &
		lexer::move_previous(
			__inout cc65c::assembler::token_t &type
			)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_stream_mutex);

			cc65c::assembler::token &result = move_previous();
			type = result.type();

			TRACE_EXIT();
			return result;
		}

		void 
		lexer::reset(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_lexer_mutex);

			m_token_position = 0;

			TRACE_EXIT();
		}

		void 
		lexer::set(
			__in const std::string &input,
			__in_opt bool is_file
			)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_lexer_mutex);

			cc65c::assembler::stream::set(input, is_file);
			clear();

			TRACE_EXIT();
		}

		size_t 
		lexer::size(void)
		{
			size_t result;

			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_lexer_mutex);

			result = (m_token.size() - TOKEN_SENTINEL_COUNT);

			TRACE_EXIT_FORMAT("Result=%u", result);
			return result;
		}

		void 
		lexer::skip_comment(void)
		{
			cc65c::core::stream_t column, row;

			TRACE_ENTRY();

			if(cc65c::assembler::stream::match(CHARACTER_SYMBOL, CHARACTER_COMMENT)) {
				cc65c::assembler::stream::metadata(row, column);

				if(cc65c::assembler::stream::has_next()) {
					cc65c::assembler::stream::move_next();

					if(cc65c::assembler::stream::match(CHARACTER_SYMBOL, CHARACTER_COMMENT)) {

						for(;;) {

							if(!cc65c::assembler::stream::has_next()) {
								THROW_STREAM_EXCEPTION_FORMAT(
									CC65C_ASSEMBLER_LEXER_EXCEPTION_UNTERMINATED_COMMENT,
									row, column, true);
							}

							cc65c::assembler::stream::move_next();

							if(cc65c::assembler::stream::match(CHARACTER_SYMBOL, CHARACTER_COMMENT)) {

								if(!cc65c::assembler::stream::has_next()) {
									THROW_STREAM_EXCEPTION_FORMAT(
										CC65C_ASSEMBLER_LEXER_EXCEPTION_UNTERMINATED_COMMENT, 
										row, column, true);
								}

								cc65c::assembler::stream::move_next();

								if(cc65c::assembler::stream::match(CHARACTER_SYMBOL, CHARACTER_COMMENT)) {
									break;
								}
							}
						}
					} else {

						for(;;) {

							if(!cc65c::assembler::stream::has_next()
									|| cc65c::assembler::stream::match(CHARACTER_SPACE, CHARACTER_NEWLINE)) {
								break;
							}

							cc65c::assembler::stream::move_next();
						}
					}

					if(cc65c::assembler::stream::has_next()) {
						cc65c::assembler::stream::move_next();
					}

					skip_whitespace();
				}
			}

			TRACE_EXIT();
		}

		void 
		lexer::skip_whitespace(void)
		{
			TRACE_ENTRY();

			for(;;) {

				if(!cc65c::assembler::stream::has_next()
						|| !cc65c::assembler::stream::match(CHARACTER_SPACE)) {
					break;
				}

				cc65c::assembler::stream::move_next();
			}

			skip_comment();

			TRACE_EXIT();
		}

		std::string 
		lexer::to_string(
			__in_opt bool verbose,
			__in_opt cc65c::core::stream_t index,
			__in_opt cc65c::core::stream_t offset
			)
		{
			std::string result;

			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_lexer_mutex);

			result = cc65c::assembler::lexer::as_string(*this, verbose, index, offset);

			TRACE_EXIT();
			return result;
		}

		cc65c::assembler::token &
		lexer::token(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_lexer_mutex);

			cc65c::assembler::token &result = m_token.at(m_token_position);

			TRACE_EXIT();
			return result;
		}

		cc65c::assembler::token &
		lexer::token(
			__inout cc65c::assembler::token_t &type
			)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_stream_mutex);

			cc65c::assembler::token &result = token();
			type = result.type();

			TRACE_EXIT();
			return result;
		}

		std::vector<cc65c::assembler::token>::const_iterator 
		lexer::token_begin(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_stream_mutex);

			std::vector<cc65c::assembler::token>::const_iterator result = m_token.begin();

			TRACE_EXIT();
			return result;
		}

		std::vector<cc65c::assembler::token>::const_iterator 
		lexer::token_end(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_stream_mutex);

			std::vector<cc65c::assembler::token>::const_iterator result = m_token.end();

			TRACE_EXIT();
			return result;
		}
	}
}
