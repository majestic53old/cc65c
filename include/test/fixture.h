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

#ifndef CC65C_TEST_FIXTURE_H_
#define CC65C_TEST_FIXTURE_H_

#include <mutex>
#include "../define.h"

namespace cc65c {

	namespace test {

		typedef enum {
			STATUS_FAILURE = 0,
			STATUS_INCONCLUSIVE,
			STATUS_SUCCESS,
		} status_t;

		#define STATUS_MAX STATUS_SUCCESS

		class fixture {

			public:

				fixture(
					__in const std::string &name
					);

				fixture(
					__in const fixture &other
					);

				virtual ~fixture(void);

				fixture &operator=(
					__in const fixture &other
					);

				static std::string as_name(
					__in const fixture &reference
					);

				static std::string as_string(
					__in const fixture &reference,
					__in_opt bool verbose = false
					);

				const std::string &name(void);

				virtual cc65c::test::status_t on_setup(void) = 0;

				virtual cc65c::test::status_t on_teardown(void) = 0;

				virtual cc65c::test::status_t on_test(void) = 0;

				virtual void reset(void);

				cc65c::test::status_t run(void);

				cc65c::test::status_t status(void);

				virtual std::string to_string(
					__in_opt bool verbose = false
					);

			protected:

				std::mutex m_mutex;

				std::string m_name;

				cc65c::test::status_t m_status;
		};
	}
}

#endif // CC65C_TEST_FIXTURE_H_
