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

#ifndef CC65C_ASSEMBLER_LEXER_H_
#define CC65C_ASSEMBLER_LEXER_H_

#include "token.h"

namespace cc65c {

	namespace assembler {

		class lexer :
				protected cc65c::assembler::stream {

			public:

				lexer(void);

				lexer(
					__in const std::string &input,
					__in_opt bool is_file = true
					);

				lexer(
					__in const lexer &other
					);

				virtual ~lexer(void);

				lexer &operator=(
					__in const lexer &other
					);

				virtual std::string as_exception(
					__in_opt bool verbose = false,
					__in_opt size_t tabs = 0					
					);

				std::string as_exception(
					__in cc65c::core::stream_t row,
					__in cc65c::core::stream_t column,
					__in_opt bool verbose = false,
					__in_opt size_t tabs = 0					
					);

				static std::string as_string(
					__in const lexer &reference,
					__in_opt bool verbose = false,
					__in_opt cc65c::core::stream_t index = 0,
					__in_opt cc65c::core::stream_t offset = 0
					);

				virtual void clear(void);

				virtual size_t enumerate(void);

				bool has_next(void);

				bool has_previous(void);

				bool match(
					__in cc65c::assembler::token_t type,
					__in_opt uint32_t subtype = SUBTYPE_UNDEFINED
					);

				bool match(
					__in cc65c::assembler::token_t type,
					__in const std::string &key
					);

				bool match(
					__in int32_t value
					);

				cc65c::assembler::token &move_next(void);

				cc65c::assembler::token &move_next(
					__inout cc65c::assembler::token_t &type
					);

				cc65c::assembler::token &move_previous(void);

				cc65c::assembler::token &move_previous(
					__inout cc65c::assembler::token_t &type
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

				cc65c::assembler::token &token(void);

				cc65c::assembler::token &token(
					__inout cc65c::assembler::token_t &type
					);

				std::vector<cc65c::assembler::token>::const_iterator token_begin(void);

				std::vector<cc65c::assembler::token>::const_iterator token_end(void);

			protected:

				void add_token(
					__in cc65c::assembler::token_t type,
					__in_opt uint32_t subtype = SUBTYPE_UNDEFINED,
					__in_opt cc65c::core::stream_t row = 0,
					__in_opt cc65c::core::stream_t column = 0
					);

				void add_token(
					__in cc65c::assembler::token_t type,
					__in const std::string &key,
					__in_opt cc65c::core::stream_t row = 0,
					__in_opt cc65c::core::stream_t column = 0
					);

				void add_token(
					__in int32_t value,
					__in_opt cc65c::core::stream_t row = 0,
					__in_opt cc65c::core::stream_t column = 0
					);

				void enumerate_token(void);

				void enumerate_token_alpha(void);

				void enumerate_token_digit(void);

				std::string enumerate_token_literal_character(void);

				std::string enumerate_token_literal_character_escape(void);

				std::string enumerate_token_literal_string(void);

				int32_t enumerate_token_scalar_binary(void);

				int32_t enumerate_token_scalar_decimal(void);

				int32_t enumerate_token_scalar_hexidecimal(void);

				int32_t enumerate_token_scalar_octal(void);

				void enumerate_token_symbol(void);

				void skip_comment(void);

				void skip_whitespace(void);

				std::recursive_mutex m_lexer_mutex;

				std::vector<cc65c::assembler::token> m_token;

				size_t m_token_position;
		};
	}
}

#endif // CC65C_ASSEMBLER_LEXER_H_
