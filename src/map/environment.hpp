/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
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

#ifndef WC3LIB_MAP_ENVIRONMENT_HPP
#define WC3LIB_MAP_ENVIRONMENT_HPP

/**
 * \defgroup environment Map Environment
 * \brief Each map's environment is defined by its terrain, shadow and pathmap.
 */

#include <boost/multi_array.hpp>

#include "platform.hpp"
#include "tilepoint.hpp"

namespace wc3lib
{

namespace map
{

/**
 * \ingroup environment
 */
class Environment : public FileFormat
{
	public:
		/**
		 * \brief Container of ids refering to tilesets.
		 */
		typedef std::vector<id> Ids;
		/**
		 * All tilepoints are stored in a two-dimensional array using the X-coordinate as the first index
		 * and the Y-coordinate as the second one.
		 *
		 * \todo We need a ptr container instead!
		 */
		typedef boost::multi_array<Tilepoint*, 2> Tilepoints;

		/**
		 * There is a physical limit of usable tilesets per environment since each tilepoint (\ref Tilepoint) only uses 4 bits to refer on its ground texture type and its cliff texture type in its corresponding environment.
		 * Indeed, you can store more than 16 different ground and cliff types since their numbers are stored as \ref uint32 values but they cannot be refered by any tilepoint of your terrain.
		 */
		static const uint32 maxTilesets;

		enum class MainTileset : byte
		{
			Ashenvale = 'A',
			Barrens = 'B',
			Felwood = 'C',
			Dungeon = 'D',
			LordaeronFall = 'F',
			Underground = 'G',
			LordaeronSummer = 'L',
			Northrend = 'N',
			VillageFall = 'Q',
			Village = 'V',
			LordaeronWinter = 'W',
			Dalaran = 'X',
			Cityscape = 'Y',
			SunkenRuins = 'Z',
			Icecrown = 'I',
			DalaranRuins = 'J',
			Outland = 'O',
			BlackCitadel = 'K'
		};

		virtual ~Environment();

		std::streamsize read(InputStream &istream);
		std::streamsize write(OutputStream &ostream) const;

		virtual const byte* fileTextId() const;
		virtual const byte* fileName() const;
		virtual uint32 latestFileVersion() const;

		uint32 mapWidth() const;
		uint32 mapHeight() const;

		MainTileset mainTileset() const;
		bool customized() const;
		Ids& groundTilesetsIds();
		const Ids& groundTilesetsIds() const;
		Ids& cliffTilesetsIds();
		const Ids& cliffTilesetsIds() const;
		uint32 maxX() const;
		uint32 maxY() const;
		float32 centerOffsetX() const;
		float32 centerOffsetY() const;

		const Tilepoints& tilepoints() const;
		Tilepoints& tilepoints();

	protected:
		void clear();

		MainTileset m_mainTileset;
		bool m_customized;
		Ids m_groundTilesetsIds;
		Ids m_cliffTilesetsIds;
		uint32 m_maxX;
		uint32 m_maxY;
		float32 m_centerOffsetX;
		float32 m_centerOffsetY;
		Tilepoints m_tilepoints;

};

inline const byte* Environment::fileTextId() const
{
	return "W3E!";
}

inline const byte* Environment::fileName() const
{
	return "war3map.w3e";
}

inline uint32 Environment::latestFileVersion() const
{
	return 11;
}

inline uint32 Environment::mapWidth() const
{
	return maxX() - 1;
}

inline uint32 Environment::mapHeight() const
{
	return maxY() - 1;
}

inline Environment::MainTileset Environment::mainTileset() const
{
	return m_mainTileset;
}

inline bool Environment::customized() const
{
	return m_customized;
}

inline Environment::Ids& Environment::groundTilesetsIds()
{
	return m_groundTilesetsIds;
}

inline const Environment::Ids& Environment::groundTilesetsIds() const
{
	return m_groundTilesetsIds;
}

inline Environment::Ids& Environment::cliffTilesetsIds()
{
	return m_cliffTilesetsIds;
}

inline const Environment::Ids& Environment::cliffTilesetsIds() const
{
	return m_cliffTilesetsIds;
}

inline uint32 Environment::maxX() const
{
	return m_maxX;
}

inline uint32 Environment::maxY() const
{
	return m_maxY;
}

inline float32 Environment::centerOffsetX() const
{
	return m_centerOffsetX;
}

inline float32 Environment::centerOffsetY() const
{
	return m_centerOffsetY;
}

inline const Environment::Tilepoints& Environment::tilepoints() const
{
	return m_tilepoints;
}

inline Environment::Tilepoints& Environment::tilepoints()
{
	return m_tilepoints;
}

}

}

#endif
