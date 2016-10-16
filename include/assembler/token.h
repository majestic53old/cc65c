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

#ifndef CC65C_ASSEMBLER_TOKEN_H_
#define CC65C_ASSEMBLER_TOKEN_H_

#include "../core/object.h"
#include "define.h"
#include "stream.h"

namespace cc65c {

	namespace assembler {

		enum {
			TOKEN_METADATA_COLUMN = 0,
			TOKEN_METADATA_KEY,
			TOKEN_METADATA_ROW,
			TOKEN_METADATA_SUBTYPE,
			TOKEN_METADATA_TYPE,
			TOKEN_METADATA_VALUE,
		};

		typedef std::tuple<cc65c::core::stream_t, std::string *, cc65c::core::stream_t, uint32_t, cc65c::assembler::token_t, 
			int32_t *> token_metadata_t;

		class token :
				public cc65c::core::object {

			public:

				token(void);

				token(
					__in cc65c::assembler::token_t type,
					__in_opt uint32_t subtype = SUBTYPE_UNDEFINED,
					__in_opt cc65c::core::stream_t row = 0,
					__in_opt cc65c::core::stream_t column = 0
					);

				token(
					__in cc65c::assembler::token_t type,
					__in const std::string &key,
					__in_opt cc65c::core::stream_t row = 0,
					__in_opt cc65c::core::stream_t column = 0
					);

				token(
					__in int32_t value,
					__in_opt cc65c::core::stream_t row = 0,
					__in_opt cc65c::core::stream_t column = 0
					);

				token(
					__in cc65c::core::uuid_t id,
					__in const cc65c::assembler::token_metadata_t &metadata
					);

				token(
					__in const token &other
					);

				virtual ~token(void);

				token &operator=(
					__in const token &other
					);

				bool operator==(
					__in const token &other
					);

				bool operator!=(
					__in const token &other
					);

				static int32_t as_scalar(
					__in const std::string &input,
					__in cc65c::assembler::base_t base
					);

				static std::string as_string(
					__in const token &reference,
					__in_opt bool verbose = false
					);

				static cc65c::assembler::token_t as_type(
					__in const token &reference
					);

				cc65c::core::stream_t column(void);

				static bool determine_type(
					__in const std::string &key,
					__inout cc65c::assembler::token_t &type,
					__inout uint32_t &subtype
					);

				std::string &key(void);

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

				cc65c::core::stream_t row(void);

				void set_metadata(
					__in cc65c::core::stream_t row,
					__in cc65c::core::stream_t column
					);

				uint32_t subtype(void);

				virtual std::string to_string(
					__in_opt bool verbose = false
					);

				cc65c::assembler::token_t type(void);

				int32_t &value(void);

			protected:

				void decrement(void);

				void generate(
					__in cc65c::assembler::token_t type,
					__in_opt uint32_t subtype = SUBTYPE_UNDEFINED,
					__in_opt cc65c::core::stream_t row = 0,
					__in_opt cc65c::core::stream_t column = 0
					);

				void generate(
					__in cc65c::assembler::token_t type,
					__in const std::string &key,
					__in_opt cc65c::core::stream_t row = 0,
					__in_opt cc65c::core::stream_t column = 0
					);

				void generate(
					__in int32_t value,
					__in_opt cc65c::core::stream_t row = 0,
					__in_opt cc65c::core::stream_t column = 0
					);

				void increment(void);

				cc65c::core::stream_t m_column;

				std::string *m_key;

				cc65c::core::stream_t m_row;

				std::mutex m_token_mutex;

				uint32_t m_token_subtype;

				int32_t *m_value;
		};
	}
}

#endif // CC65C_ASSEMBLER_TOKEN_H_
