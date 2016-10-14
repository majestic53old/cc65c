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

#include "../../include/test/suite.h"
#include "suite_type.h"
#include "../../include/core/trace.h"

namespace cc65c {

	namespace test {

		suite::suite(
			__in const std::string &name,
			__in_opt const std::map<std::string, std::reference_wrapper<cc65c::test::fixture>> &entry
			) :
				m_name(name)
		{
			TRACE_ENTRY();

			if(name.empty()) {
				THROW_CC65C_TEST_SUITE_EXCEPTION(CC65C_TEST_SUITE_EXCEPTION_INVALID);
			}

			add(entry);

			TRACE_EXIT();
		}

		suite::suite(
			__in const suite &other
			) :
				m_fixture(other.m_fixture),
				m_name(other.m_name)
		{
			TRACE_ENTRY();
			TRACE_EXIT();
		}

		suite::~suite(void)
		{
			TRACE_ENTRY();
			TRACE_EXIT();
		}

		suite &
		suite::operator=(
			__in const suite &other
			)
		{
			TRACE_ENTRY();

			if(this != &other) {
				m_fixture = other.m_fixture;
				m_name = other.m_name;
			}

			TRACE_EXIT_FORMAT("Result=%p", this);
			return *this;
		}

		void 
		suite::add(
			__in cc65c::test::fixture &entry
			)
		{
			std::string name;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_fixture_mutex);

			name = cc65c::test::fixture::as_name(entry);
			if(m_fixture.find(name) != m_fixture.end()) {
				THROW_CC65C_TEST_SUITE_EXCEPTION_FORMAT(CC65C_TEST_SUITE_EXCEPTION_DUPLICATE,
					"Name[%u]=\"%s\"", name.size(), STRING_CHECK(name));
			}

			m_fixture.insert(std::make_pair(name, std::reference_wrapper<cc65c::test::fixture>(entry)));

			TRACE_EXIT();
		}

		void 
		suite::add(
			__in const std::map<std::string, std::reference_wrapper<cc65c::test::fixture>> &entry
			)
		{
			std::string name;
			std::map<std::string, std::reference_wrapper<cc65c::test::fixture>>::const_iterator iter;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_fixture_mutex);

			for(iter = entry.begin(); iter != entry.end(); ++iter) {

				name = iter->first;
				if(m_fixture.find(name) != m_fixture.end()) {
					THROW_CC65C_TEST_SUITE_EXCEPTION_FORMAT(CC65C_TEST_SUITE_EXCEPTION_DUPLICATE,
						"Name[%u]=\"%s\"", name.size(), STRING_CHECK(name));
				}

				m_fixture.insert(*iter);
			}

			TRACE_EXIT();
		}

		std::string 
		suite::as_string(
			__in const suite &reference,
			__in_opt bool verbose
			)
		{
			std::stringstream result;
			std::map<std::string, std::reference_wrapper<cc65c::test::fixture>>::const_iterator iter;

			TRACE_ENTRY();

			result << "[" << STRING_CHECK(reference.m_name) << "]" 
				<< " Fixture[" << reference.m_fixture.size() << "]";

			if(verbose) {
				result << "={";

				for(iter = reference.m_fixture.begin(); iter != reference.m_fixture.end(); ++iter) {

					if(iter != reference.m_fixture.begin()) {
						result << ", ";
					}

					result << "{" << iter->second.get().to_string(true) << "}";
				}

				result << "}";
			}

			TRACE_EXIT();
			return result.str();
		}

		void 
		suite::clear(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_fixture_mutex);

			m_fixture.clear();

			TRACE_EXIT();
		}

		bool 
		suite::contains(
			__in const std::string &name
			)
		{
			bool result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_fixture_mutex);

			result = (m_fixture.find(name) != m_fixture.end());

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		std::map<std::string, std::reference_wrapper<cc65c::test::fixture>>::iterator 
		suite::find(
			__in const std::string &name
			)
		{
			std::map<std::string, std::reference_wrapper<cc65c::test::fixture>>::iterator result;

			TRACE_ENTRY();

			result = m_fixture.find(name);
			if(result == m_fixture.end()) {
				THROW_CC65C_TEST_SUITE_EXCEPTION_FORMAT(CC65C_TEST_SUITE_EXCEPTION_NOT_FOUND,
					"Name[%u]=\"%s\"", name.size(), STRING_CHECK(name));
			}

			TRACE_EXIT();
			return result;
		}

		const std::string &
		suite::name(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_fixture_mutex);

			TRACE_EXIT_FORMAT("Result[%u]=\"%s\"", m_name.size(), STRING_CHECK(m_name));
			return m_name;
		}

		void 
		suite::remove(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_fixture_mutex);

			m_fixture.clear();

			TRACE_EXIT();
		}

		void 
		suite::remove(
			__in const std::string &name
			)
		{
			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_fixture_mutex);

			m_fixture.erase(find(name));

			TRACE_EXIT();
		}

		void 
		suite::reset(void)
		{
			std::map<std::string, std::reference_wrapper<cc65c::test::fixture>>::iterator iter;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_fixture_mutex);

			for(iter = m_fixture.begin(); iter != m_fixture.end(); ++iter) {
				iter->second.get().reset();
			}

			TRACE_EXIT();
		}

		void 
		suite::reset(
			__in const std::string &name
			)
		{
			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_fixture_mutex);

			find(name)->second.get().reset();

			TRACE_EXIT();
		}

		cc65c::test::status_t 
		suite::run(
			__inout std::map<std::string, cc65c::test::status_t> &output
			)
		{
			cc65c::test::status_t result = STATUS_SUCCESS, status;
			std::map<std::string, std::reference_wrapper<cc65c::test::fixture>>::iterator iter;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_fixture_mutex);

			output.clear();

			for(iter = m_fixture.begin(); iter != m_fixture.end(); ++iter) {

				status = iter->second.get().run();
				if((status == STATUS_FAILURE) 
						|| ((result == STATUS_SUCCESS) && (status == STATUS_INCONCLUSIVE))) {
					result = status;
				}

				output.insert(std::make_pair(iter->first, status));
			}

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		cc65c::test::status_t 
		suite::run(
			__in const std::string &name
			)
		{
			cc65c::test::status_t result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_fixture_mutex);

			result = find(name)->second.get().run();

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		cc65c::test::status_t 
		suite::status(
			__inout std::map<std::string, cc65c::test::status_t> &output
			)
		{
			cc65c::test::status_t result = STATUS_SUCCESS, status;
			std::map<std::string, std::reference_wrapper<cc65c::test::fixture>>::iterator iter;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_fixture_mutex);

			output.clear();

			for(iter = m_fixture.begin(); iter != m_fixture.end(); ++iter) {

				status = iter->second.get().status();
				if((status == STATUS_FAILURE) 
						|| ((result == STATUS_SUCCESS) && (status == STATUS_INCONCLUSIVE))) {
					result = status;
				}

				output.insert(std::make_pair(iter->first, status));
			}

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		cc65c::test::status_t 
		suite::status(
			__in const std::string &name
			)
		{
			cc65c::test::status_t result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_fixture_mutex);

			result = find(name)->second.get().status();

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		size_t 
		suite::size(void)
		{
			size_t result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_fixture_mutex);

			result = m_fixture.size();

			TRACE_EXIT_FORMAT("Result=%u", result);
			return result;
		}

		std::string 
		suite::to_string(
			__in_opt bool verbose
			)
		{
			std::string result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_fixture_mutex);

			result = cc65c::test::suite::as_string(*this, verbose);

			TRACE_EXIT();
			return result;
		}
	}
}
