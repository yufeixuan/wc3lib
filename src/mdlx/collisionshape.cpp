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

#include "collisionshape.hpp"
#include "../utilities.hpp"

namespace wc3lib
{

namespace mdlx
{

CollisionShape::CollisionShape(class CollisionShapes *collisionShapes) : Object(collisionShapes->mdlx()), GroupMdxBlockMember(collisionShapes, "CollisionShape")
{
}

std::streamsize CollisionShape::readMdl(istream &istream) throw (class Exception)
{
	return 0;
}

std::streamsize CollisionShape::writeMdl(ostream &ostream) const throw (class Exception)
{
	return 0;
}

std::streamsize CollisionShape::readMdx(istream &istream) throw (class Exception)
{
	std::streamsize size = Object::readMdx(istream);
	long32 shape;
	wc3lib::read(istream, shape, size);
	this->m_shape = static_cast<BOOST_SCOPED_ENUM(Shape)>(shape);
	wc3lib::read(istream, this->m_vertexData, size);

	if (this->m_shape == Shape::Box)
		wc3lib::read(istream, this->m_boxVertexData, size);
	else
		wc3lib::read(istream, this->m_boundsRadius, size);

	return size;
}

std::streamsize CollisionShape::writeMdx(ostream &ostream) const throw (class Exception)
{
	std::streamsize size = Object::writeMdx(ostream);
	wc3lib::write(ostream, static_cast<long32>(this->m_shape), size);
	wc3lib::write(ostream, this->m_vertexData, size);

	if (this->m_shape == Shape::Box)
		wc3lib::write(ostream, this->m_boxVertexData, size);
	else
		wc3lib::write(ostream, this->m_boundsRadius, size);

	return size;
}

}

}
