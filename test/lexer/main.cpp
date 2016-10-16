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
#include "../../include/assembler/lexer.h"
#include "../../include/test/suite.h"

namespace cc65c {

	namespace test {

		#define LEXER_FIXTURE_NAME "LEXER_FIXTURE"
		#define LEXER_SUITE_NAME "LEXER_SUITE"

		enum {
			LEXER_TEST_TITLE = 0,
			LEXER_TEST_FILE,
			LEXER_TEST_EXPECTED,
		};

		static const std::vector<std::tuple<std::string, std::pair<std::string, bool>, cc65c::assembler::token_t>> LEXER_TEST_SET = {
			std::make_tuple("TEST_COMMENT", std::make_pair("./test/lexer/test/test_comment.asm", true), cc65c::assembler::TOKEN_END),
			std::make_tuple("TEST_EMPTY", std::make_pair("./test/lexer/test/test_empty.asm", true), cc65c::assembler::TOKEN_END),
			std::make_tuple("TEST_IDENTIFIER", std::make_pair("./test/lexer/test/test_identifier.asm", true), 
				cc65c::assembler::TOKEN_IDENTIFIER),
			std::make_tuple("TEST_KEYWORD_COMMAND", std::make_pair("./test/lexer/test/test_keyword_command.asm", true), 
				cc65c::assembler::TOKEN_KEYWORD_COMMAND),
			std::make_tuple("TEST_KEYWORD_CONDITION", std::make_pair("./test/lexer/test/test_keyword_condition.asm", true),
				cc65c::assembler::TOKEN_KEYWORD_CONDITION),
			std::make_tuple("TEST_KEYWORD_DEFINE", std::make_pair("./test/lexer/test/test_keyword_define.asm", true),
				cc65c::assembler::TOKEN_KEYWORD_DEFINE),
			std::make_tuple("TEST_KEYWORD_INCLUDE", std::make_pair("./test/lexer/test/test_keyword_include.asm", true),
				cc65c::assembler::TOKEN_KEYWORD_INCLUDE),
			std::make_tuple("TEST_KEYWORD_MACRO", std::make_pair("./test/lexer/test/test_keyword_macro.asm", true),
				cc65c::assembler::TOKEN_KEYWORD_MACRO),
			std::make_tuple("TEST_KEYWORD_REGISTER", std::make_pair("./test/lexer/test/test_keyword_register.asm", true),
				cc65c::assembler::TOKEN_KEYWORD_REGISTER),
			std::make_tuple("TEST_LABEL", std::make_pair("./test/lexer/test/test_label.asm", true), cc65c::assembler::TOKEN_LABEL),
			std::make_tuple("TEST_LITERAL_CHARACTER", std::make_pair("./test/lexer/test/test_literal_character.asm", true),
				cc65c::assembler::TOKEN_LITERAL),
			std::make_tuple("TEST_LITERAL_STRING", std::make_pair("./test/lexer/test/test_literal_string.asm", true),
				cc65c::assembler::TOKEN_LITERAL),
			std::make_tuple("TEST_SCALAR_BINARY", std::make_pair("./test/lexer/test/test_scalar_binary.asm", true), 
				cc65c::assembler::TOKEN_SCALAR),
			std::make_tuple("TEST_SCALAR_DECIMAL", std::make_pair("./test/lexer/test/test_scalar_decimal.asm", true),
				cc65c::assembler::TOKEN_SCALAR),
			std::make_tuple("TEST_SCALAR_HEXIDECIMAL", std::make_pair("./test/lexer/test/test_scalar_hexidecimal.asm", true),
				cc65c::assembler::TOKEN_SCALAR),
			std::make_tuple("TEST_SCALAR_OCTAL", std::make_pair("./test/lexer/test/test_scalar_octal.asm", true),
				cc65c::assembler::TOKEN_SCALAR),
			std::make_tuple("TEST_SYMBOL_BRACE", std::make_pair("./test/lexer/test/test_symbol_brace.asm", true),
				cc65c::assembler::TOKEN_SYMBOL_BRACE),
			std::make_tuple("TEST_SYMBOL_BRACKET", std::make_pair("./test/lexer/test/test_symbol_bracket.asm", true),
				cc65c::assembler::TOKEN_SYMBOL_BRACKET),
			std::make_tuple("TEST_SYMBOL_IMMEDIATE", std::make_pair("./test/lexer/test/test_symbol_immediate.asm", true),
				cc65c::assembler::TOKEN_SYMBOL_IMMEDIATE),
			std::make_tuple("TEST_SYMBOL_OPERATOR_ARITHMETIC", std::make_pair("./test/lexer/test/test_symbol_operator_arithmetic.asm", true),
				cc65c::assembler::TOKEN_SYMBOL_ARITHMETIC),
			std::make_tuple("TEST_SYMBOL_OPERATOR_BINARY", std::make_pair("./test/lexer/test/test_symbol_operator_binary.asm", true),
				cc65c::assembler::TOKEN_OPERATOR_BINARY),
			std::make_tuple("TEST_SYMBOL_OPERATOR_UNARY", std::make_pair("./test/lexer/test/test_symbol_operator_unary.asm", true),
				cc65c::assembler::TOKEN_OPERATOR_UNARY),
			std::make_tuple("TEST_SYMBOL_POSITION", std::make_pair("./test/lexer/test/test_symbol_position.asm", true),
				cc65c::assembler::TOKEN_SYMBOL_POSITION),
			std::make_tuple("TEST_SYMBOL_SEPERATOR", std::make_pair("./test/lexer/test/test_symbol_seperator.asm", true),
				cc65c::assembler::TOKEN_SYMBOL_SEPERATOR),
			};

		class fixture_lexer :
				public cc65c::test::fixture {

			public:

				fixture_lexer(void) :
					cc65c::test::fixture(LEXER_FIXTURE_NAME)
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
					std::vector<std::tuple<std::string, std::pair<std::string, bool>, cc65c::assembler::token_t>>::const_iterator 
						iter;

					TRACE_ENTRY();

					for(iter = LEXER_TEST_SET.begin(); iter != LEXER_TEST_SET.end(); ++iter) {

						result = test_input(std::get<LEXER_TEST_FILE>(*iter).first, std::get<LEXER_TEST_FILE>(*iter).second, 
							std::get<LEXER_TEST_EXPECTED>(*iter));
						if(result != STATUS_SUCCESS) {
							TRACE_ERROR_FORMAT("Test failure", "Name=%s", STRING_CHECK(std::get<LEXER_TEST_TITLE>(*iter)));
						}
					}

					TRACE_EXIT_FORMAT("Result=%x", result);
					return result;
				}

			protected:

				static cc65c::test::status_t 
				test_input(
					__in const std::string &input,
					__in bool is_file,
					__in cc65c::assembler::token_t expected
					)
				{
					cc65c::test::status_t result = STATUS_SUCCESS;

					TRACE_ENTRY();

					try {
						cc65c::assembler::lexer lex(input, is_file);

						if(!lex.match(cc65c::assembler::TOKEN_BEGIN)) {
							THROW_EXCEPTION_FORMAT("Expecting TOKEN_BEGIN", "%s", STRING_CHECK(lex.to_string(true)));
						}

						while(lex.has_next()) {
							lex.move_next();

							if(!lex.match(cc65c::assembler::TOKEN_BEGIN) 
									&& !lex.match(cc65c::assembler::TOKEN_END) 
									&& !lex.match(expected)) {
								THROW_EXCEPTION_FORMAT("Token mismatch", "Expecting=%u(%x), Found=%s", 
									expected, expected, STRING_CHECK(lex.to_string(true)));
							}
						}

						if(!lex.match(cc65c::assembler::TOKEN_END)) {
							THROW_EXCEPTION_FORMAT("Expecting TOKEN_END", "%s", STRING_CHECK(lex.to_string(true)));
						}

						while(lex.has_previous()) {

							if(!lex.match(cc65c::assembler::TOKEN_BEGIN) 
									&& !lex.match(cc65c::assembler::TOKEN_END) 
									&& !lex.match(expected)) {
								THROW_EXCEPTION_FORMAT("Token mismatch", "Expecting=%u(%x), Found=%s", 
									expected, expected, STRING_CHECK(lex.to_string(true)));
							}

							lex.move_previous();
						}

						if(!lex.match(cc65c::assembler::TOKEN_BEGIN)) {
							THROW_EXCEPTION_FORMAT("Expecting TOKEN_BEGIN", "%s", STRING_CHECK(lex.to_string(true)));
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
		cc65c::test::suite suite(LEXER_SUITE_NAME);
		cc65c::test::fixture_lexer test; 
		suite.add(test);
		result = suite.run(LEXER_FIXTURE_NAME);
		instance.uninitialize();
	} catch(cc65c::core::exception &exc) {
		TRACE_ERROR(STRING_CHECK(exc.to_string(true)));
		std::cerr << exc.to_string(true) << std::endl;
	}

	return result;
}
