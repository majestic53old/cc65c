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

#include "../include/runtime.h"
#include "runtime_type.h"

namespace cc65c {

	runtime::runtime(void) :
		m_symbol(cc65c::assembler::symbol::acquire()),
		m_trace(cc65c::core::trace::acquire()),
		m_uuid(cc65c::core::uuid::acquire())
	{
		m_trace.initialize();

		TRACE_ENTRY();
		TRACE_EXIT();
	}

	runtime::~runtime(void)
	{
		TRACE_ENTRY();

		uninitialize();

		TRACE_EXIT();

		m_trace.uninitialize();
	}

	std::string 
	runtime::as_string(
		__in const runtime &reference,
		__in_opt bool verbose
		)
	{
		std::stringstream result;

		TRACE_ENTRY();

		result << CC65C_RUNTIME_HEADER
			<< " State=" << cc65c::core::singleton<cc65c::runtime>::as_string(reference, verbose);

		if(verbose) {
			result << ", Instance=" << SCALAR_AS_HEX(uintptr_t, &reference);
		}

		TRACE_EXIT();
		return result.str();
	}

	void 
	runtime::initialize(void)
	{
		TRACE_ENTRY();

		if(cc65c::core::singleton<cc65c::runtime>::is_initialized()) {
			THROW_CC65C_RUNTIME_EXCEPTION(CC65C_RUNTIME_EXCEPTION_INITIALIZED);
		}
		
		setup();
		cc65c::core::singleton<cc65c::runtime>::set_initialized(true);
		TRACE_INFORMATION("Runtime initialized");

		TRACE_EXIT();
	}

	void 
	runtime::setup(void)
	{
		TRACE_ENTRY();

		m_uuid.initialize();
		m_symbol.initialize();

		// TODO

		TRACE_EXIT();
	}

	void 
	runtime::teardown(void)
	{
		TRACE_ENTRY();

		// TODO

		m_symbol.uninitialize();
		m_uuid.uninitialize();

		TRACE_EXIT();
	}

	std::string 
	runtime::to_string(
		__in_opt bool verbose
		)
	{
		std::string result;

		TRACE_ENTRY();

		result = cc65c::runtime::as_string(*this, verbose);

		TRACE_EXIT();
		return result;
	}

	void 
	runtime::uninitialize(void)
	{
		TRACE_ENTRY();

		if(cc65c::core::singleton<cc65c::runtime>::is_initialized()) {
			cc65c::core::singleton<cc65c::runtime>::set_initialized(false);
			teardown();
			TRACE_INFORMATION("Runtime uninitialized");
		}

		TRACE_EXIT();
	}

	std::string 
	runtime::version(
		__in_opt bool verbose
		)
	{
		std::stringstream result;

		TRACE_ENTRY();

		result << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_WEEK
			<< "." << VERSION_REVISION;

		if(verbose) {
			result << "-" << VERSION_RELEASE;
		}

		TRACE_EXIT();
		return result.str();
	}
}
