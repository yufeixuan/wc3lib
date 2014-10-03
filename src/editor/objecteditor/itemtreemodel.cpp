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

#include "itemtreemodel.hpp"
#include "itemdata.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

ItemTreeModel::ItemTreeModel(MpqPriorityList *source, QObject *parent) : ObjectTreeModel(source, parent)
{
	QStringList names;
	names << source->sharedData()->tr("WESTRING_UE_ITEMS", "WorldEditStrings");
	names << source->sharedData()->tr("WESTRING_UE_ITEMS", "WorldEditStrings");

	insertRowFolders(names, 0);
}

void ItemTreeModel::load(MpqPriorityList *source, ObjectData *objectData, QWidget *window)
{
	ItemData *itemData = dynamic_cast<ItemData*>(objectData);

	// add all entries from "UnitData.slk" to standard units in Unit Editor
	if (!itemData->itemData()->isEmpty())
	{
		for (map::Slk::Table::size_type row = 1; row < itemData->itemData()->slk().rows(); ++row)
		{
			const QString objectId = itemData->itemData()->value(row, "itemID");
			createItem(source, objectData, window, objectId, "");
		}
	}

	ObjectTreeModel::load(source, objectData, window);
}

ObjectTreeItem* ItemTreeModel::createItem(MpqPriorityList *source, ObjectData *objectData, QWidget *window, const QString &originalObjectId, const QString &customObjectId)
{
	ItemData *itemData = dynamic_cast<ItemData*>(objectData);
	const QModelIndex parentIndex = itemParent(itemData, originalObjectId, customObjectId);
	ObjectTreeItem *parent = item(parentIndex);
	insertRows(parent->children().count(), 1, parentIndex);

	ObjectTreeItem *item = parent->children().last();
	item->setObjectData(objectData);
	item->setObjectId(originalObjectId, customObjectId);

	const QString art = objectData->fieldValue(originalObjectId, customObjectId, "uico");
	item->setIcon(objectData->source()->sharedData()->icon(art, window));

	if (customObjectId.isEmpty())
	{
		this->insertStandardItem(item);
	}
	else
	{
		this->insertCustomItem(item);
	}

	return item;
}


QModelIndex ItemTreeModel::itemParent(ObjectData *objectData, const QString &originalObjectId, const QString &customObjectId)
{
	if (customObjectId.isEmpty())
	{
		return index(0, 0);
	}

	return index(1, 0);
}

}

}