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

#ifndef CC65C_ASSEMBLER_TOKEN_H_
#define CC65C_ASSEMBLER_TOKEN_H_

#include "../core/object.h"
#include "stream.h"

namespace cc65c {

	namespace assembler {

		enum {
			CHARACTER_ESCAPE_ALARM = 0,
			CHARACTER_ESCAPE_BACKSLASH,
			CHARACTER_ESCAPE_BACKSPACE,
			CHARACTER_ESCAPE_CARRAGE_RETURN,
			CHARACTER_ESCAPE_DOUBLE_QUOTE,
			CHARACTER_ESCAPE_FORMFEED,
			CHARACTER_ESCAPE_HORIZONTAL_TAB,
			CHARACTER_ESCAPE_NEWLINE,
			CHARACTER_ESCAPE_QUESTION_MARK,
			CHARACTER_ESCAPE_SCALAR_HEXIDECIMAL,
			CHARACTER_ESCAPE_SINGLE_QUOTE,
			CHARACTER_ESCAPE_VERTICAL_TAB,
		};

		#define CHARACTER_ESCAPE_MAX CHARACTER_ESCAPE_VERTICAL_TAB

		static const char CHARACTER_ESCAPE_CHAR[] = {
			'a', '\\', 'b', 'r', '\"', 'f', 't', 'n', '?', 'x', '\'', 'v',
			};

		#define CHARACTER_ESCAPE_CHARACTER(_TYPE_) \
			((_TYPE_) > CHARACTER_ESCAPE_MAX ? '\0' : CHARACTER_ESCAPE_CHAR[_TYPE_])

		static const char CHARACTER_ESCAPE_CHAR_CONV[] = {
			'\a', '\\', '\b', '\r', '\"', '\f', '\t', '\n', '\?', '\0', '\'', '\v',
			};

		#define CHARACTER_ESCAPE_CHARACTER_CONVERSION(_TYPE_) \
			((_TYPE_) > CHARACTER_ESCAPE_MAX ? '\0' : CHARACTER_ESCAPE_CHAR_CONV[_TYPE_])

		enum {
			KEYWORD_COMMAND_ADC = 0, KEYWORD_COMMAND_AND, KEYWORD_COMMAND_ASL,
			KEYWORD_COMMAND_BBR0, KEYWORD_COMMAND_BBR1, KEYWORD_COMMAND_BBR2,
			KEYWORD_COMMAND_BBR3, KEYWORD_COMMAND_BBR4, KEYWORD_COMMAND_BBR5,
			KEYWORD_COMMAND_BBR6, KEYWORD_COMMAND_BBR7, KEYWORD_COMMAND_BBS0, 
			KEYWORD_COMMAND_BBS1, KEYWORD_COMMAND_BBS2, KEYWORD_COMMAND_BBS3, 
			KEYWORD_COMMAND_BBS4, KEYWORD_COMMAND_BBS5, KEYWORD_COMMAND_BBS6, 
			KEYWORD_COMMAND_BBS7, KEYWORD_COMMAND_BCC, KEYWORD_COMMAND_BCS, 
			KEYWORD_COMMAND_BEQ, KEYWORD_COMMAND_BIT, KEYWORD_COMMAND_BMI, 
			KEYWORD_COMMAND_BNE, KEYWORD_COMMAND_BPL, KEYWORD_COMMAND_BRA, 
			KEYWORD_COMMAND_BRK, KEYWORD_COMMAND_BVC, KEYWORD_COMMAND_BVS, 
			KEYWORD_COMMAND_CLC, KEYWORD_COMMAND_CLD, KEYWORD_COMMAND_CLI, 
			KEYWORD_COMMAND_CLV, KEYWORD_COMMAND_CMP, KEYWORD_COMMAND_CPX, 
			KEYWORD_COMMAND_CPY, KEYWORD_COMMAND_DEC, KEYWORD_COMMAND_DEX, 
			KEYWORD_COMMAND_DEY, KEYWORD_COMMAND_EOR, KEYWORD_COMMAND_INC, 
			KEYWORD_COMMAND_INX, KEYWORD_COMMAND_INY, KEYWORD_COMMAND_JMP, 
			KEYWORD_COMMAND_JSR, KEYWORD_COMMAND_LDA, KEYWORD_COMMAND_LDX, 
			KEYWORD_COMMAND_LDY, KEYWORD_COMMAND_LSR, KEYWORD_COMMAND_NOP, 
			KEYWORD_COMMAND_ORA, KEYWORD_COMMAND_PHA, KEYWORD_COMMAND_PHP, 
			KEYWORD_COMMAND_PHX, KEYWORD_COMMAND_PHY, KEYWORD_COMMAND_PLA, 
			KEYWORD_COMMAND_PLP, KEYWORD_COMMAND_PLX, KEYWORD_COMMAND_PLY, 
			KEYWORD_COMMAND_RMB0, KEYWORD_COMMAND_RMB1, KEYWORD_COMMAND_RMB2,
			KEYWORD_COMMAND_RMB3, KEYWORD_COMMAND_RMB4, KEYWORD_COMMAND_RMB5,
			KEYWORD_COMMAND_RMB6, KEYWORD_COMMAND_RMB7, KEYWORD_COMMAND_ROL, 
			KEYWORD_COMMAND_ROR, KEYWORD_COMMAND_RTI, KEYWORD_COMMAND_RTS, 
			KEYWORD_COMMAND_SBC, KEYWORD_COMMAND_SEC, KEYWORD_COMMAND_SED, 
			KEYWORD_COMMAND_SEI, KEYWORD_COMMAND_SMB0, KEYWORD_COMMAND_SMB1, 
			KEYWORD_COMMAND_SMB2, KEYWORD_COMMAND_SMB3, KEYWORD_COMMAND_SMB4, 
			KEYWORD_COMMAND_SMB5, KEYWORD_COMMAND_SMB6, KEYWORD_COMMAND_SMB7,
			KEYWORD_COMMAND_STA, KEYWORD_COMMAND_STP, KEYWORD_COMMAND_STX, 
			KEYWORD_COMMAND_STY, KEYWORD_COMMAND_STZ, KEYWORD_COMMAND_TAX,
			KEYWORD_COMMAND_TAY, KEYWORD_COMMAND_TRB, KEYWORD_COMMAND_TSB, 
			KEYWORD_COMMAND_TSX, KEYWORD_COMMAND_TXA, KEYWORD_COMMAND_TXS, 
			KEYWORD_COMMAND_TYA, KEYWORD_COMMAND_WAI,
		};

		#define KEYWORD_COMMAND_MAX KEYWORD_COMMAND_WAI

		static const std::string KEYWORD_COMMAND_STR[] = {
			"adc", "and", "asl", "bbr0", "bbr1", "bbr2", "bbr3", "bbr4", "bbr5", "bbr6", "bbr7", 
			"bbs0", "bbs1", "bbs2", "bbs3", "bbs4", "bbs5", "bbs6", "bbs7", "bcc", "bcs", "beq", 
			"bit", "bmi", "bne", "bpl", "bra", "brk", "bvc", "bvs", "clc", "cld", "cli", "clv", 
			"cmp", "cpx", "cpy", "dec", "dex", "dey", "eor", "inc", "inx", "iny", "jmp", "jsr", 
			"lda", "ldx", "ldy", "lsr", "nop", "ora", "pha", "php", "phx", "phy", "pla", "plp", 
			"plx", "ply", "rmb0", "rmb1", "rmb2", "rmb3", "rmb4", "rmb5", "rmb6", "rmb7", "rol", 
			"ror", "rti", "rts", "sbc", "sec", "sed", "sei", "smb0", "smb1", "smb2", "smb3", 
			"smb4", "smb5", "smb6", "smb7", "sta", "stp", "stx", "sty", "stz", "tax", "tay", 
			"trb", "tsb", "tsx", "txa", "txs", "tya", "wai",
			};

		#define KEYWORD_COMMAND_STRING(_TYPE_) \
			((_TYPE_) > KEYWORD_COMMAND_MAX ? STRING_UNKNOWN : STRING_CHECK(KEYWORD_COMMAND_STR[_TYPE_]))

		static const std::set<std::string> KEYWORD_COMMAND_SET = {
			KEYWORD_COMMAND_STR, KEYWORD_COMMAND_STR + (KEYWORD_COMMAND_MAX + 1)
			};

		#define KEYWORD_COMMAND_CONTAINS(_VAL_) \
			(KEYWORD_COMMAND_SET.find(_VAL_) != KEYWORD_COMMAND_SET.end())

		enum {
			KEYWORD_CONDITION_ELSE = 0,
			KEYWORD_CONDITION_ELSE_IF,
			KEYWORD_CONDITION_ELSE_IF_DEFINE,
			KEYWORD_CONDITION_END_IF,
			KEYWORD_CONDITION_IF,
			KEYWORD_CONDITION_IF_DEFINE,
		};

		#define KEYWORD_CONDITION_MAX KEYWORD_CONDITION_IF_DEFINE

		static const std::string KEYWORD_CONDITION_STR[] = {
			"else", "elif", "elifdef", "endif", "if", "ifdef",
			};

		#define KEYWORD_CONDITION_STRING(_TYPE_) \
			((_TYPE_) > KEYWORD_CONDITION_MAX ? STRING_UNKNOWN : STRING_CHECK(KEYWORD_CONDITION_STR[_TYPE_]))

		static const std::set<std::string> KEYWORD_CONDITION_SET = {
			KEYWORD_CONDITION_STR, KEYWORD_CONDITION_STR + (KEYWORD_CONDITION_MAX + 1)
			};

		#define KEYWORD_CONDITION_CONTAINS(_VAL_) \
			(KEYWORD_CONDITION_SET.find(_VAL_) != KEYWORD_CONDITION_SET.end())

		enum {
			KEYWORD_DEFINE_DATA_BYTE = 0,
			KEYWORD_DEFINE_DATA_WORD,
			KEYWORD_DEFINE_DEFINE_BYTE,
			KEYWORD_DEFINE_ORIGIN,
			KEYWORD_DEFINE_RESERVE,
			KEYWORD_DEFINE_SEGMENT,
			KEYWORD_DEFINE_UNDEFINE,
		};

		#define KEYWORD_DEFINE_MAX KEYWORD_DEFINE_UNDEFINE

		static const std::string KEYWORD_DEFINE_STR[] = {
			"db", "dw", "def", "org", "res", "seg", "undef",
			};

		#define KEYWORD_DEFINE_STRING(_TYPE_) \
			((_TYPE_) > KEYWORD_DEFINE_MAX ? STRING_UNKNOWN : STRING_CHECK(KEYWORD_DEFINE_STR[_TYPE_]))

		static const std::set<std::string> KEYWORD_DEFINE_SET = {
			KEYWORD_DEFINE_STR, KEYWORD_DEFINE_STR + (KEYWORD_DEFINE_MAX + 1)
			};

		#define KEYWORD_DEFINE_CONTAINS(_VAL_) \
			(KEYWORD_DEFINE_SET.find(_VAL_) != KEYWORD_DEFINE_SET.end())

		enum {
			KEYWORD_INCLUDE_BINARY = 0,
			KEYWORD_INCLUDE_HEX,
			KEYWORD_INCLUDE_SOURCE,
		};

		#define KEYWORD_INCLUDE_MAX KEYWORD_INCLUDE_SOURCE

		static const std::string KEYWORD_INCLUDE_STR[] = {
			"incb", "inch", "incs",
			};

		#define KEYWORD_INCLUDE_STRING(_TYPE_) \
			((_TYPE_) > KEYWORD_INCLUDE_MAX ? STRING_UNKNOWN : STRING_CHECK(KEYWORD_INCLUDE_STR[_TYPE_]))

		static const std::set<std::string> KEYWORD_INCLUDE_SET = {
			KEYWORD_INCLUDE_STR, KEYWORD_INCLUDE_STR + (KEYWORD_INCLUDE_MAX + 1)
			};

		#define KEYWORD_INCLUDE_CONTAINS(_VAL_) \
			(KEYWORD_INCLUDE_SET.find(_VAL_) != KEYWORD_INCLUDE_SET.end())

		enum {
			KEYWORD_MACRO_BYTE = 0,
			KEYWORD_MACRO_HIGH,
			KEYWORD_MACRO_LOW,
			KEYWORD_MACRO_WORD,
		};

		#define KEYWORD_MACRO_MAX KEYWORD_MACRO_WORD

		static const std::string KEYWORD_MACRO_STR[] = {
			"byte", "high", "low", "word",
			};

		#define KEYWORD_MACRO_STRING(_TYPE_) \
			((_TYPE_) > KEYWORD_MACRO_MAX ? STRING_UNKNOWN : STRING_CHECK(KEYWORD_MACRO_STR[_TYPE_]))

		static const std::set<std::string> KEYWORD_MACRO_SET = {
			KEYWORD_MACRO_STR, KEYWORD_MACRO_STR + (KEYWORD_MACRO_MAX + 1)
			};

		#define KEYWORD_MACRO_CONTAINS(_VAL_) \
			(KEYWORD_MACRO_SET.find(_VAL_) != KEYWORD_MACRO_SET.end())

		enum {
			KEYWORD_REGISTER_A = 0,
			KEYWORD_REGISTER_X,
			KEYWORD_REGISTER_Y,
		};

		#define KEYWORD_REGISTER_MAX KEYWORD_REGISTER_Y

		static const std::string KEYWORD_REGISTER_STR[] = {
			"a", "x", "y"
			};

		#define KEYWORD_REGISTER_STRING(_TYPE_) \
			((_TYPE_) > KEYWORD_REGISTER_MAX ? STRING_UNKNOWN : STRING_CHECK(KEYWORD_REGISTER_STR[_TYPE_]))

		static const std::set<std::string> KEYWORD_REGISTER_SET = {
			KEYWORD_REGISTER_STR, KEYWORD_REGISTER_STR + (KEYWORD_REGISTER_MAX + 1)
			};

		#define KEYWORD_REGISTER_CONTAINS(_VAL_) \
			(KEYWORD_REGISTER_SET.find(_VAL_) != KEYWORD_REGISTER_SET.end())

		enum {
			OPERATOR_BINARY_AND = 0,
			OPERATOR_BINARY_EQUAL,
			OPERATOR_BINARY_GREATER_THAN,
			OPERATOR_BINARY_GREATER_THAN_EQUAL,
			OPERATOR_BINARY_LESS_THAN,
			OPERATOR_BINARY_LESS_THAN_EQUAL,
			OPERATOR_BINARY_NOT_EQUAL,
			OPERATOR_BINARY_OR,
		};

		#define OPERATOR_BINARY_MAX OPERATOR_BINARY_OR

		static const std::string OPERATOR_BINARY_STR[] = {
			"&&", "==", ">", ">=", "<", "<=", "!=", "||",
			};

		#define OPERATOR_BINARY_STRING(_TYPE_) \
			((_TYPE_) > OPERATOR_BINARY_MAX ? STRING_UNKNOWN : STRING_CHECK(OPERATOR_BINARY_STR[_TYPE_]))

		static const std::set<std::string> OPERATOR_BINARY_SET = {
			OPERATOR_BINARY_STR, OPERATOR_BINARY_STR + (OPERATOR_BINARY_MAX + 1)
			};

		#define OPERATOR_BINARY_CONTAINS(_VAL_) \
			(OPERATOR_BINARY_SET.find(_VAL_) != OPERATOR_BINARY_SET.end())

		enum {
			OPERATOR_UNARY_NOT_BINARY = 0,
			OPERATOR_UNARY_NOT_LOGICAL,
		};

		#define OPERATOR_UNARY_MAX OPERATOR_UNARY_NOT_LOGICAL

		static const std::string OPERATOR_UNARY_STR[] = {
			"~", "!",
			};

		#define OPERATOR_UNARY_STRING(_TYPE_) \
			((_TYPE_) > OPERATOR_UNARY_MAX ? STRING_UNKNOWN : STRING_CHECK(OPERATOR_UNARY_STR[_TYPE_]))

		static const std::set<std::string> OPERATOR_UNARY_SET = {
			OPERATOR_UNARY_STR, OPERATOR_UNARY_STR + (OPERATOR_UNARY_MAX + 1)
			};

		#define OPERATOR_UNARY_CONTAINS(_VAL_) \
			(OPERATOR_UNARY_SET.find(_VAL_) != OPERATOR_UNARY_SET.end())

		typedef enum {
			SCALAR_BASE_BINARY = 0,
			SCALAR_BASE_DECIMAL,
			SCALAR_BASE_HEXIDECIMAL,
			SCALAR_BASE_OCTAL,
		} base_t;

		#define SCALAR_BASE_MAX SCALAR_BASE_OCTAL

		enum {
			SYMBOL_ARITHMETIC_ADDITION = 0,
			SYMBOL_ARITHMETIC_AND,
			SYMBOL_ARITHMETIC_DIVISION,
			SYMBOL_ARITHMETIC_MODULUS,
			SYMBOL_ARITHMETIC_MULTIPLICATION,
			SYMBOL_ARITHMETIC_OR,
			SYMBOL_ARITHMETIC_SUBTRACTION,
			SYMBOL_ARITHMETIC_XOR,
		};

		#define SYMBOL_ARITHMETIC_MAX SYMBOL_ARITHMETIC_XOR

		static const std::string SYMBOL_ARITHMETIC_STR[] = {
			"+", "&", "/", "%", "*", "|", "-", "^",
			};

		#define SYMBOL_ARITHMETIC_STRING(_TYPE_) \
			((_TYPE_) > SYMBOL_ARITHMETIC_MAX ? STRING_UNKNOWN : STRING_CHECK(SYMBOL_ARITHMETIC_STR[_TYPE_]))

		static const std::set<std::string> SYMBOL_ARITHMETIC_SET = {
			SYMBOL_ARITHMETIC_STR, SYMBOL_ARITHMETIC_STR + (SYMBOL_ARITHMETIC_MAX + 1)
			};

		#define SYMBOL_ARITHMETIC_CONTAINS(_VAL_) \
			(SYMBOL_ARITHMETIC_SET.find(_VAL_) != SYMBOL_ARITHMETIC_SET.end())

		enum {
			SYMBOL_BRACE_CLOSE= 0,
			SYMBOL_BRACE_OPEN,
		};

		#define SYMBOL_BRACE_MAX SYMBOL_BRACE_OPEN

		static const std::string SYMBOL_BRACE_STR[] = {
			"]", "[",
			};

		#define SYMBOL_BRACE_STRING(_TYPE_) \
			((_TYPE_) > SYMBOL_BRACE_MAX ? STRING_UNKNOWN : STRING_CHECK(SYMBOL_BRACE_STR[_TYPE_]))

		static const std::set<std::string> SYMBOL_BRACE_SET = {
			SYMBOL_BRACE_STR, SYMBOL_BRACE_STR + (SYMBOL_BRACE_MAX + 1)
			};

		#define SYMBOL_BRACE_CONTAINS(_VAL_) \
			(SYMBOL_BRACE_SET.find(_VAL_) != SYMBOL_BRACE_SET.end())

		enum {
			SYMBOL_BRACKET_CLOSE = 0,
			SYMBOL_BRACKET_OPEN,
		};

		#define SYMBOL_BRACKET_MAX SYMBOL_BRACKET_OPEN

		static const std::string SYMBOL_BRACKET_STR[] = {
			")", "(",
			};

		#define SYMBOL_BRACKET_STRING(_TYPE_) \
			((_TYPE_) > SYMBOL_BRACKET_MAX ? STRING_UNKNOWN : STRING_CHECK(SYMBOL_BRACKET_STR[_TYPE_]))

		static const std::set<std::string> SYMBOL_BRACKET_SET = {
			SYMBOL_BRACKET_STR, SYMBOL_BRACKET_STR + (SYMBOL_BRACKET_MAX + 1)
			};

		#define SYMBOL_BRACKET_CONTAINS(_VAL_) \
			(SYMBOL_BRACKET_SET.find(_VAL_) != SYMBOL_BRACKET_SET.end())

		enum {
			SYMBOL_IMMEDIATE = 0,
		};

		#define SYMBOL_IMMEDIATE_MAX SYMBOL_IMMEDIATE

		static const std::string SYMBOL_IMMEDIATE_STR[] = {
			"#"
			};

		#define SYMBOL_IMMEDIATE_STRING(_TYPE_) \
			((_TYPE_) > SYMBOL_IMMEDIATE_MAX ? STRING_UNKNOWN : STRING_CHECK(SYMBOL_IMMEDIATE_STR[_TYPE_]))

		static const std::set<std::string> SYMBOL_IMMEDIATE_SET = {
			SYMBOL_IMMEDIATE_STR, SYMBOL_IMMEDIATE_STR + (SYMBOL_IMMEDIATE_MAX + 1)
			};

		#define SYMBOL_IMMEDIATE_CONTAINS(_VAL_) \
			(SYMBOL_IMMEDIATE_SET.find(_VAL_) != SYMBOL_IMMEDIATE_SET.end())

		enum {
			SYMBOL_POSITION_BASE = 0,
			SYMBOL_POSITION_OFFSET,
		};

		#define SYMBOL_POSITION_MAX SYMBOL_POSITION_OFFSET

		static const std::string SYMBOL_POSITION_STR[] = {
			"$$", "$",
			};

		#define SYMBOL_POSITION_STRING(_TYPE_) \
			((_TYPE_) > SYMBOL_POSITION_MAX ? STRING_UNKNOWN : STRING_CHECK(SYMBOL_POSITION_STR[_TYPE_]))

		static const std::set<std::string> SYMBOL_POSITION_SET = {
			SYMBOL_POSITION_STR, SYMBOL_POSITION_STR + (SYMBOL_POSITION_MAX + 1)
			};

		#define SYMBOL_POSITION_CONTAINS(_VAL_) \
			(SYMBOL_POSITION_SET.find(_VAL_) != SYMBOL_POSITION_SET.end())

		enum {
			SYMBOL_SEPERATOR = 0,
		};

		#define SYMBOL_SEPERATOR_MAX SYMBOL_SEPERATOR

		static const std::string SYMBOL_SEPERATOR_STR[] = {
			",",
			};

		#define SYMBOL_SEPERATOR_STRING(_TYPE_) \
			((_TYPE_) > SYMBOL_SEPERATOR_MAX ? STRING_UNKNOWN : STRING_CHECK(SYMBOL_SEPERATOR_STR[_TYPE_]))

		static const std::set<std::string> SYMBOL_SEPERATOR_SET = {
			SYMBOL_SEPERATOR_STR, SYMBOL_SEPERATOR_STR + (SYMBOL_SEPERATOR_MAX + 1)
			};

		#define SYMBOL_SEPERATOR_CONTAINS(_VAL_) \
			(SYMBOL_SEPERATOR_SET.find(_VAL_) != SYMBOL_SEPERATOR_SET.end())

		typedef enum {
			TOKEN_BEGIN = 0,
			TOKEN_END,
			TOKEN_IDENTIFIER,
			TOKEN_KEYWORD_COMMAND,
			TOKEN_KEYWORD_CONDITION,
			TOKEN_KEYWORD_DEFINE,
			TOKEN_KEYWORD_INCLUDE,
			TOKEN_KEYWORD_MACRO,
			TOKEN_KEYWORD_REGISTER,
			TOKEN_LABEL,
			TOKEN_LITERAL,
			TOKEN_OPERATOR_BINARY,
			TOKEN_OPERATOR_UNARY,
			TOKEN_SCALAR,
			TOKEN_SYMBOL_ARITHMETIC,
			TOKEN_SYMBOL_BRACE,
			TOKEN_SYMBOL_BRACKET,
			TOKEN_SYMBOL_IMMEDIATE,
			TOKEN_SYMBOL_POSITION,
			TOKEN_SYMBOL_SEPERATOR,
		} token_t;

		#define TOKEN_MAX TOKEN_SYMBOL_SEPERATOR

		enum {
			TOKEN_METADATA_COLUMN = 0,
			TOKEN_METADATA_KEY,
			TOKEN_METADATA_ROW,
			TOKEN_METADATA_SUBTYPE,
			TOKEN_METADATA_TYPE,
			TOKEN_METADATA_VALUE,
		};

		typedef std::tuple<cc65c::core::stream_t, std::string *, cc65c::core::stream_t, uint32_t, cc65c::assembler::token_t, 
			int32_t *> token_metadata_t;

		class token :
				public cc65c::core::object {

			public:

				token(void);

				token(
					__in cc65c::assembler::token_t type,
					__in_opt uint32_t subtype = SUBTYPE_UNDEFINED,
					__in_opt cc65c::core::stream_t row = 0,
					__in_opt cc65c::core::stream_t column = 0
					);

				token(
					__in cc65c::assembler::token_t type,
					__in const std::string &key,
					__in_opt cc65c::core::stream_t row = 0,
					__in_opt cc65c::core::stream_t column = 0
					);

				token(
					__in int32_t value,
					__in_opt cc65c::core::stream_t row = 0,
					__in_opt cc65c::core::stream_t column = 0
					);

				token(
					__in cc65c::core::uuid_t id,
					__in const cc65c::assembler::token_metadata_t &metadata
					);

				token(
					__in const token &other
					);

				virtual ~token(void);

				token &operator=(
					__in const token &other
					);

				bool operator==(
					__in const token &other
					);

				bool operator!=(
					__in const token &other
					);

				static int32_t as_scalar(
					__in const std::string &input,
					__in cc65c::assembler::base_t base
					);

				static std::string as_string(
					__in const token &reference,
					__in_opt bool verbose = false
					);

				static cc65c::assembler::token_t as_type(
					__in const token &reference
					);

				cc65c::core::stream_t column(void);

				static bool determine_type(
					__in const std::string &key,
					__inout cc65c::assembler::token_t &type,
					__inout uint32_t &subtype
					);

				std::string &key(void);

				bool match(
					__in cc65c::assembler::token_t type,
					__in_opt uint32_t subtype = SUBTYPE_UNDEFINED
					);

				bool match(
					__in cc65c::assembler::token_t type,
					__in const std::string &key
					);

				bool match(
					__in int32_t value
					);

				cc65c::core::stream_t row(void);

				void set_metadata(
					__in cc65c::core::stream_t row,
					__in cc65c::core::stream_t column
					);

				uint32_t subtype(void);

				virtual std::string to_string(
					__in_opt bool verbose = false
					);

				cc65c::assembler::token_t type(void);

				int32_t &value(void);

			protected:

				void decrement(void);

				void generate(
					__in cc65c::assembler::token_t type,
					__in_opt uint32_t subtype = SUBTYPE_UNDEFINED,
					__in_opt cc65c::core::stream_t row = 0,
					__in_opt cc65c::core::stream_t column = 0
					);

				void generate(
					__in cc65c::assembler::token_t type,
					__in const std::string &key,
					__in_opt cc65c::core::stream_t row = 0,
					__in_opt cc65c::core::stream_t column = 0
					);

				void generate(
					__in int32_t value,
					__in_opt cc65c::core::stream_t row = 0,
					__in_opt cc65c::core::stream_t column = 0
					);

				void increment(void);

				cc65c::core::stream_t m_column;

				std::string *m_key;

				cc65c::core::stream_t m_row;

				std::mutex m_token_mutex;

				uint32_t m_token_subtype;

				int32_t *m_value;
		};
	}
}

#endif // CC65C_ASSEMBLER_TOKEN_H_
