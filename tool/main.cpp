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

// TODO: DEBUGGING
#include "../include/assembler/parser.h"
// ---

int 
main(void)
{
	int result = 0;

	std::cout << CC65C << " " << cc65c::runtime::version(true) << std::endl;

	try {
		cc65c::runtime &instance = cc65c::runtime::acquire();
		instance.initialize();

		// TODO: DEBUGGING
		cc65c::assembler::parser par("./test/test.asm");
		//par.enumerate();

		while(par.has_next()) {
			std::cout << par.to_string(true) << std::endl;
			par.move_next();
		}

		std::cout << par.to_string(true) << std::endl;

		/*while(par.has_previous()) {
			par.move_previous();
			std::cout << par.to_string(true) << std::endl;
		}*/
		// ---

		instance.uninitialize();
	} catch(cc65c::core::exception &exc) {
		std::cerr << exc.to_string(true) << std::endl;
	}

	return result;
}
