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

#include "../../include/assembler/stream.h"
#include "stream_type.h"
#include "../../include/core/trace.h"

namespace cc65c {

	namespace assembler {

		#define CHARACTER_STREAM_CURSOR '^'
		#define CHARACTER_STREAM_CURSOR_EXTENSION '~'
		#define CHARACTER_EOS '\0'
		#define CHARACTER_FILL ' '
		#define CHARACTER_NEWLINE '\n'

		#define STREAM_CURSOR_COLOR "\033[32m"
		#define STREAM_CURSOR_COLOR_RESET "\033[0m"

		std::string CHARACTER_STR[] = {
			"ALPHA", "DIGIT", "", "END", "SPACE", "SYMBOL",
			};

		#define CHARACTER_STRING(_TYPE_) \
			((_TYPE_) > CHARACTER_MAX ? STRING_UNKNOWN : STRING_CHECK(CHARACTER_STR[_TYPE_]))

		stream::stream(void) :
			m_column(0),
			m_position(0),
			m_row(0),
			m_type(CHARACTER_END)
		{
			TRACE_ENTRY();

			clear();

			TRACE_EXIT();
		}

		stream::stream(
			__in const std::string &input,
			__in_opt bool is_file
			) :
				m_column(0),
				m_position(0),
				m_row(0),
				m_type(CHARACTER_END)
		{
			TRACE_ENTRY();

			set(input, is_file);

			TRACE_EXIT();
		}

		stream::stream(
			__in const stream &other
			) :
				cc65c::core::stream(other),
				m_column(other.m_column),
				m_line(other.m_line),
				m_path(other.m_path),
				m_position(other.m_position),
				m_row(other.m_row),
				m_type(other.m_type)
		{
			TRACE_ENTRY();
			TRACE_EXIT();
		}

		stream::~stream(void)
		{
			TRACE_ENTRY();
			TRACE_EXIT();
		}

		stream &
		stream::operator=(
			__in const stream &other
			)
		{
			TRACE_ENTRY();

			if(this != &other) {
				cc65c::core::stream::operator=(other);
				m_column = other.m_column;
				m_line = other.m_line;
				m_path = other.m_path;
				m_position = other.m_position;
				m_row = other.m_row;
				m_type = other.m_type;
			}

			TRACE_EXIT_FORMAT("Result=%p", this);
			return *this;
		}

		std::string 
		stream::as_exception(
			__in cc65c::core::stream_t row,
			__in cc65c::core::stream_t column,
			__in_opt bool verbose,
			__in_opt size_t tabs
			)
		{
			size_t tab_iter;
			std::string buffer;
			std::stringstream result;
			cc65c::core::stream_t iter = 0;

			TRACE_ENTRY();

			for(tab_iter = 0; tab_iter < tabs; ++tab_iter) {
				result << "\t";
			}

			buffer = find(row)->second;
			result << buffer;

			if(verbose) {
				result << " ("; 

				if(!m_path.empty()) {
					result << m_path << "@";
				}

				result << m_row << ")" << std::endl;

				for(tab_iter = 0; tab_iter < tabs; ++tab_iter) {
					result << "\t";
				}

#ifdef TRACE_COLOR
				result << STREAM_CURSOR_COLOR;
#else
				result << " ";
#endif // TRACE_COLOR

				for(; iter < buffer.size(); ++iter) {

					if(iter == column) {
						result << CHARACTER_STREAM_CURSOR;
						break;
					} else {
						result << CHARACTER_STREAM_CURSOR_EXTENSION;
					}
				}

				if(iter == buffer.size()) {
					result << CHARACTER_STREAM_CURSOR;
				}

#ifdef TRACE_COLOR
				result << STREAM_CURSOR_COLOR_RESET;
#endif // TRACE_COLOR
			}

			TRACE_EXIT();
			return result.str();
		}

		std::string 
		stream::as_string(
			__in const stream &reference,
			__in_opt bool verbose,
			__in_opt cc65c::core::stream_t index,
			__in_opt cc65c::core::stream_t offset
			)
		{
			char ch;
			size_t ch_iter = 0;
			std::string buffer;
			std::stringstream result;
			cc65c::assembler::character_t type;
			std::map<cc65c::core::stream_t, std::string>::const_iterator iter;

			TRACE_ENTRY();

			type = reference.m_type;
			ch = reference.m_stream.at(reference.m_position);
			result << "[" << CHARACTER_STRING(type);

			if(type != CHARACTER_END) {
				result << ", " << SCALAR_AS_HEX(uint8_t, ch)
					<< "(\'" << (std::isprint(ch) ? ch : CHARACTER_FILL) << "\'" << ")";
			}

			result << "]";

			iter = reference.m_line.find(reference.m_row);
			if(iter != reference.m_line.end()) {
				buffer = iter->second;
			}

			if(!buffer.empty()) {
				result << " \"";

				for(; ch_iter < buffer.size(); ++ch_iter) {

#ifdef TRACE_COLOR
					if(ch_iter == reference.m_column) {
						result << STREAM_CURSOR_SET;
					}
#endif // TRACE_COLOR

					result << buffer.at(ch_iter);

#ifdef TRACE_COLOR
					if(ch_iter == reference.m_column) {
						result << STREAM_CURSOR_RESET;
					}
#endif // TRACE_COLOR
				}

				result << "\"";
			}

			if(!reference.m_path.empty()) {
				result << " Path[" << reference.m_path.size() << "]="
					<< "\"" << STRING_CHECK(reference.m_path) << "\",";
			}

			result << " Line=" << reference.m_row 
				<< ", Column=" << reference.m_column
				<< ", " << cc65c::core::stream::as_string(reference, verbose, index, offset, 
						reference.m_position);

			TRACE_EXIT();
			return result.str();
		}

		char 
		stream::character(void)
		{
			char result;

			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_stream_mutex);

			result = cc65c::core::stream::at(m_position);

			TRACE_EXIT_FORMAT("Result=%x(\'%c\')", result, std::isprint(result) ? result : CHARACTER_FILL);
			return result;
		}

		char 
		stream::character(
			__inout cc65c::assembler::character_t &type
			)
		{
			char result;

			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_stream_mutex);

			result = character();
			type = m_type;

			TRACE_EXIT_FORMAT("Result=[%x(%s)] %x(\'%c\')", type, CHARACTER_STRING(type),
				result, std::isprint(result) ? result : CHARACTER_FILL);
			return result;
		}

		void 
		stream::clear(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_stream_mutex);

			cc65c::core::stream::resize(1, CHARACTER_EOS);
			m_column = 0;
			m_line.clear();
			m_path.clear();
			m_position = 0;
			m_row = 0;
			m_type = CHARACTER_END;

			TRACE_EXIT();
		}

		cc65c::core::stream_t 
		stream::column(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_stream_mutex);

			TRACE_EXIT_FORMAT("Result=%u", m_column);
			return m_column;
		}

		void 
		stream::enumerate_row(void)
		{
			char ch;
			std::string buffer;
			cc65c::core::stream_t position = m_position;
			std::map<cc65c::core::stream_t, std::string>::iterator iter;

			TRACE_ENTRY();

			for(;; ++position) {

				ch = cc65c::core::stream::at(position);
				buffer += (std::isprint(ch) ? ch : CHARACTER_FILL);

				if((ch == CHARACTER_EOS) || (ch == CHARACTER_NEWLINE)) {
					break;
				}
			}

			iter = m_line.find(m_row);
			if(iter == m_line.end()) {
				m_line.insert(std::make_pair(m_row, buffer));	
			} else {
				iter->second = buffer;
			}

			TRACE_EXIT();
		}

		cc65c::assembler::character_t 
		stream::enumerate_type(
			__in char input
			)
		{
			cc65c::assembler::character_t result;

			TRACE_ENTRY();

			if(input == CHARACTER_EOS) {
				result = CHARACTER_END;
			} else if(std::isalpha(input)) {
				result = CHARACTER_ALPHA;
			} else if(std::isdigit(input)) {
				result = CHARACTER_DIGIT;
			} else if(std::isspace(input)) {
				result = CHARACTER_SPACE;
			} else {
				result = CHARACTER_SYMBOL;
			}

			TRACE_EXIT_FORMAT("Result=%x(%s)", result, CHARACTER_STRING(result));
			return result;
		}

		std::map<cc65c::core::stream_t, std::string>::iterator 
		stream::find(
			__in cc65c::core::stream_t row
			)
		{
			std::map<cc65c::core::stream_t, std::string>::iterator result;

			TRACE_ENTRY();

			result = m_line.find(row);
			if(result == m_line.end()) {
				THROW_CC65C_ASSEMBLER_STREAM_EXCEPTION_FORMAT(CC65C_ASSEMBLER_STREAM_EXCEPTION_NOT_FOUND,
					"Row=%u", row);
			}

			TRACE_EXIT();
			return result;
		}

		bool 
		stream::has_next(void)
		{
			bool result;

			TRACE_ENTRY();

			result = (character() != CHARACTER_EOS);

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		bool 
		stream::has_path(void)
		{
			bool result;

			TRACE_ENTRY();

			result = !m_path.empty();

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		bool 
		stream::has_previous(void)
		{
			bool result;

			TRACE_ENTRY();

			result = (m_position > 0);

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		const std::string &
		stream::line(void)
		{
			std::map<cc65c::core::stream_t, std::string>::iterator iter;

			TRACE_ENTRY();

			iter = find(m_row);

			TRACE_EXIT_FORMAT("Result[%u]=\"%s\"", iter->second.size(), STRING_CHECK(iter->second));
			return iter->second;
		}

		bool 
		stream::match(
			__in cc65c::assembler::character_t type
			)
		{
			bool result;

			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_stream_mutex);

			if(type == CHARACTER_DIGIT_HEXIDECIMAL) {
				result = std::isxdigit(cc65c::core::stream::at(m_position));
			} else {
				result = (type == m_type);
			}

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		bool 
		stream::match(
			__in char value
			)
		{
			bool result;

			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_stream_mutex);

			result = (value == cc65c::core::stream::at(m_position));

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		bool 
		stream::match(
			__in cc65c::assembler::character_t type,
			__in char value
			)
		{
			bool result;

			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_stream_mutex);

			if(type == CHARACTER_DIGIT_HEXIDECIMAL) {
				result = (std::isxdigit(cc65c::core::stream::at(m_position)) 
					&& (value == cc65c::core::stream::at(m_position)));
			} else {
				result = ((type == m_type) && (value == cc65c::core::stream::at(m_position)));
			}

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		void 
		stream::metadata(
			__inout cc65c::core::stream_t &row,
			__inout cc65c::core::stream_t &column
			)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_stream_mutex);

			row = m_row;
			column = m_column;

			TRACE_EXIT();
		}

		char 
		stream::move_next(void)
		{
			char result;

			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_stream_mutex);

			if(!has_next()) {
				THROW_CC65C_ASSEMBLER_STREAM_EXCEPTION_FORMAT(CC65C_ASSEMBLER_STREAM_EXCEPTION_NO_NEXT,
					"Position=%u", m_position);
			}

			result = character();
			++m_position;

			if(result == CHARACTER_NEWLINE) {
				m_column = 0;
				++m_row;
				enumerate_row();
			} else {
				++m_column;
			}

			result = character();
			m_type = cc65c::assembler::stream::enumerate_type(result);

			TRACE_EXIT_FORMAT("Result=[%x(%s)] %x(\'%c\')", m_type, CHARACTER_STRING(m_type),
				result, std::isprint(result) ? result : CHARACTER_FILL);
			return result;
		}

		char 
		stream::move_next(
			__inout cc65c::assembler::character_t &type
			)
		{
			char result;

			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_stream_mutex);

			result = move_next();
			type = m_type;

			TRACE_EXIT_FORMAT("Result=[%x(%s)] %x(\'%c\')", type, CHARACTER_STRING(type),
				result, std::isprint(result) ? result : CHARACTER_FILL);
			return result;
		}

		char 
		stream::move_previous(void)
		{
			char result;

			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_stream_mutex);

			if(!has_previous()) {
				THROW_CC65C_ASSEMBLER_STREAM_EXCEPTION_FORMAT(CC65C_ASSEMBLER_STREAM_EXCEPTION_NO_PREVIOUS,
					"Position=%u", m_position);
			}

			--m_position;
			result = character();
			if(result == CHARACTER_NEWLINE) {
				--m_row;
				m_column = line().size();
			} else {
				--m_column;
			}

			m_type = cc65c::assembler::stream::enumerate_type(result);

			TRACE_EXIT_FORMAT("Result=[%x(%s)] %x(\'%c\')", m_type, CHARACTER_STRING(m_type),
				result, std::isprint(result) ? result : CHARACTER_FILL);
			return result;
		}

		char 
		stream::move_previous(
			__inout cc65c::assembler::character_t &type
			)
		{
			char result;

			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_stream_mutex);

			result = move_previous();
			type = m_type;

			TRACE_EXIT_FORMAT("Result=[%x(%s)] %x(\'%c\')", type, CHARACTER_STRING(type),
				result, std::isprint(result) ? result : CHARACTER_FILL);
			return result;
		}

		const std::string &
		stream::path(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_stream_mutex);

			TRACE_EXIT_FORMAT("Result[%u]=\"%s\"", m_path.size(), STRING_CHECK(m_path));
			return m_path;
		}

		void 
		stream::reset(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_stream_mutex);

			m_column = 0;
			m_position = 0;
			m_row = 0;
			m_type = cc65c::assembler::stream::enumerate_type(character());

			TRACE_EXIT();
		}

		cc65c::core::stream_t 
		stream::row(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_stream_mutex);

			TRACE_EXIT_FORMAT("Result=%u", m_row);
			return m_row;
		}

		void 
		stream::set(
			__in const std::string &input,
			__in_opt bool is_file
			)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_stream_mutex);

			clear();

			if(is_file) {
				m_path = input;
			}

			cc65c::core::stream::set(input, is_file);
			cc65c::core::stream::resize(cc65c::core::stream::size() + 1, CHARACTER_EOS);
			m_type = cc65c::assembler::stream::enumerate_type(character());
			enumerate_row();

			TRACE_EXIT();
		}

		std::string 
		stream::to_string(
			__in_opt bool verbose,
			__in_opt cc65c::core::stream_t index,
			__in_opt cc65c::core::stream_t offset
			)
		{
			std::string result;

			TRACE_ENTRY();

			result = cc65c::assembler::stream::as_string(*this, verbose, index, offset);

			TRACE_EXIT();
			return result;
		}

		cc65c::assembler::character_t 
		stream::type(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::recursive_mutex> lock(m_stream_mutex);

			TRACE_EXIT_FORMAT("Result=%x(%s)", m_type, CHARACTER_STRING(m_type));
			return m_type;
		}
	}
}
