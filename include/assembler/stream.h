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

#ifndef CC65C_ASSEMBLER_STREAM_H_
#define CC65C_ASSEMBLER_STREAM_H_

#include <map>
#include "../core/stream.h"

namespace cc65c {

	namespace assembler {

		typedef enum {
			CHARACTER_ALPHA = 0,
			CHARACTER_DIGIT,
			CHARACTER_DIGIT_HEXIDECIMAL,
			CHARACTER_END,
			CHARACTER_SPACE,
			CHARACTER_SYMBOL,
		} character_t;

		#define CHARACTER_MAX CHARACTER_SYMBOL

		class stream :
				protected cc65c::core::stream {

			public:

				stream(void);

				stream(
					__in const std::string &input,
					__in_opt bool is_file = true
					);

				stream(
					__in const stream &other
					);

				virtual ~stream(void);

				stream &operator=(
					__in const stream &other
					);

				std::string as_exception(
					__in cc65c::core::stream_t row,
					__in cc65c::core::stream_t column,
					__in_opt bool verbose = false,
					__in_opt size_t tabs = 0
					);

				static std::string as_string(
					__in const stream &reference,
					__in_opt bool verbose = false,
					__in_opt cc65c::core::stream_t index = 0,
					__in_opt cc65c::core::stream_t offset = 0
					);

				char character(void);

				char character(
					__inout cc65c::assembler::character_t &type
					);

				virtual void clear(void);

				cc65c::core::stream_t column(void);

				bool has_next(void);

				bool has_path(void);

				bool has_previous(void);

				const std::string &line(void);

				bool match(
					__in cc65c::assembler::character_t type
					);

				bool match(
					__in char value
					);

				bool match(
					__in cc65c::assembler::character_t type,
					__in char value
					);

				void metadata(
					__inout cc65c::core::stream_t &row,
					__inout cc65c::core::stream_t &column
					);

				char move_next(void);

				char move_next(
					__inout cc65c::assembler::character_t &type
					);

				char move_previous(void);

				char move_previous(
					__inout cc65c::assembler::character_t &type
					);

				const std::string &path(void);

				virtual void reset(void);

				cc65c::core::stream_t row(void);

				virtual void set(
					__in const std::string &input,
					__in_opt bool is_file = true
					);

				virtual std::string to_string(
					__in_opt bool verbose = false,
					__in_opt cc65c::core::stream_t index = 0,
					__in_opt cc65c::core::stream_t offset = 0
					);

				cc65c::assembler::character_t type(void);

			protected:

				void enumerate_row(void);

				static cc65c::assembler::character_t enumerate_type(
					__in char input
					);

				std::map<cc65c::core::stream_t, std::string>::iterator find(
					__in cc65c::core::stream_t row
					);

				cc65c::core::stream_t m_column;				

				std::map<cc65c::core::stream_t, std::string> m_line;

				std::string m_path;

				cc65c::core::stream_t m_position;

				cc65c::core::stream_t m_row;

				std::recursive_mutex m_stream_mutex;

				cc65c::assembler::character_t m_type;
		};
	}
}

#endif // CC65C_ASSEMBLER_STREAM_H_
