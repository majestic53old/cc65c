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

#ifndef CC65C_CORE_STREAM_H_
#define CC65C_CORE_STREAM_H_

#include <mutex>
#include <vector>
#include "../define.h"

namespace cc65c {

	namespace core {

		#define STREAM_INDEX_MAX UINT32_MAX
		#define STREAM_CURSOR_RESET "\033[24m"
		#define STREAM_CURSOR_SET "\033[4m"

		typedef uint32_t stream_t;

		class stream {

			public:

				stream(
					__in_opt const std::vector<uint8_t> &input = std::vector<uint8_t>()
					);

				stream(
					__in const std::string &input,
					__in_opt bool is_file = true
					);

				stream(
					__in stream_t size,
					__in_opt uint8_t value = 0
					);

				stream(
					__in const stream &other
					);

				virtual stream &operator=(
					__in const stream &other
					);

				bool operator==(
					__in const stream &other
					);

				bool operator!=(
					__in const stream &other
					);

				uint8_t &operator[](
					__in stream_t index
					);

				void add(
					__in const std::vector<uint8_t> &input
					);

				static std::string as_string(
					__in const stream &reference,
					__in_opt bool verbose = false,
					__in_opt stream_t index = 0,
					__in_opt stream_t offset = 0,
					__in_opt stream_t cursor = 0
					);

				uint8_t &at(
					__in stream_t index
					);

				void clear(void);

				stream_t copy(
					__in const std::vector<uint8_t> &input,
					__in stream_t index
					);

				void insert(
					__in const std::vector<uint8_t> &input,
					__in stream_t index
					);

				void resize(
					__in stream_t size,
					__in_opt uint8_t value = 0
					);

				void set(
					__in const std::string &input,
					__in_opt bool is_file = true
					);

				stream_t size(void);

				stream_t read(
					__in const std::string &path
					);

				stream_t remove(
					__in stream_t index,
					__in_opt stream_t offset = 0
					);

				virtual std::string to_string(
					__in_opt bool verbose = false,
					__in_opt stream_t index = 0,
					__in_opt stream_t offset = 0
					);

				void write(
					__in const std::string &path
					);

			protected:

				static void append_string(
					__in std::stringstream &stream,
					__inout std::string &buffer,
					__inout stream_t &index,
					__inout bool &set
					);

				std::vector<uint8_t> m_stream;

				std::mutex m_mutex;
		};
	}
}

#endif // CC65C_CORE_STREAM_H_
