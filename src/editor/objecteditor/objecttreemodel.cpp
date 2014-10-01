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

#include <QtGui>

#include "objecttreemodel.hpp"
#include "objectdata.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

ObjectTreeModel::ObjectTreeModel(MpqPriorityList *source, QObject *parent) : QAbstractItemModel(parent), m_source(source), m_objectData(0)
{
}

ObjectTreeModel::~ObjectTreeModel()
{
	qDeleteAll(this->m_topLevelItems);
}

QVariant ObjectTreeModel::data(const QModelIndex &index, int role) const
{

     if (!index.isValid())
     {
         return QVariant();
     }

     ObjectTreeItem *item = static_cast<ObjectTreeItem*>(index.internalPointer());

     switch (role)
     {
	     case Qt::UserRole:
	     {
		     return QString("%1:%2").arg(item->originalObjectId()).arg(item->customObjectId());
	     }

	     case Qt::DisplayRole:
	     {
		     return item->text(false);
	     }

	     case Qt::DecorationRole:
	     {
		     if (item->isFolder())
		     {
			     return source()->sharedData()->worldEditDataIcon("UEIcon_UnitCategory", "WorldEditArt", 0);
		     }

		     return item->icon();
	     }

	     case Qt::ForegroundRole:
	     {
		     if (item->hasModifiedObject())
		     {
			     return Qt::magenta;
		     }

		     return Qt::black;
	     }
     }

     return QVariant();
}

int ObjectTreeModel::columnCount(const QModelIndex &parent) const
{
	return 1;
}

int ObjectTreeModel::rowCount(const QModelIndex &parent) const
{
	ObjectTreeItem *parentItem = 0;

	if (!parent.isValid())
	{
		return m_topLevelItems.size();
	}
	else
	{
		parentItem = static_cast<ObjectTreeItem*>(parent.internalPointer());
	}

	return parentItem->children().size();
}

QModelIndex ObjectTreeModel::parent(const QModelIndex &index) const
{
	 if (!index.isValid())
	 {
		return QModelIndex();
	 }

	ObjectTreeItem *childItem = static_cast<ObjectTreeItem*>(index.internalPointer());
	ObjectTreeItem *parentItem = childItem->parent();

	if (parentItem == 0)
	{
		return QModelIndex();
	}

	int parentRow = 0;

	/*
	 * If it is a top level item we have to calculate the row manually.
	 */
	if (parentItem->parent() == 0)
	{
		parentRow = m_topLevelItems.indexOf(parentItem);

		if (parentRow == -1)
		{
			qDebug() << "Missing top level item:" << parentItem->text(false);
		}
	}
	else
	{
		parentRow = parentItem->row();
	}

	return createIndex(parentRow, 0, parentItem);
}

QModelIndex ObjectTreeModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent) || column != 0)
	{
		return QModelIndex();
	}

	ObjectTreeItem *parentItem;

	if (!parent.isValid())
	{
		parentItem = 0;
	}
	else
	{
		parentItem = static_cast<ObjectTreeItem*>(parent.internalPointer());
	}

	ObjectTreeItem *childItem = 0;

	if (parentItem != 0)
	{
		childItem = parentItem->child(row);
	}
	else if (row >= 0 && row < m_topLevelItems.size())
	{
		childItem = m_topLevelItems.at(row);
	}
	else
	{
		qDebug() << "Error error error:" << row;
	}

	if (childItem != 0)
	{
		return createIndex(row, column, childItem);
	}
	else
	{
		return QModelIndex();
	}
}

QVariant ObjectTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	// no header data available
	return QVariant();
}

void ObjectTreeModel::insertRowFolders(const QStringList& folderNames, int row, QModelIndex parent)
{
	insertRows(row, folderNames.size(), parent);

	foreach (QString name, folderNames)
	{
		const QModelIndex index = this->index(row, 0, parent);
		this->item(index)->setFolderText(name);

		++row;
	}
}

bool ObjectTreeModel::insertRows(int row, int count, const QModelIndex &parent)
{
	ObjectTreeItem *parentItem = item(parent);
	ObjectTreeItem::Children children;

	if (parentItem != 0)
	{
		children = parentItem->children();
	}

	const int last = row + count - 1;
	beginInsertRows(parent, row, last);

	for (int i = row; i < last + 1; ++i)
	{
		ObjectTreeItem *item = new ObjectTreeItem(0, "", "", parentItem);

		if (parentItem != 0)
		{
			item->setObjectData(parentItem->objectData());

			if (i >= children.size())
			{
				children.push_back(item);
			}
			else
			{
				children[i] = item;
			}
		}
		else
		{
			this->insertTopLevelItem(item);
		}
	}

	if (parentItem != 0)
	{
		parentItem->setChildren(children);
	}

	endInsertRows();


	return true;
}

bool ObjectTreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
	ObjectTreeItem *parentItem = item(parent);

	if (row + count > parentItem->children().count())
	{
		return false;
	}

	beginRemoveRows(parent, row, row + count - 1);

	for (int i = row; i < count; ++i)
	{
		ObjectTreeItem *item = parentItem->children().takeAt(i);
		delete item;
	}


	endRemoveRows();

	return true;
}

void ObjectTreeModel::clearCustomItems()
{
	foreach (ObjectTreeItem *item, this->m_customItems)
	{
		delete item;
	}

	this->m_customItems.clear();
}

ObjectTreeItem* ObjectTreeModel::item(const QModelIndex &index) const
{
	return (ObjectTreeItem*)(index.internalPointer());
}

void ObjectTreeModel::load(MpqPriorityList* source, ObjectData *objectData, QWidget *window)
{
	/*
	 * Disconnect from old invalid object data first.
	 */
	if (this->objectData() != 0)
	{
		disconnect(this->objectData(), 0, this, 0);
		qDebug() << "Disconnect from" << this->objectData();

		// remove standard objects
		// TODO is always set?
		this->removeRows(0, m_topLevelItems.size(), QModelIndex());
	}

	QStack<ObjectTreeItem*> items;

	foreach (ObjectTreeItem *item, m_topLevelItems)
	{
		items.push(item);
	}

	while (!items.isEmpty())
	{
		ObjectTreeItem *item = items.pop();

		if (item->isFolder())
		{
			item->setObjectData(objectData);

			foreach (ObjectTreeItem *child, item->children())
			{
				items.push(child);
			}
		}
	}

	connect(objectData, SIGNAL(objectCreation(const QString&, const QString&)), this, SLOT(createObject(const QString&, const QString&)));
	connect(objectData, SIGNAL(objectRemoval(const QString&, const QString&)), this, SLOT(removeObject(const QString&, const QString&)));
	connect(objectData, SIGNAL(objectReset(const QString&, const QString&)), this, SLOT(resetObject(const QString&, const QString&)));
	connect(objectData, SIGNAL(fieldModification(const QString &, const QString &, const QString &)), this, SLOT(modifyField(const QString&, const QString&, const QString&)));
	this->m_source = source;
	this->m_objectData = objectData;
}

void ObjectTreeModel::createObject(const QString &originalObjectId, const QString &customObjectId)
{
	ObjectData *objectData = dynamic_cast<ObjectData*>(sender());

	// TODO insert item
	// createItem should emit all signals for adding new rows
	if (objectData != 0)
	{
		ObjectTreeItem *item = this->createItem(objectData->source(), objectData, 0, originalObjectId, customObjectId);
	}
}

void ObjectTreeModel::removeObject(const QString &originalObjectId, const QString &customObjectId)
{
	ObjectData *objectData = dynamic_cast<ObjectData*>(sender());

	if (objectData != 0)
	{
		ObjectTreeItem *item = this->item(originalObjectId, customObjectId);

		if (item != 0)
		{
			ObjectTreeItem *parent = item->parent();
			QModelIndex parentIndex;
			int row = 0;

			if (parent != 0)
			{
				parentIndex = this->parent(item->modelIndex(this));
				row = item->row();
			}
			else
			{
				row = this->topLevelRow(item);
			}

			removeRows(row, 1, parentIndex);
		}
	}
}

void ObjectTreeModel::resetObject(const QString& originalObjectId, const QString& customObjectId)
{
	ObjectTreeItem *item = this->item(originalObjectId, customObjectId);

	if (item != 0)
	{
		QModelIndex index = item->modelIndex(this);

		emit dataChanged(index, index);
	}
	else
	{
		qDebug() << "Missing" << originalObjectId << customObjectId;
	}
}

void ObjectTreeModel::modifyField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId)
{
	ObjectData *objectData = boost::polymorphic_cast<ObjectData*>(QObject::sender());
	ObjectTreeItem *item = this->item(originalObjectId, customObjectId);

	if (item != 0)
	{
		QModelIndex topLeft;

		// TODO only use the top as left if it is the first modification!
		// TODO a function should provide if there is these two items.
		if (customObjectId.isEmpty())
		{
			topLeft = index(0, 0);
		}
		else
		{
			topLeft = index(1, 0);
		}

		const QModelIndex bottomRight = item->modelIndex(this);

		/*
		 * Signal that data have changed.
		 * For example if the name has changed it will be updated automatically in the view.
		 * The whole object tree from the top level to the object item has to be updated since the color changes if the item got its initial modification.
		 */
		emit dataChanged(topLeft, bottomRight);

		const QModelIndex parentIndex = this->itemParent(objectData, originalObjectId, customObjectId);
		ObjectTreeItem *parent = this->item(parentIndex);

		/*
		 * If the item would have a new parent it has been moved.
		 * For example it would have a new parent if the race or unit type has been changed.
		 */
		if (item->parent() != parent)
		{
			qDebug() << "Move the row!!";
			beginMoveRows(bottomRight, item->row(), item->row(), parent->modelIndex(this), parent->children().size());
			endMoveRows();

		}
	}
	else
	{
		qDebug() << "Missing" << originalObjectId << customObjectId;
	}
}

}

}
