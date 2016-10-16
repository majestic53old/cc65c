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

#include <algorithm>
#include <cmath>
#include "../../include/assembler/token.h"
#include "token_type.h"
#include "../../include/core/trace.h"
#include "../../include/assembler/symbol.h"

namespace cc65c {

	namespace assembler {

		#define CHARACTER_LITERAL_ESCAPE '\\'
		#define CHARACTER_SCALAR_MIN '0'
		#define CHARACTER_SCALAR_MIN_ALPHA 'a'

		static const uint32_t SCALAR_BASE_RAD[] = {
			2, 10, 16, 8,
			};

		#define SCALAR_BASE_RADIX(_TYPE_) \
			((_TYPE_) > SCALAR_BASE_MAX ? 0 : SCALAR_BASE_RAD[_TYPE_])

		static const std::map<cc65c::assembler::token_t, const std::set<std::string> *> TOKEN_SUBTYPE_TYPE_MAP = {
			std::make_pair(TOKEN_KEYWORD_COMMAND, &KEYWORD_COMMAND_SET),
			std::make_pair(TOKEN_KEYWORD_CONDITION, &KEYWORD_CONDITION_SET),
			std::make_pair(TOKEN_KEYWORD_DEFINE, &KEYWORD_DEFINE_SET),
			std::make_pair(TOKEN_KEYWORD_INCLUDE, &KEYWORD_INCLUDE_SET),
			std::make_pair(TOKEN_KEYWORD_MACRO, &KEYWORD_MACRO_SET),
			std::make_pair(TOKEN_KEYWORD_REGISTER, &KEYWORD_REGISTER_SET),
			std::make_pair(TOKEN_OPERATOR_BINARY, &OPERATOR_BINARY_SET),
			std::make_pair(TOKEN_OPERATOR_UNARY, &OPERATOR_UNARY_SET),
			std::make_pair(TOKEN_SYMBOL_ARITHMETIC, &SYMBOL_ARITHMETIC_SET),
			std::make_pair(TOKEN_SYMBOL_BRACE, &SYMBOL_BRACE_SET),
			std::make_pair(TOKEN_SYMBOL_BRACKET, &SYMBOL_BRACKET_SET),
			std::make_pair(TOKEN_SYMBOL_IMMEDIATE, &SYMBOL_IMMEDIATE_SET),
			std::make_pair(TOKEN_SYMBOL_POSITION, &SYMBOL_POSITION_SET),
			std::make_pair(TOKEN_SYMBOL_SEPERATOR, &SYMBOL_SEPERATOR_SET)		
			};

		static const std::map<cc65c::assembler::token_t, std::pair<const std::string *, size_t>> TOKEN_SUBTYPE_VALUE_MAP = {
			std::make_pair(TOKEN_KEYWORD_COMMAND, std::make_pair(KEYWORD_COMMAND_STR, (KEYWORD_COMMAND_MAX + 1))),
			std::make_pair(TOKEN_KEYWORD_CONDITION, std::make_pair(KEYWORD_CONDITION_STR, (KEYWORD_CONDITION_MAX + 1))),
			std::make_pair(TOKEN_KEYWORD_DEFINE, std::make_pair(KEYWORD_DEFINE_STR, (KEYWORD_DEFINE_MAX + 1))),
			std::make_pair(TOKEN_KEYWORD_INCLUDE, std::make_pair(KEYWORD_INCLUDE_STR, (KEYWORD_INCLUDE_MAX + 1))),
			std::make_pair(TOKEN_KEYWORD_MACRO, std::make_pair(KEYWORD_MACRO_STR, (KEYWORD_MACRO_MAX + 1))),
			std::make_pair(TOKEN_KEYWORD_REGISTER, std::make_pair(KEYWORD_REGISTER_STR, (KEYWORD_REGISTER_MAX + 1))),
			std::make_pair(TOKEN_OPERATOR_BINARY, std::make_pair(OPERATOR_BINARY_STR, (OPERATOR_BINARY_MAX + 1))),
			std::make_pair(TOKEN_OPERATOR_UNARY, std::make_pair(OPERATOR_UNARY_STR, (OPERATOR_UNARY_MAX + 1))),
			std::make_pair(TOKEN_SYMBOL_ARITHMETIC, std::make_pair(SYMBOL_ARITHMETIC_STR, (SYMBOL_ARITHMETIC_MAX + 1))),
			std::make_pair(TOKEN_SYMBOL_BRACE, std::make_pair(SYMBOL_BRACE_STR, (SYMBOL_BRACE_MAX + 1))),
			std::make_pair(TOKEN_SYMBOL_BRACKET, std::make_pair(SYMBOL_BRACKET_STR, (SYMBOL_BRACKET_MAX + 1))),
			std::make_pair(TOKEN_SYMBOL_IMMEDIATE, std::make_pair(SYMBOL_IMMEDIATE_STR, (SYMBOL_IMMEDIATE_MAX + 1))),
			std::make_pair(TOKEN_SYMBOL_POSITION, std::make_pair(SYMBOL_POSITION_STR, (SYMBOL_POSITION_MAX + 1))),
			std::make_pair(TOKEN_SYMBOL_SEPERATOR, std::make_pair(SYMBOL_SEPERATOR_STR, (SYMBOL_SEPERATOR_MAX + 1))),
			};

		token::token(void) :
			cc65c::core::object(cc65c::core::OBJECT_TOKEN, TOKEN_BEGIN),
			m_column(0),
			m_key(nullptr),
			m_row(0),
			m_token_subtype(SUBTYPE_UNDEFINED),
			m_value(nullptr)
		{
			TRACE_ENTRY();

			generate(TOKEN_BEGIN, SUBTYPE_UNDEFINED);

			TRACE_EXIT();
		}

		token::token(
			__in cc65c::assembler::token_t type,
			__in_opt uint32_t subtype,
			__in_opt cc65c::core::stream_t row,
			__in_opt cc65c::core::stream_t column
			) :
				cc65c::core::object(cc65c::core::OBJECT_TOKEN, type),
				m_column(column),
				m_key(nullptr),
				m_row(row),
				m_token_subtype(subtype),
				m_value(nullptr)
		{
			TRACE_ENTRY();

			if(type > TOKEN_MAX) {
				THROW_CC65C_ASSEMBLER_TOKEN_EXCEPTION_FORMAT(CC65C_ASSEMBLER_TOKEN_EXCEPTION_INVALID_TYPE,
					"Type=%x", type);
			}

			generate(type, subtype, row, column);

			TRACE_EXIT();
		}

		token::token(
			__in cc65c::assembler::token_t type,
			__in const std::string &key,
			__in_opt cc65c::core::stream_t row,
			__in_opt cc65c::core::stream_t column
			) :
				cc65c::core::object(cc65c::core::OBJECT_TOKEN, type),
				m_column(column),
				m_key(nullptr),
				m_row(row),
				m_token_subtype(SUBTYPE_UNDEFINED),
				m_value(nullptr)
		{
			TRACE_ENTRY();

			switch(type) {
				case TOKEN_IDENTIFIER:
				case TOKEN_LABEL:
				case TOKEN_LITERAL:
					generate(type, key, row, column);
					break;
				default:
					THROW_CC65C_ASSEMBLER_TOKEN_EXCEPTION_FORMAT(CC65C_ASSEMBLER_TOKEN_EXCEPTION_INVALID_TYPE,
						"Type=%x", type);
			}

			TRACE_EXIT();
		}

		token::token(
			__in int32_t value,
			__in_opt cc65c::core::stream_t row,
			__in_opt cc65c::core::stream_t column
			) :
				cc65c::core::object(cc65c::core::OBJECT_TOKEN, TOKEN_SCALAR),
				m_column(column),
				m_key(nullptr),
				m_row(row),
				m_token_subtype(SUBTYPE_UNDEFINED),
				m_value(nullptr)
		{
			TRACE_ENTRY();

			generate(value, row, column);

			TRACE_EXIT();
		}

		token::token(
			__in cc65c::core::uuid_t id,
			__in const cc65c::assembler::token_metadata_t &metadata
			) :
				cc65c::core::object(id, cc65c::core::OBJECT_TOKEN, std::get<TOKEN_METADATA_TYPE>(metadata)),
				m_column(std::get<TOKEN_METADATA_COLUMN>(metadata)),
				m_key(std::get<TOKEN_METADATA_KEY>(metadata)),
				m_row(std::get<TOKEN_METADATA_ROW>(metadata)),
				m_token_subtype(std::get<TOKEN_METADATA_SUBTYPE>(metadata)),
				m_value(std::get<TOKEN_METADATA_VALUE>(metadata))
		{
			TRACE_ENTRY();

			increment();

			TRACE_EXIT();
		}

		token::token(
			__in const token &other
			) :
				cc65c::core::object(other),
				m_column(other.m_column),
				m_key(other.m_key),
				m_row(other.m_row),
				m_token_subtype(other.m_token_subtype),
				m_value(other.m_value)
		{
			TRACE_ENTRY();

			increment();

			TRACE_EXIT();
		}

		token::~token(void)
		{
			TRACE_ENTRY();

			decrement();

			TRACE_EXIT();
		}

		token &
		token::operator=(
			__in const token &other
			)
		{
			TRACE_ENTRY();

			if(this != &other) {
				decrement();
				cc65c::core::object::operator=(other);
				m_column = other.m_column;
				m_key = other.m_key;
				m_row = other.m_row;
				m_token_subtype = other.m_token_subtype;
				m_value = other.m_value;
				increment();
			}

			TRACE_EXIT_FORMAT("Result=%p", this);
			return *this;
		}

		bool 
		token::operator==(
			__in const token &other
			)
		{
			bool result;

			TRACE_ENTRY();

			if(!m_key || !m_value) {
				THROW_CC65C_ASSEMBLER_TOKEN_EXCEPTION_FORMAT(CC65C_ASSEMBLER_TOKEN_EXCEPTION_INVALID_ALLOCATION,
					"Key=%p, Value=%p", m_key, m_value);
			}

			result = ((this == &other)
				|| (cc65c::core::object::operator==(other)
				&& (m_column == other.m_column)
				&& (m_key == other.m_key)
				&& (m_row == other.m_row)
				&& (m_token_subtype == other.m_token_subtype)
				&& (m_value == other.m_value)));

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		bool 
		token::operator!=(
			__in const token &other
			)
		{
			bool result;

			TRACE_ENTRY();

			if(!m_key || !m_value) {
				THROW_CC65C_ASSEMBLER_TOKEN_EXCEPTION_FORMAT(CC65C_ASSEMBLER_TOKEN_EXCEPTION_INVALID_ALLOCATION,
					"Key=%p, Value=%p", m_key, m_value);
			}

			result = ((this != &other)
				&& (cc65c::core::object::operator!=(other)
				|| (m_column != other.m_column)
				|| (m_key != other.m_key)
				|| (m_row != other.m_row)
				|| (m_token_subtype != other.m_token_subtype)
				|| (m_value != other.m_value)));

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		int32_t 
		token::as_scalar(
			__in const std::string &input,
			__in cc65c::assembler::base_t base
			)
		{
			uint8_t value;
			size_t iter = 0;
			int32_t radix, result = 0;
			std::string processed = input;

			TRACE_ENTRY();

			radix = SCALAR_BASE_RADIX(base);
			std::transform(processed.begin(), processed.end(), processed.begin(), ::tolower);

			for(; iter < processed.size(); ++iter) {

				value = processed.at(iter);
				if(std::isalpha(value)) {
					value -= CHARACTER_SCALAR_MIN_ALPHA;
					value += SCALAR_BASE_RADIX(SCALAR_BASE_DECIMAL);
				} else {
					value = (processed.at(iter) - CHARACTER_SCALAR_MIN);
				}

				result += (value * std::pow(radix, processed.size() - iter - 1));
			}

			TRACE_EXIT_FORMAT("Result=%i", result);
			return result;
		}

		std::string 
		token::as_string(
			__in const token &reference,
			__in_opt bool verbose
			)
		{
			char ch;
			std::string buffer;
			std::stringstream result;
			cc65c::assembler::token_t type;
			uint32_t iter = 0, iter_escape, subtype;

			TRACE_ENTRY();

			type = (cc65c::assembler::token_t) reference.m_subtype;
			subtype = reference.m_token_subtype;
			result << cc65c::core::object::as_string(reference, verbose)
				<< ", Type=" << SCALAR_AS_HEX(cc65c::assembler::token_t, type) << "(" << TOKEN_STRING(type) << ")";

			if(subtype != SUBTYPE_UNDEFINED) {

				switch(type) {
					case TOKEN_BEGIN:
					case TOKEN_END:
					case TOKEN_IDENTIFIER:
					case TOKEN_LABEL:
					case TOKEN_LITERAL:
					case TOKEN_SCALAR:
						break;
					case TOKEN_KEYWORD_COMMAND:
						buffer = KEYWORD_COMMAND_STRING(subtype);
						break;
					case TOKEN_KEYWORD_CONDITION:
						buffer = KEYWORD_CONDITION_STRING(subtype);
						break;
					case TOKEN_KEYWORD_DEFINE:
						buffer = KEYWORD_DEFINE_STRING(subtype);
						break;
					case TOKEN_KEYWORD_INCLUDE:
						buffer = KEYWORD_INCLUDE_STRING(subtype);
						break;
					case TOKEN_KEYWORD_MACRO:
						buffer = KEYWORD_MACRO_STRING(subtype);
						break;
					case TOKEN_KEYWORD_REGISTER:
						buffer = KEYWORD_REGISTER_STRING(subtype);
						break;
					case TOKEN_OPERATOR_BINARY:
						result << ", " << OPERATOR_BINARY_STRING(subtype);
						break;
					case TOKEN_OPERATOR_UNARY:
						result << ", " << OPERATOR_UNARY_STRING(subtype);
						break;
					case TOKEN_SYMBOL_ARITHMETIC:
						result << ", " << SYMBOL_ARITHMETIC_STRING(subtype);
						break;
					case TOKEN_SYMBOL_BRACE:
						result << ", " << SYMBOL_BRACE_STRING(subtype);
						break;
					case TOKEN_SYMBOL_BRACKET:
						result << ", " << SYMBOL_BRACKET_STRING(subtype);
						break;
					case TOKEN_SYMBOL_IMMEDIATE:
						result << ", " << SYMBOL_IMMEDIATE_STRING(subtype);
						break;
					case TOKEN_SYMBOL_POSITION:
						result << ", " << SYMBOL_POSITION_STRING(subtype);
						break;
					case TOKEN_SYMBOL_SEPERATOR:
						result << ", " << SYMBOL_SEPERATOR_STRING(subtype);
						break;
					default:
						result << ", " << SCALAR_AS_HEX(uint32_t, subtype);
						break;
				}

				if(!buffer.empty()) {
					std::transform(buffer.begin(), buffer.end(), buffer.begin(), ::toupper);
					result << ", " << SCALAR_AS_HEX(uint32_t, subtype) << "(" << buffer << ")";
				}
			}

			switch(type) {
				case TOKEN_IDENTIFIER:
				case TOKEN_LABEL:
				case TOKEN_LITERAL:

					if(!reference.m_key) {
						THROW_CC65C_ASSEMBLER_TOKEN_EXCEPTION_FORMAT(CC65C_ASSEMBLER_TOKEN_EXCEPTION_INVALID_ALLOCATION,
							"Key=%p", reference.m_key);
					}

					result << ", Key[" << reference.m_key->size() << "]=\"";

					for(; iter < reference.m_key->size(); ++iter) {

						ch = reference.m_key->at(iter);
						if(!std::isprint(ch)) {
							result << CHARACTER_LITERAL_ESCAPE;

							for(iter_escape = 0; iter_escape <= CHARACTER_ESCAPE_MAX; ++iter_escape) {

								if(ch == CHARACTER_ESCAPE_CHARACTER_CONVERSION(iter_escape)) {
									break;
								}
							}

							if((iter_escape > CHARACTER_ESCAPE_MAX)
									|| (iter_escape == CHARACTER_ESCAPE_SCALAR_HEXIDECIMAL)) {
								result << CHARACTER_ESCAPE_CHARACTER(CHARACTER_ESCAPE_SCALAR_HEXIDECIMAL)
									<< SCALAR_AS_HEX(uint8_t, ch);
							} else {
								result << CHARACTER_ESCAPE_CHARACTER(iter_escape);
							}
						} else {
							result << ch;
						}
					}

					result << "\"";
					break;
				case TOKEN_SCALAR:

					if(!reference.m_value) {
						THROW_CC65C_ASSEMBLER_TOKEN_EXCEPTION_FORMAT(CC65C_ASSEMBLER_TOKEN_EXCEPTION_INVALID_ALLOCATION,
							"Value=%p", reference.m_value);
					}

					result << ", Value=" << *reference.m_value 
						<< "(" << SCALAR_AS_HEX(int32_t, *reference.m_value) << ")";
					break;
				default:
					break;
			}

			result << " (" << reference.m_row << ":" << reference.m_column << ")";

			TRACE_EXIT();
			return result.str();
		}

		cc65c::assembler::token_t 
		token::as_type(
			__in const token &reference
			)
		{
			cc65c::assembler::token_t result;

			TRACE_ENTRY();

			result = (cc65c::assembler::token_t) reference.m_subtype;

			TRACE_EXIT_FORMAT("Result=%x(%s)", result, TOKEN_STRING(result));
			return result;
		}

		cc65c::core::stream_t 
		token::column(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_token_mutex);

			TRACE_EXIT_FORMAT("Result=%u", m_column);
			return m_column;
		}

		void 
		token::decrement(void)
		{
			TRACE_ENTRY();

			cc65c::assembler::symbol &instance = cc65c::assembler::symbol::acquire();
			if(instance.is_initialized() && instance.contains_token(m_id)) {
				instance.decrement_token(m_id);
			}

			TRACE_EXIT();
		}

		bool 
		token::determine_type(
			__in const std::string &key,
			__inout cc65c::assembler::token_t &type,
			__inout uint32_t &subtype
			)
		{
			bool result = false;
			std::map<cc65c::assembler::token_t, const std::set<std::string> *>::const_iterator type_iter;
			std::map<cc65c::assembler::token_t, std::pair<const std::string *, size_t>>::const_iterator subtype_iter;

			TRACE_ENTRY();

			if(key.empty()) {
				THROW_CC65C_ASSEMBLER_TOKEN_EXCEPTION_FORMAT(CC65C_ASSEMBLER_TOKEN_EXCEPTION_INVALID_KEY,
					"Key[%u]=\"%s\"", key.size(), STRING_CHECK(key));
			}

			type = TOKEN_IDENTIFIER;
			subtype = SUBTYPE_UNDEFINED;

			for(type_iter = TOKEN_SUBTYPE_TYPE_MAP.begin(); type_iter != TOKEN_SUBTYPE_TYPE_MAP.end(); ++type_iter) {

				if(type_iter->second 
						&& (type_iter->second->find(key) != type_iter->second->end())) {
					type = type_iter->first;
					result = true;
					break;
				}
			}

			if(result) {

				if(type_iter != TOKEN_SUBTYPE_TYPE_MAP.end()) {

					subtype_iter = TOKEN_SUBTYPE_VALUE_MAP.find(type);
					if(subtype_iter != TOKEN_SUBTYPE_VALUE_MAP.end()) {

						for(subtype = 0; subtype < subtype_iter->second.second; ++subtype) {

							if(subtype_iter->second.first 
									&& subtype_iter->second.first[subtype] == key) {
								break;
							}
						}

						if(subtype == subtype_iter->second.second) {
							subtype = SUBTYPE_UNDEFINED;
						}
					}
				}
			}

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		void 
		token::generate(
			__in cc65c::assembler::token_t type,
			__in_opt uint32_t subtype,
			__in_opt cc65c::core::stream_t row,
			__in_opt cc65c::core::stream_t column
			)
		{
			TRACE_ENTRY();

			cc65c::assembler::symbol &instance = cc65c::assembler::symbol::acquire();
			if(instance.is_initialized()) {
				instance.generate_token(m_id, type, subtype, m_key, m_value, row, column);

				if(!m_key || !m_value) {
					THROW_CC65C_ASSEMBLER_TOKEN_EXCEPTION_FORMAT(CC65C_ASSEMBLER_TOKEN_EXCEPTION_INVALID_ALLOCATION,
						"Key=%p, Value=%p", m_key, m_value);
				}
			}

			TRACE_EXIT();
		}

		void 
		token::generate(
			__in cc65c::assembler::token_t type,
			__in const std::string &key,
			__in_opt cc65c::core::stream_t row,
			__in_opt cc65c::core::stream_t column
			)
		{
			TRACE_ENTRY();

			cc65c::assembler::symbol &instance = cc65c::assembler::symbol::acquire();
			if(instance.is_initialized()) {
				instance.generate_token(m_id, type, key, m_key, m_value, row, column);

				if(!m_key || !m_value) {
					THROW_CC65C_ASSEMBLER_TOKEN_EXCEPTION_FORMAT(CC65C_ASSEMBLER_TOKEN_EXCEPTION_INVALID_ALLOCATION,
						"Key=%p, Value=%p", m_key, m_value);
				}
			}

			TRACE_EXIT();
		}

		void 
		token::generate(
			__in int32_t value,
			__in_opt cc65c::core::stream_t row,
			__in_opt cc65c::core::stream_t column
			)
		{
			TRACE_ENTRY();

			cc65c::assembler::symbol &instance = cc65c::assembler::symbol::acquire();
			if(instance.is_initialized()) {
				instance.generate_token(m_id, value, m_key, m_value, row, column);

				if(!m_key || !m_value) {
					THROW_CC65C_ASSEMBLER_TOKEN_EXCEPTION_FORMAT(CC65C_ASSEMBLER_TOKEN_EXCEPTION_INVALID_ALLOCATION,
						"Key=%p, Value=%p", m_key, m_value);
				}
			}

			TRACE_EXIT();
		}

		void 
		token::increment(void)
		{
			TRACE_ENTRY();

			cc65c::assembler::symbol &instance = cc65c::assembler::symbol::acquire();
			if(instance.is_initialized() && instance.contains_token(m_id)) {
				instance.increment_token(m_id);
			}

			TRACE_EXIT();
		}

		std::string &
		token::key(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_token_mutex);

			if(!m_key) {
				THROW_CC65C_ASSEMBLER_TOKEN_EXCEPTION_FORMAT(CC65C_ASSEMBLER_TOKEN_EXCEPTION_INVALID_ALLOCATION,
					"Key=%p", m_key);
			}

			TRACE_EXIT_FORMAT("Result=%s", STRING_CHECK((*m_key)));
			return *m_key;
		}

		bool 
		token::match(
			__in cc65c::assembler::token_t type,
			__in_opt uint32_t subtype
			)
		{
			bool result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_token_mutex);

			result = (type == (cc65c::assembler::token_t) m_subtype);

			if(subtype != SUBTYPE_UNDEFINED) {
				result &= (subtype == m_token_subtype);
			}

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		bool 
		token::match(
			__in cc65c::assembler::token_t type,
			__in const std::string &key
			)
		{
			bool result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_token_mutex);

			if(!m_key) {
				THROW_CC65C_ASSEMBLER_TOKEN_EXCEPTION_FORMAT(CC65C_ASSEMBLER_TOKEN_EXCEPTION_INVALID_ALLOCATION,
					"Key=%p", m_key);
			}

			result = ((type == (cc65c::assembler::token_t) m_subtype) && (key == *m_key));

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		bool 
		token::match(
			__in int32_t value
			)
		{
			bool result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_token_mutex);

			if(!m_value) {
				THROW_CC65C_ASSEMBLER_TOKEN_EXCEPTION_FORMAT(CC65C_ASSEMBLER_TOKEN_EXCEPTION_INVALID_ALLOCATION,
					"Value=%p", m_value);
			}

			result = ((((cc65c::assembler::token_t) m_subtype) == TOKEN_SCALAR) && (value == *m_value));

			TRACE_EXIT_FORMAT("Result=%x", result);
			return result;
		}

		cc65c::core::stream_t 
		token::row(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_token_mutex);

			TRACE_EXIT_FORMAT("Result=%u", m_row);
			return m_row;
		}

		void 
		token::set_metadata(
			__in cc65c::core::stream_t row,
			__in cc65c::core::stream_t column
			)
		{
			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_token_mutex);

			m_column = column;
			m_row = row;

			TRACE_EXIT();
		}

		uint32_t 
		token::subtype(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_token_mutex);

			TRACE_EXIT_FORMAT("Result=%x", m_token_subtype);
			return m_token_subtype;
		}

		std::string 
		token::to_string(
			__in_opt bool verbose
			)
		{
			std::string result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_token_mutex);

			result = cc65c::assembler::token::as_string(*this, verbose);

			TRACE_EXIT();
			return result;
		}

		cc65c::assembler::token_t 
		token::type(void)
		{
			cc65c::assembler::token_t result;

			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_token_mutex);

			result = (cc65c::assembler::token_t) m_subtype;

			TRACE_EXIT_FORMAT("Result=%x(%s)", result, TOKEN_STRING(result));
			return result;
		}

		int32_t &
		token::value(void)
		{
			TRACE_ENTRY();

			std::lock_guard<std::mutex> lock(m_token_mutex);

			if(!m_value) {
				THROW_CC65C_ASSEMBLER_TOKEN_EXCEPTION_FORMAT(CC65C_ASSEMBLER_TOKEN_EXCEPTION_INVALID_ALLOCATION,
					"Value=%p", m_value);
			}

			TRACE_EXIT_FORMAT("Result=%i", *m_value);
			return *m_value;
		}
	}
}
