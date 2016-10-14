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

#include <fstream>
#include "../../include/core/stream.h"
#include "stream_type.h"
#include "../../include/core/trace.h"

namespace cc65c {

	namespace core {

		#define BLOCK_WIDTH 0x10
		#define BYTES_PER_KBYTE 1024
		#define CHARACTER_FILL '.'

		stream::stream(
			__in_opt const std::vector<uint8_t> &input
			) :
				m_stream(input)
		{
			TRACE_ENTRY();
			TRACE_EXIT();
		}

		stream::stream(
			__in const std::string &input,
			__in_opt bool is_file
			)
		{
			TRACE_ENTRY();

			set(input, is_file);

			TRACE_EXIT();
		}

		stream::stream(
			__in stream_t size,
			__in_opt uint8_t value
			)
		{
			TRACE_ENTRY();

			m_stream.resize(size, value);

			TRACE_EXIT();
		}

		stream::stream(
			__in const stream &other
			) :
				m_stream(other.m_stream)
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
				m_stream = other.m_stream;
			}

			TRACE_EXIT_FORMAT("Result=%p", this);
			return *this;
		}

		bool 
		stream::operator==(
			__in const stream &other
			)
		{
			bool result;

			TRACE_ENTRY();

			result = ((this == &other) || (m_stream == other.m_stream));

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		bool 
		stream::operator!=(
			__in const stream &other
			)
		{
			bool result;

			TRACE_ENTRY();

			result = ((this != &other) && (m_stream != other.m_stream));

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		uint8_t &
		stream::operator[](
			__in stream_t index
			)
		{
			TRACE_ENTRY();

			uint8_t &result = at(index);

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		void 
		stream::add(
			__in const std::vector<uint8_t> &input
			)
		{
			TRACE_ENTRY();

			insert(input, m_stream.size());

			TRACE_EXIT();
		}

		void 
		stream::append_string(
			__in std::stringstream &stream,
			__inout std::string &buffer,
			__inout stream_t &index,
			__inout bool &set
			)
		{
			stream_t iter = 0;

			TRACE_ENTRY();

			if(!buffer.empty()) {
				stream << "          ";

				if(set) {

					for(; iter < buffer.size(); ++iter) {

#ifdef TRACE_COLOR
						if(iter == index) {
							stream << STREAM_CURSOR_SET;
						}
#endif // TRACE_COLOR
						stream << buffer.at(iter);
#ifdef TRACE_COLOR
						if(iter == index) {
							stream << STREAM_CURSOR_RESET;
						}
#endif // TRACE_COLOR
					}

					index = 0;
					set = false;
				} else {
					stream << buffer;
				}

				buffer.clear();
			}

			TRACE_EXIT();
		}

		std::string 
		stream::as_string(
			__in const stream &reference,
			__in_opt bool verbose,
			__in_opt stream_t index,
			__in_opt stream_t offset,
			__in_opt stream_t cursor
			)
		{
			char ch;			
			std::string buffer;
			bool cursor_set = false;
			std::stringstream result;
			stream_t cursor_index = 0, iter, iter_max, iter_pre, iter_post;

			TRACE_ENTRY();

			result << "Size=" << (reference.m_stream.size() / (float) BYTES_PER_KBYTE) << " KB"
				<< " (" << reference.m_stream.size() << " bytes)";

			if(verbose && (index || offset)) {
				iter_pre = (index - (index % BLOCK_WIDTH));
				iter_max = std::min((stream_t) (index + offset), (stream_t) reference.m_stream.size());
				iter_post = (iter_max + (BLOCK_WIDTH - (iter_max % BLOCK_WIDTH)));
				result << " [" << SCALAR_AS_HEX(stream_t, index) << "-" << SCALAR_AS_HEX(stream_t, iter_max) << "]" 
					<< std::endl << "  ";

				for(iter = 0; iter < (sizeof(stream_t) * 2); ++iter) {
					result << " ";
				}

				for(iter = 0; iter < BLOCK_WIDTH; ++iter) {
					result << " " << SCALAR_AS_HEX(uint8_t, iter);
				}

				for(iter = iter_pre; iter < iter_post; ++iter) {

					if(!(iter % BLOCK_WIDTH)) {
						append_string(result, buffer, cursor_index, cursor_set);
						result << std::endl << SCALAR_AS_HEX(stream_t, iter) << " |";
					}

					if((iter < index) 
							|| (iter > iter_max) 
							|| (iter >= reference.m_stream.size())) {
						result << " --";
						buffer += CHARACTER_FILL;
					} else {
						result << " ";

#ifdef TRACE_COLOR
						if(iter == cursor) {
							result << STREAM_CURSOR_SET;
							cursor_index = (cursor % BLOCK_WIDTH);
							cursor_set = true;
						}
#endif // TRACE_COLOR
						result << SCALAR_AS_HEX(uint8_t, reference.m_stream.at(iter));

#ifdef TRACE_COLOR
						if(iter == cursor) {
							result << STREAM_CURSOR_RESET;
						}
#endif // TRACE_COLOR

						ch = reference.m_stream.at(iter);
						if(std::isprint(ch) && !std::isspace(ch)) {
							buffer += ch;
						} else {
							buffer += CHARACTER_FILL;
						}
					}
				}

				append_string(result, buffer, cursor_index, cursor_set);
			}

			TRACE_EXIT();
			return result.str();
		}

		uint8_t &
		stream::at(
			__in stream_t index
			)
		{
			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_mutex);

			if(index >= m_stream.size()) {
				THROW_CC65C_CORE_STREAM_EXCEPTION_FORMAT(CC65C_CORE_STREAM_EXCEPTION_INVALID,
					"Index=%x(%u)", index);
			}

			uint8_t &result = m_stream.at(index);

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		void 
		stream::clear(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_mutex);
			m_stream.clear();

			TRACE_EXIT();
		}

		stream_t 
		stream::copy(
			__in const std::vector<uint8_t> &input,
			__in stream_t index
			)
		{
			stream_t iter = 0, result = 0;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_mutex);

			if(index >= m_stream.size()) {
				THROW_CC65C_CORE_STREAM_EXCEPTION_FORMAT(CC65C_CORE_STREAM_EXCEPTION_INVALID,
					"Index=%x(%u)", index);
			}

			for(; (iter < input.size()) && ((index + iter) < m_stream.size()); ++iter, ++result) {
				m_stream.at(index + iter) = input.at(iter);
			}

			TRACE_EXIT_FORMAT("Result=%.4f KB (%u bytes)", result / (float) BYTES_PER_KBYTE, result);
			return result;
		}

		void 
		stream::insert(
			__in const std::vector<uint8_t> &input,
			__in stream_t index
			)
		{
			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_mutex);

			if(index > m_stream.size()) {
				THROW_CC65C_CORE_STREAM_EXCEPTION_FORMAT(CC65C_CORE_STREAM_EXCEPTION_INVALID,
					"Index=%x(%u)", index);
			}

			m_stream.insert(m_stream.begin() + index, input.begin(), input.end());

			TRACE_EXIT();
		}

		stream_t 
		stream::read(
			__in const std::string &path
			)
		{
			int length;
			stream_t result = 0;
			std::ifstream file;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_mutex);

			file = std::ifstream(path.c_str(), std::ios::binary | std::ios::in);
			if(!file) {
				THROW_CC65C_CORE_STREAM_EXCEPTION_FORMAT(CC65C_CORE_STREAM_EXCEPTION_NOT_FOUND,
					"Path[%u]=\"%s\"", path.size(), STRING_CHECK(path));
			}

			file.seekg(0, std::ios::end);
			length = file.tellg();
			file.seekg(0, std::ios::beg);

			if(length > 0) {
				m_stream.resize(length, 0);
				file.read((char *) &m_stream[0], m_stream.size());
				result = length;
			}

			file.close();

			if(length < 0) {
				THROW_CC65C_CORE_STREAM_EXCEPTION_FORMAT(CC65C_CORE_STREAM_EXCEPTION_MALFORMED,
					"Path[%u]=\"%s\"", path.size(), STRING_CHECK(path));
			}

			TRACE_EXIT_FORMAT("Result=%.4f KB (%u bytes)", result / (float) BYTES_PER_KBYTE, result);
			return result;
		}

		stream_t 
		stream::remove(
			__in_opt stream_t index,
			__in_opt stream_t offset
			)
		{
			stream_t result = 0;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_mutex);

			if(index >= m_stream.size()) {
				THROW_CC65C_CORE_STREAM_EXCEPTION_FORMAT(CC65C_CORE_STREAM_EXCEPTION_INVALID,
					"Index=%x(%u)", index);
			}

			if((index + offset) >= m_stream.size()) {
				offset = (m_stream.size() - index);
			}

			m_stream.erase(m_stream.begin() + index, m_stream.begin() + index + offset - 1);
			result = m_stream.size();

			TRACE_EXIT_FORMAT("Result=%.4f KB (%u bytes)", result / (float) BYTES_PER_KBYTE, result);
			return result;
		}

		void 
		stream::resize(
			__in stream_t size,
			__in_opt uint8_t value
			)
		{
			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_mutex);
			m_stream.resize(size, value);

			TRACE_EXIT();
		}

		stream_t 
		stream::size(void)
		{
			stream_t result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_mutex);
			result = m_stream.size();

			TRACE_EXIT_FORMAT("Result=%.4f KB (%u bytes)", result / (float) BYTES_PER_KBYTE, result);
			return result;
		}

		void 
		stream::set(
			__in const std::string &input,
			__in_opt bool is_file
			)
		{
			TRACE_ENTRY();

			if(is_file) {
				read(input);
			} else {
				m_stream = std::vector<uint8_t>(input.begin(), input.end());
			}

			TRACE_EXIT();
		}

		std::string 
		stream::to_string(
			__in_opt bool verbose,
			__in_opt stream_t index,
			__in_opt stream_t offset
			)
		{
			std::string result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_mutex);
			result = cc65c::core::stream::as_string(*this, verbose, index, offset);

			TRACE_EXIT();
			return result;
		}

		void 
		stream::write(
			__in const std::string &path
			)
		{
			std::ofstream file;

			std::lock_guard<std::mutex> lock(m_mutex);

			file = std::ofstream(path.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
			if(!file) {
				THROW_CC65C_CORE_STREAM_EXCEPTION_FORMAT(CC65C_CORE_STREAM_EXCEPTION_NOT_FOUND,
					"Path[%u]=\"%s\"", path.size(), STRING_CHECK(path));
			}

			file.write((char *) &m_stream[0], m_stream.size());
			file.close();
		}
	}
}
