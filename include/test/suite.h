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

#ifndef CC65C_TEST_SUITE_H_
#define CC65C_TEST_SUITE_H_

#include <map>
#include "fixture.h"

namespace cc65c {

	namespace test {

		class suite {

			public:

				suite(
					__in const std::string &name,
					__in_opt const std::map<std::string, std::reference_wrapper<cc65c::test::fixture>> &entry 
						= std::map<std::string, std::reference_wrapper<cc65c::test::fixture>>()
					);

				suite(
					__in const suite &other
					);

				virtual ~suite(void);

				suite &operator=(
					__in const suite &other
					);

				void add(
					__in cc65c::test::fixture &entry
					);

				void add(
					__in const std::map<std::string, std::reference_wrapper<cc65c::test::fixture>> &entry
					);

				static std::string as_string(
					__in const suite &reference,
					__in_opt bool verbose = false
					);

				bool contains(
					__in const std::string &name
					);

				const std::string &name(void);

				void remove(void);

				void remove(
					__in const std::string &name
					);

				void reset(void);

				void reset(
					__in const std::string &name
					);

				cc65c::test::status_t run(
					__inout std::map<std::string, cc65c::test::status_t> &output
					);

				cc65c::test::status_t run(
					__in const std::string &name
					);

				cc65c::test::status_t status(
					__inout std::map<std::string, cc65c::test::status_t> &output
					);

				cc65c::test::status_t status(
					__in const std::string &name
					);

				size_t size(void);

				virtual std::string to_string(
					__in_opt bool verbose = false
					);

			protected:

				void clear(void);

				std::map<std::string, std::reference_wrapper<cc65c::test::fixture>>::iterator find(
					__in const std::string &name
					);

				std::map<std::string, std::reference_wrapper<cc65c::test::fixture>> m_fixture;

				std::mutex m_fixture_mutex;

				std::string m_name;
		};
	}
}

#endif // CC65C_TEST_SUITE_H_
