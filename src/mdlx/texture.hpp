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

#ifndef WC3LIB_MDLX_TEXTURE_HPP
#define WC3LIB_MDLX_TEXTURE_HPP

#include "groupmdxblockmember.hpp"
#include "mdlxproperty.hpp"
#include "textures.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * MDL tag "Bitmap".
 */
class Texture : public GroupMdxBlockMember
{
	public:
		enum class Wrapping : long32
		{
			WrapWidth = 1,
			WrapHeight = 2,
			Both = 3
		};

		static const std::size_t texturePathSize = 0x100;

		Texture(class Textures *textures);

		class Textures* textures() const;
		ReplaceableId replaceableId() const;
		void setTexturePath(const byte texturePath[texturePathSize]);
		/**
		 * \return Returns ASCII texture path with length \ref texturePathSize.
		 */
		const byte* texturePath() const;
		long32 unknown0() const;
		Wrapping wrapping() const;

		virtual std::streamsize readMdl(istream &istream);
		virtual std::streamsize writeMdl(ostream &ostream) const;
		virtual std::streamsize readMdx(istream &istream);
		virtual std::streamsize writeMdx(ostream &ostream) const;

	protected:
		ReplaceableId m_replaceableId;
		byte m_texturePath[texturePathSize]; //(0x100 bytes)
		long32 m_unknown0; //(0)
		Wrapping m_wrapping; //(1:WrapWidth;2:WrapHeight;3:Both)
};

inline class Textures* Texture::textures() const
{
	return boost::polymorphic_cast<class Textures*>(this->parent());
}

inline ReplaceableId Texture::replaceableId() const
{
	return this->m_replaceableId;
}

inline void Texture::setTexturePath(const byte texturePath[Texture::texturePathSize])
{
	memcpy(this->m_texturePath, texturePath, Texture::texturePathSize);
}

inline const char* Texture::texturePath() const
{
	return this->m_texturePath;
}

inline long32 Texture::unknown0() const
{
	return this->m_unknown0;
}

inline Texture::Wrapping Texture::wrapping() const
{
	return this->m_wrapping;
}

}

}

#endif
