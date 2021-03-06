/***************************************************************************
 *   Copyright (C) 2010 by Tamino Dauth                                    *
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

#ifndef WC3LIB_MAP_CAMPAIGN_HPP
#define WC3LIB_MAP_CAMPAIGN_HPP

#include "platform.hpp"
#include "playable.hpp"

namespace wc3lib
{

namespace map
{

class Campaign : public Playable
{
	public:
		Campaign();

	protected:
		/**
		war3campaign.w3u
		war3campaign.w3t
		war3campaign.w3a
		war3campaign.w3b
		war3campaign.w3d
		war3campaign.w3q
		war3campaign.w3f
		war3campaign.imp
		war3campaignImported\*.*
		*/
};

}

}

#endif
