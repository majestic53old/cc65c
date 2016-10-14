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

#include "../../include/test/fixture.h"
#include "fixture_type.h"
#include "../../include/core/trace.h"

namespace cc65c {

	namespace test {

		static const std::string STATUS_STR[] = {
			"FAILURE", "INCONCLUSIVE", "SUCCESS",
			};

		#define STATUS_STRING(_TYPE_) \
			((_TYPE_) > STATUS_MAX ? STRING_UNKNOWN : STRING_CHECK(STATUS_STR[_TYPE_]))

		fixture::fixture(
			__in const std::string &name
			) :
				m_name(name),
				m_status(STATUS_INCONCLUSIVE)
		{
			TRACE_ENTRY();

			if(name.empty()) {
				THROW_CC65C_TEST_FIXTURE_EXCEPTION(CC65C_TEST_FIXTURE_EXCEPTION_INVALID);
			}

			TRACE_EXIT();
		}

		fixture::fixture(
			__in const fixture &other
			) :
				m_name(other.m_name),
				m_status(other.m_status)
		{
			TRACE_ENTRY();
			TRACE_EXIT();
		}

		fixture::~fixture(void)
		{
			TRACE_ENTRY();
			TRACE_EXIT();
		}

		fixture &
		fixture::operator=(
			__in const fixture &other
			)
		{
			TRACE_ENTRY();

			if(this != &other) {
				m_name = other.m_name;
				m_status = other.m_status;
			}

			TRACE_EXIT_FORMAT("Result=%p", this);
			return *this;
		}

		std::string 
		fixture::as_name(
			__in const fixture &reference
			)
		{
			std::string result;

			TRACE_ENTRY();

			result = reference.m_name;

			TRACE_EXIT_FORMAT("Result[%u]=\"%s\"", result.size(), STRING_CHECK(result));
			return result;
		}

		std::string 
		fixture::as_string(
			__in const fixture &reference,
			__in_opt bool verbose
			)
		{
			std::stringstream result;

			TRACE_ENTRY();

			result << STRING_CHECK(reference.m_name) 
				<< ", Status=" << reference.m_status 
				<< "(" << STATUS_STRING(reference.m_status) << ")";

			TRACE_EXIT();
			return result.str();
		}

		const std::string &
		fixture::name(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_mutex);

			TRACE_EXIT_FORMAT("Result[%u]=\"%s\"", m_name.size(), STRING_CHECK(m_name));
			return m_name;
		}

		void 
		fixture::reset(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_mutex);

			m_status = STATUS_INCONCLUSIVE;

			TRACE_EXIT();
		}

		cc65c::test::status_t 
		fixture::run(void)
		{
			cc65c::test::status_t result_setup, result_teardown = STATUS_INCONCLUSIVE, 
				result_test = STATUS_INCONCLUSIVE;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_mutex);

			result_setup = on_setup();
			if(result_setup == STATUS_SUCCESS) {
				result_test = on_test();
				result_teardown = on_teardown();
			}

			if((result_setup == STATUS_FAILURE) 
					|| (result_teardown == STATUS_FAILURE) 
					|| (result_test == STATUS_FAILURE)) {
				m_status = STATUS_FAILURE;
			} else if((result_setup == STATUS_INCONCLUSIVE) 
					|| (result_teardown == STATUS_INCONCLUSIVE) 
					|| (result_test == STATUS_INCONCLUSIVE)) {
				m_status = STATUS_INCONCLUSIVE;
			} else {
				m_status = STATUS_SUCCESS;
			}

			TRACE_EXIT_FORMAT("Result=%x(%s)", m_status, STATUS_STRING(m_status));
			return m_status;
		}

		cc65c::test::status_t 
		fixture::status(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_mutex);

			TRACE_EXIT_FORMAT("Result=%x(%s)", m_status, STATUS_STRING(m_status));
			return m_status;
		}

		std::string 
		fixture::to_string(
			__in_opt bool verbose
			)
		{
			std::string result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_mutex);

			result = cc65c::test::fixture::as_string(*this, verbose);

			TRACE_EXIT();
			return result;
		}
	}
}
