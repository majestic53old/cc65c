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

#include <tuple>
#include "../../include/runtime.h"
#include "../../include/assembler/parser.h"
#include "../../include/test/suite.h"

namespace cc65c {

	namespace test {

		#define PARSER_FIXTURE_NAME "PARSER_FIXTURE"
		#define PARSER_SUITE_NAME "PARSER_SUITE"

		enum {
			PARSER_TEST_TITLE = 0,
			PARSER_TEST_FILE,
		};

		static const std::vector<std::tuple<std::string, std::pair<std::string, bool>>> PARSER_TEST_SET = {
			std::make_tuple("TEST_EMPTY", std::make_pair("./test/parser/test/test_empty.asm", true)),

			// TODO: Add additional test cases

			};

		class fixture_parser :
				public cc65c::test::fixture {

			public:

				fixture_parser(void) :
					cc65c::test::fixture(PARSER_FIXTURE_NAME)
				{
					TRACE_ENTRY();
					TRACE_EXIT();
				}

				cc65c::test::status_t 
				on_setup(void)
				{
					cc65c::test::status_t result = STATUS_SUCCESS;

					TRACE_ENTRY();
					TRACE_EXIT_FORMAT("Result=%x", result);
					return result;
				}

				cc65c::test::status_t 
				on_teardown(void)
				{
					cc65c::test::status_t result = STATUS_SUCCESS;

					TRACE_ENTRY();
					TRACE_EXIT_FORMAT("Result=%x", result);
					return result;
				}

				cc65c::test::status_t 
				on_test(void)
				{
					cc65c::test::status_t result = STATUS_SUCCESS;
					std::vector<std::tuple<std::string, std::pair<std::string, bool>>>::const_iterator iter;

					TRACE_ENTRY();

					for(iter = PARSER_TEST_SET.begin(); iter != PARSER_TEST_SET.end(); ++iter) {

						result = test_input(std::get<PARSER_TEST_FILE>(*iter).first, std::get<PARSER_TEST_FILE>(*iter).second);
						if(result != STATUS_SUCCESS) {
							TRACE_ERROR_FORMAT("Test failure", "Name=%s", STRING_CHECK(std::get<PARSER_TEST_TITLE>(*iter)));
						}
					}

					TRACE_EXIT_FORMAT("Result=%x", result);
					return result;
				}

			protected:

				static cc65c::test::status_t 
				test_input(
					__in const std::string &input,
					__in bool is_file
					)
				{
					cc65c::test::status_t result = STATUS_SUCCESS;

					TRACE_ENTRY();

					try {
						cc65c::assembler::parser par(input, is_file);

						if(!par.match(cc65c::assembler::TREE_BEGIN)) {
							THROW_EXCEPTION_FORMAT("Expecting TREE_BEGIN", "%s", STRING_CHECK(par.to_string(true)));
						}

						while(par.has_next()) {
							par.move_next();

							if(!par.match(cc65c::assembler::TREE_BEGIN) 
									&& !par.match(cc65c::assembler::TREE_END)
									&& !par.match(cc65c::assembler::TREE_STATEMENT)) {
								THROW_EXCEPTION_FORMAT("Tree mismatch", "Expecting=%u(%x), Found=%s", 
									cc65c::assembler::TREE_STATEMENT, cc65c::assembler::TREE_STATEMENT, 
									STRING_CHECK(par.to_string(true)));
							}
						}

						if(!par.match(cc65c::assembler::TREE_END)) {
							THROW_EXCEPTION_FORMAT("Expecting TREE_END", "%s", STRING_CHECK(par.to_string(true)));
						}

						while(par.has_previous()) {

							if(!par.match(cc65c::assembler::TREE_BEGIN) 
									&& !par.match(cc65c::assembler::TREE_END)
									&& !par.match(cc65c::assembler::TREE_STATEMENT)) {
								THROW_EXCEPTION_FORMAT("Tree mismatch", "Expecting=%u(%x), Found=%s", 
									cc65c::assembler::TREE_STATEMENT, cc65c::assembler::TREE_STATEMENT, 
									STRING_CHECK(par.to_string(true)));
							}

							par.move_previous();
						}

						if(!par.match(cc65c::assembler::TREE_BEGIN)) {
							THROW_EXCEPTION_FORMAT("Expecting TREE_BEGIN", "%s", STRING_CHECK(par.to_string(true)));
						}
					} catch(cc65c::core::exception &exc) {
						result = STATUS_FAILURE;
					}

					TRACE_EXIT_FORMAT("Result=%x", result);
					return result;
				}
		};
	}
}

int 
main(void)
{
	cc65c::test::status_t result = cc65c::test::STATUS_INCONCLUSIVE;

	try {
		cc65c::runtime &instance = cc65c::runtime::acquire();
		instance.initialize();
		cc65c::test::suite suite(PARSER_SUITE_NAME);
		cc65c::test::fixture_parser test; 
		suite.add(test);
		result = suite.run(PARSER_FIXTURE_NAME);
		instance.uninitialize();
	} catch(cc65c::core::exception &exc) {
		TRACE_ERROR(STRING_CHECK(exc.to_string(true)));
		std::cerr << exc.to_string(true) << std::endl;
	}

	return result;
}
