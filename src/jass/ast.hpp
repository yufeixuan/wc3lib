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

#ifndef WC3LIB_JASS_AST_HPP
#define WC3LIB_JASS_AST_HPP

#include <vector>
#include <string>
#include <ostream>
#include <fstream>

#include "../spirit.hpp"

#include <boost/spirit/include/qi_symbols.hpp>
#include <boost/detail/scoped_enum_emulation.hpp>

#include "../platform.hpp"

namespace wc3lib
{

namespace jass
{

namespace qi = boost::spirit::qi;

struct jass_ast_node {
	virtual std::string type_name() const { return "bla"; }
};

/**
 * A type reference which is used in functions as return type or for types themselves as their parent type
 * can be either a pointer to the corresponding found type or if the type has not been found in the symbol table
 * which usually should cause an error in the end, the string identifier expression.
 */
struct jass_type; // forward declaration
struct jass_var_declaration;
struct jass_function_declaration;

typedef boost::variant<jass_type*, std::string> jass_type_reference;
typedef boost::variant<jass_var_declaration*, std::string> jass_var_reference;
typedef boost::variant<jass_function_declaration*, std::string> jass_function_reference;

struct jass_expression : public jass_ast_node {
};

struct jass_array_reference : public jass_expression {
	jass_var_reference var;
	jass_expression index;
};

//----------------------------------------------------------------------
// Statements
//----------------------------------------------------------------------
struct jass_statement : public jass_ast_node {
};

struct jass_statements : public jass_statement, public std::vector<jass_statement> {
};

struct jass_array_reference;

typedef boost::variant<jass_var_reference, jass_array_reference> jass_set_var;

struct jass_set : public jass_statement {
	jass_set_var var;
	jass_expression expression;
};

struct jass_function_args : public jass_statement, public std::vector<jass_expression> {
	
};

struct jass_call : public jass_statement {
	jass_function_reference function;
	jass_function_args arguments;
};

struct jass_conditional_statements : public jass_expression {
	jass_expression expression; // condition
	jass_statements statements;
};

struct jass_elseifs : public jass_statement, public std::vector<jass_conditional_statements> {
};

struct jass_ifthenelse : public jass_statement {
	jass_conditional_statements ifexpression;
	jass_elseifs elseifexpressions; // TODO optional!
	jass_statements elseexpression; // TODO optional!
};

struct jass_loop : public jass_statement {
	jass_statements statements;
};

struct jass_exitwhen : public jass_statement {
	jass_expression expression; // condition must be true so exitwhen will break the loop, expects boolean expression
};

struct jass_return : public jass_statement {
	jass_expression expression;
};

typedef boost::variant<jass_set, jass_call, jass_ifthenelse, jass_loop> jass_debug_statement;

struct jass_debug : public jass_statement {
	jass_debug_statement statement;
};

//----------------------------------------------------------------------
// Expressions
//----------------------------------------------------------------------
BOOST_SCOPED_ENUM_START(jass_binary_operator) {
	Plus,
	Minus,
	Multiply,
	Divide,
	Greater,
	Less,
	Equal,
	NotEqual,
	GreaterEqual,
	LessEqual,
	And,
	Or
};
BOOST_SCOPED_ENUM_END

struct jass_binary_operators : qi::symbols<char, BOOST_SCOPED_ENUM(jass_binary_operator)> {
	
	jass_binary_operators()
	{
		name("JASS binary operators");
		add
			("+", jass_binary_operator::Plus)
			("-", jass_binary_operator::Minus)
			("*", jass_binary_operator::Multiply)
			("/", jass_binary_operator::Divide)
			(">", jass_binary_operator::Greater)
			("<", jass_binary_operator::Less)
			("==", jass_binary_operator::Equal)
			("!=", jass_binary_operator::NotEqual)
			(">=", jass_binary_operator::GreaterEqual)
			("<=", jass_binary_operator::LessEqual)
			("and", jass_binary_operator::And)
			("or", jass_binary_operator::Or)
		;
	}

	~jass_binary_operators() {
	}
};

struct jass_binary_operation : public jass_expression {
	jass_expression first_expression;
	jass_binary_operator op;
	jass_expression second_expression;
};

BOOST_SCOPED_ENUM_START(jass_unary_operator) {
	Plus,
	Minus,
	Not
};
BOOST_SCOPED_ENUM_END

struct jass_unary_operators : qi::symbols<char, BOOST_SCOPED_ENUM(jass_unary_operator)> {
	
	jass_unary_operators()
	{
		name("JASS unary operators");
		add
			("+", jass_unary_operator::Plus)
			("-", jass_unary_operator::Minus)
			("not", jass_unary_operator::Not)
		;
	}

	~jass_unary_operators() {
	}
};

struct jass_unary_operation : public jass_expression {
	jass_unary_operator op;
	jass_expression expression;
};

struct jass_function_call : public jass_expression {
	jass_function_reference function;
	jass_function_args arguments;
};

struct jass_function_ref : public jass_expression {
	jass_function_reference function;
};

struct jass_null : public jass_ast_node {
};

// TODO use charArray, boost::array does not work with Spirit
typedef std::vector<char> fourcc;

typedef boost::variant<int32, fourcc, float32, bool, string, jass_null> jass_const_value;

struct jass_const : public jass_expression {
	jass_const_value value;
};

//----------------------------------------------------------------------
// Local Declarations
//----------------------------------------------------------------------
struct jass_local_declaration : public jass_ast_node {
};

struct jass_var_declaration : public jass_local_declaration {
	jass_type_reference type;
	bool is_array;
	string identifier;
	jass_expression assignment; // TODO optional
	
};

struct jass_locals : public jass_local_declaration, public std::vector<jass_var_declaration> {
};

struct jass_var_declarations : qi::symbols<char, jass_var_declaration> {
};


//----------------------------------------------------------------------
// Global Declarations
//----------------------------------------------------------------------
struct jass_global_declaration : public jass_ast_node {
};

struct jass_type : public jass_global_declaration {
	string identifier;
	jass_type_reference parent;
	
	jass_type() : parent("") { }
	jass_type(const string &identifier, jass_type *parent) : identifier(identifier), parent(parent) { }
	// if symbol table does not contain the parent type
	jass_type(const string &identifier) : identifier(identifier), parent("") { }
};

struct jass_types : public jass_global_declaration, public std::vector<jass_type> {
};

struct jass_type_declarations : qi::symbols<char, jass_type>, public jass_global_declaration {
	
	jass_type_declarations()
	{
		name("JASS types");
		// add basic JASS types which do not have to be declared
		add
			("integer", jass_type("integer", 0))
			("real", jass_type("real", 0))
			("code", jass_type("code", 0))
			("handle", jass_type("handle", 0))
			("boolean", jass_type("boolean", 0))
			("string", jass_type("string", 0))
		;
	}

	~jass_type_declarations() {
	}
};

struct jass_global : public jass_global_declaration {
	bool is_constant;
	jass_var_declaration declaration;
};

/*
 * globals
 * 	<vars|constants>
 * endglobals
 * 
 * All global and constant statements have to be placed between "globals" and "endglobals".
 * This node inherits std::vector and can be filled with global and constant declarations.
 */
struct jass_globals : public jass_global_declaration, public std::vector<jass_global> {
};

struct jass_function_parameter : public jass_global_declaration {
	jass_type_reference type;
	string identifier;
};

struct jass_function_parameters : public jass_global_declaration, public std::vector<jass_function_parameter> {
	
};

struct jass_function_declaration : public jass_global_declaration {
	string identifier;
	jass_function_parameters parameters;
	jass_type_reference return_type;
};

// TODO grammar states only natives can be constant, wrong?
struct jass_native : public jass_global_declaration {
	bool is_constant;
	jass_function_declaration declaration;
};

struct jass_natives : public jass_global_declaration, public std::vector<jass_native> {
};

struct jass_function_declarations : qi::symbols<char, jass_function_declaration> {
};

struct jass_function : public jass_global_declaration {
	bool is_constant;
	jass_function_declaration declaration;
	jass_locals locals;
	jass_statements statements;
};

struct jass_functions : public jass_global_declaration, public std::vector<jass_function> {
};

struct jass_declarations : public jass_global_declaration {
	jass_types types;
	jass_globals globals;
	jass_natives natives;
};

struct jass_file : public jass_global_declaration {
	std::string path;
	jass_declarations declarations;
	jass_functions functions;
};

struct jass_files :  public jass_global_declaration, public std::vector<jass_file> {
};

struct jass_ast {
	jass_files files;
};

}

}

/*
 * The specialization is required for proper debug output of Spirit.
 * It prints attributes of rules and therefore attributes of type jass_ast_node which misses a stream output operator.
 * Unfortunately we have to define it for every type because it does not detect inheritance and virtual methods.
 */
// your specialization needs to be in namespace boost::spirit::traits
// https://stackoverflow.com/questions/5286720/how-to-define-streaming-operator-for-boostspiritqi-and-stdlist-container
// https://svn.boost.org/trac/boost/ticket/9803
namespace boost { namespace spirit { namespace traits
{
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_ast_node>
	{
		static void call(Out& out, wc3lib::jass::jass_ast_node const& val)
		{
			out << val.type_name();
		}
	};
	
	//----------------------------------------------------------------------
	// Statements
	//----------------------------------------------------------------------
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_statement>
	{
		static void call(Out& out, wc3lib::jass::jass_statement const& val)
		{
			out << val.type_name();
		}
	};
	
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_statements>
	{
		static void call(Out& out, wc3lib::jass::jass_statements const& val)
		{
			out << val.type_name();
		}
	};
	
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_set>
	{
		static void call(Out& out, wc3lib::jass::jass_set const& val)
		{
			out << val.type_name();
		}
	};
	
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_function_args>
	{
		static void call(Out& out, wc3lib::jass::jass_function_args const& val)
		{
			out << val.type_name();
		}
	};
	
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_call>
	{
		static void call(Out& out, wc3lib::jass::jass_call const& val)
		{
			out << val.type_name();
		}
	};
	
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_conditional_statements>
	{
		static void call(Out& out, wc3lib::jass::jass_conditional_statements const& val)
		{
			out << val.type_name();
		}
	};
	
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_elseifs>
	{
		static void call(Out& out, wc3lib::jass::jass_elseifs const& val)
		{
			out << val.type_name();
		}
	};

	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_ifthenelse>
	{
		static void call(Out& out, wc3lib::jass::jass_ifthenelse const& val)
		{
			out << val.type_name();
		}
	};
	
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_loop>
	{
		static void call(Out& out, wc3lib::jass::jass_loop const& val)
		{
			out << val.type_name();
		}
	};
	
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_exitwhen>
	{
		static void call(Out& out, wc3lib::jass::jass_exitwhen const& val)
		{
			out << val.type_name();
		}
	};
	
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_return>
	{
		static void call(Out& out, wc3lib::jass::jass_return const& val)
		{
			out << val.type_name();
		}
	};
	
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_debug>
	{
		static void call(Out& out, wc3lib::jass::jass_debug const& val)
		{
			out << val.type_name();
		}
	};
	
	//----------------------------------------------------------------------
	// Expressions
	//----------------------------------------------------------------------
	
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_expression>
	{
		static void call(Out& out, wc3lib::jass::jass_expression const& val)
		{
			out << val.type_name();
		}
	};
	
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_binary_operation>
	{
		static void call(Out& out, wc3lib::jass::jass_binary_operation const& val)
		{
			out << val.type_name();
		}
	};
	
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_unary_operation>
	{
		static void call(Out& out, wc3lib::jass::jass_unary_operation const& val)
		{
			out << val.type_name();
		}
	};
	
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_function_call>
	{
		static void call(Out& out, wc3lib::jass::jass_function_call const& val)
		{
			out << val.type_name();
		}
	};
	
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_array_reference>
	{
		static void call(Out& out, wc3lib::jass::jass_array_reference const& val)
		{
			out << val.type_name();
		}
	};
	
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_function_ref>
	{
		static void call(Out& out, wc3lib::jass::jass_function_ref const& val)
		{
			out << val.type_name();
		}
	};
	
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_null>
	{
		static void call(Out& out, wc3lib::jass::jass_null const& val)
		{
			out << val.type_name();
		}
	};
	
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_const>
	{
		static void call(Out& out, wc3lib::jass::jass_const const& val)
		{
			out << val.type_name();
		}
	};

	//----------------------------------------------------------------------
	// Local Declarations
	//----------------------------------------------------------------------

	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_local_declaration>
	{
		static void call(Out& out, wc3lib::jass::jass_local_declaration const& val)
		{
			out << val.type_name();
		}
	};
	
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_var_declaration>
	{
		static void call(Out& out, wc3lib::jass::jass_var_declaration const& val)
		{
			out << val.type_name();
		}
	};
	
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_locals>
	{
		static void call(Out& out, wc3lib::jass::jass_locals const& val)
		{
			out << val.type_name();
		}
	};
	
	//----------------------------------------------------------------------
	// Global Declarations
	//----------------------------------------------------------------------
	
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_native>
	{
		static void call(Out& out, wc3lib::jass::jass_native const& val)
		{
			out << val.type_name();
		}
	};

	// jass_types
	// symbol table
	template <typename Out>
	struct print_attribute_debug<Out, wc3lib::jass::jass_types>
	{
		static void call(Out& out, wc3lib::jass::jass_types const& val)
		{
			out << val.type_name();
		}
	};
}}}

#endif
