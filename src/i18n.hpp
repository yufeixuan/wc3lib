/***************************************************************************
 *   Copyright (C) 2011 by Tamino Dauth                                    *
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

#ifndef WC3LIB_I18N_HPP
#define WC3LIB_I18N_HPP

/**
 * \file
 * Defines all include statements required for translateable strings.
 * The core modules use gettext functions for translateable strings as well as boost::format to generate them.
 */

// preparation for gettext
#ifndef WINDOWS
#include <libintl.h>
#include <locale.h>
#define _(string) gettext(string)
#else
#define _(string) string

inline void bindtextdomain(const char *domain, const char *dir)
{
}

inline void textdomain(const char *domain)
{
}

#endif

// Boost format library
#include <boost/format.hpp>

#endif
