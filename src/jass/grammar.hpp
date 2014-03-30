/***************************************************************************
 *   Copyright (C) 2014 by Tamino Dauth                                    *
 *   tamino@cdauth.eu                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef WC3LIB_JASS_GRAMMAR_HPP
#define WC3LIB_JASS_GRAMMAR_HPP

#include "../spirit.hpp"

#include <iterator>

#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/spirit/include/classic_position_iterator.hpp> // for more detailed error information

#include "../platform.hpp"
#include "../exception.hpp"
#include "ast.hpp"

namespace wc3lib
{

namespace jass
{

class Grammar {
	public:
		typedef std::basic_istream<byte> InputStream;
		typedef std::istreambuf_iterator<byte> IteratorType;
		typedef boost::spirit::multi_pass<IteratorType> ForwardIteratorType;
		
		bool parse(InputStream &istream, jass_ast &ast, jass_file &file);
		bool parse(IteratorType first, IteratorType last, jass_ast &ast, jass_file &file);
		
		/**
		 * Creates the parsed file automatically using \p fileName as file path.
		 * The created file will be added to the AST.
		 */
		bool parse(InputStream &istream, jass_ast &ast, const std::string &fileName = "JASS file");
};

}

}

#endif
