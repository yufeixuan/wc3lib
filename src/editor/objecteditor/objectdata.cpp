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

#include "objectdata.hpp"
#include "../metadata.hpp"
#include "../mpqprioritylist.hpp"
#include "../platform.hpp"

namespace wc3lib
{

namespace editor
{

ObjectData::ObjectData(MpqPriorityList *source) : m_source(source)
{
}

map::Value::Type ObjectData::fieldType(const QString &fieldId) const
{
	qDebug() << "Getting type of field" << fieldId;

	const QString type = MetaData::fromSlkString(this->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("type")));

	if (type == "string")
	{
		return map::Value::Type::String;
	}
	else if (type == "int")
	{
		return map::Value::Type::Integer;
	}
	else if (type == "real")
	{
		return map::Value::Type::Real;
	}
	else if (type == "unreal")
	{
		return map::Value::Type::Unreal;
	}
	else if (type == "bool")
	{
		return map::Value::Type::Boolean;
	}
	else if (type == "stringList")
	{
		return map::Value::Type::StringList;
	}
	else if (type == "char")
	{
		return map::Value::Type::Character;
	}
	else if (type == "unitList")
	{
		return map::Value::Type::UnitList;
	}
	else if (type == "itemList")
	{
		return map::Value::Type::ItemList;
	}
	else if (type == "regenType")
	{
		return map::Value::Type::RegenerationType;
	}
	else if (type == "attackType")
	{
		return map::Value::Type::AttackType;
	}
	else if (type == "weaponType")
	{
		return map::Value::Type::WeaponType;
	}
	else if (type == "targetType")
	{
		return map::Value::Type::TargetType;
	}
	else if (type == "moveType")
	{
		return map::Value::Type::MoveType;
	}
	else if (type == "defenseType")
	{
		return map::Value::Type::DefenseType;
	}
	else if (type == "pathingTexture")
	{
		return map::Value::Type::PathingTexture;
	}
	else if (type == "upgradeList")
	{
		return map::Value::Type::UpgradeList;
	}
	else if (type == "abilityList")
	{
		return map::Value::Type::AbilityList;
	}
	else if (type == "heroAbilityList")
	{
		return map::Value::Type::HeroAbilityList;
	}
	else if (type == "missileArt")
	{
		return map::Value::Type::MissileArt;
	}
	else if (type == "attributeType")
	{
		return map::Value::Type::AttributeType;
	}
	else if (type == "attackBits")
	{
		return map::Value::Type::AttackBits;
	}

	// TODO how to detect Frozen Throne types, which one is int and which one boolean and which one string
	return map::Value::Type::String;
}

bool ObjectData::fieldTypeAllowsMultipleSelections(const QString &fieldId) const
{
	const QString fieldType = MetaData::fromSlkString(this->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("type")));

	return (fieldType == "unitClass"
		|| fieldType == "targetType"
		|| fieldType == "targetList"
		|| fieldType == "tilesetList");
}

ObjectData::ObjectTabEntries ObjectData::objectTabEntries(const QString &fieldType) const
{
	const QString numValues = this->objectTabData()->value(fieldType, "NumValues");
	bool ok = false;
	int num = numValues.toInt(&ok);

	if (ok)
	{
		ObjectTabEntries entries;

		for (int i = 0; i < num; ++i)
		{
			QStringList entryValues = this->objectTabData()->value(fieldType, QString("%1").arg(i, 2, 10, QChar('0'))).split(',');

			if (entryValues.size() == 2)
			{
				entries.push_back(ObjectTabEntry(entryValues[0], entryValues[1]));
			}
		}

		bool sortIt = false;

		if (this->objectTabData()->hasValue(fieldType, "Sort"))
		{
			if (this->objectTabData()->value(fieldType, "Sort") == "1")
			{
				sortIt = true;
				// TODO Flag indicating to sort the list by name in editor display
				//qSort(entries.begin(), entries.end());
			}
		}

		return entries;
	}
	else
	{
		throw Exception(boost::format(_("Invalid \"NumValues\" entry for field type %1%.")) % fieldType.toUtf8().constData());
	}

	return ObjectTabEntries();
}

map::Value ObjectData::value(const QString &fieldId, const QString &value) const
{
	const map::Value::Type type = this->fieldType(fieldId);

	switch (type)
	{
		case map::Value::Type::Integer:
		{
			int32 data = boost::numeric_cast<int32>(value.toInt());

			return map::Value(data);
		}

		case map::Value::Type::Real:
		{
			float32 data = boost::numeric_cast<float32>(value.toDouble());

			return map::Value(data);
		}

		case map::Value::Type::String:
		case map::Value::Type::RegenerationType:
		case map::Value::Type::AttackType:
		case map::Value::Type::WeaponType:
		case map::Value::Type::TargetType:
		case map::Value::Type::MoveType:
		case map::Value::Type::DefenseType:
		case map::Value::Type::PathingTexture:
		case map::Value::Type::MissileArt:
		case map::Value::Type::AttributeType:
		case map::Value::Type::AttackBits:
		{
			return map::Value(value.toUtf8().constData(), type);
		}

		case map::Value::Type::Boolean:
		{
			return map::Value((bool)value.toInt());
		}

		case map::Value::Type::Character:
		{
			return map::Value(value.toUtf8().constData()[0]);
		}

		case map::Value::Type::StringList:
		case map::Value::Type::UpgradeList:
		case map::Value::Type::UnitList:
		case map::Value::Type::ItemList:
		case map::Value::Type::AbilityList:
		case map::Value::Type::HeroAbilityList:
		{
			QStringList list = value.split(',');
			map::List vector;

			foreach (QString var, list)
			{
				vector.push_back(var.toUtf8().constData());
			}

			return map::Value(vector, type);
		}
	}

	return map::Value(0);
}

void ObjectData::modifyField(const QString &originalObjectId, const QString &customObjectId, const QString& fieldId, const map::CustomUnits::Modification &modification)
{
	const ObjectId objectId(originalObjectId, customObjectId);
	Objects::iterator iterator = this->m_objects.find(objectId);

	if (iterator == this->m_objects.end())
	{
		iterator = this->m_objects.insert(objectId, Modifications());
	}

	iterator.value().insert(fieldId, modification);
	emit fieldModification(originalObjectId, customObjectId, fieldId);
}

void ObjectData::modifyField(const QString &originalObjectId, const QString &customObjectId, const QString& fieldId, const QString& value)
{
	map::CustomUnits::Modification modification;
	modification.setValueId(map::stringToId(fieldId.toUtf8().constData()));
	modification.value() = this->value(fieldId, value);
	this->modifyField(originalObjectId, customObjectId, fieldId, modification);
}

void ObjectData::resetField(const QString &originalObjectId, const QString &customObjectId, const QString& fieldId)
{
	const ObjectId objectId(originalObjectId, customObjectId);
	Objects::iterator iterator = this->m_objects.find(objectId);

	if (iterator == this->m_objects.end())
	{
		return;
	}

	iterator.value().remove(fieldId);
	emit modificationReset(originalObjectId, customObjectId, fieldId);

	if (iterator.value().empty())
	{
		this->m_objects.erase(iterator);

		emit objectReset(originalObjectId, customObjectId);
	}
}

bool ObjectData::isFieldModified(const QString &originalObjectId, const QString &customObjectId, const QString& fieldId) const
{
	const ObjectId objectId(originalObjectId, customObjectId);
	Objects::const_iterator iterator = this->m_objects.find(objectId);

	if (iterator == this->m_objects.end())
	{
		return false;
	}

	return iterator.value().contains(fieldId);
}

void ObjectData::resetObject(const QString &originalObjectId, const QString &customObjectId)
{
	const ObjectId objectId(originalObjectId, customObjectId);
	Objects::iterator iterator = this->m_objects.find(objectId);

	if (iterator != this->m_objects.end())
	{
		this->m_objects.erase(iterator);
	}
}

bool ObjectData::isObjectModified(const QString &originalObjectId, const QString &customObjectId) const
{
	const ObjectId objectId(originalObjectId, customObjectId);
	Objects::const_iterator iterator = this->m_objects.find(objectId);

	if (iterator == this->m_objects.end())
	{
		return false;
	}

	return true;
}

void ObjectData::clearModifications()
{
	this->m_objects.clear();
}

bool ObjectData::fieldModificiation(const QString& originalObjectId, const QString& customObjectId, const QString& fieldId, map::CustomUnits::Modification &modification) const
{

	const ObjectId objectId(originalObjectId, customObjectId);
	Objects::const_iterator iterator = this->m_objects.find(objectId);

	if (iterator == this->m_objects.end())
	{
		return false;
	}

	Modifications::const_iterator iterator2 = iterator.value().find(fieldId);

	if (iterator2 == iterator.value().end())
	{
		return false;
	}

	modification = iterator2.value();

	return true;

}

QString ObjectData::fieldValue(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId) const
{
	map::CustomUnits::Modification modification;

	if (fieldModificiation(originalObjectId, customObjectId, fieldId, modification))
	{
		return valueToString(modification.value());
	}
	// Otherwise return the default value from Warcraft III
	else
	{
		return this->defaultFieldValue(originalObjectId, fieldId);
	}

	return "";
}

QString ObjectData::fieldReadableValue(const QString& originalObjectId, const QString& customObjectId, const QString& fieldId) const
{
	const QString fieldValue = this->fieldValue(originalObjectId, customObjectId, fieldId);
	const QString fieldType = MetaData::fromSlkString(this->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("type")));

	if (fieldType == "int" || fieldType == "real" || fieldType == "unreal" || fieldType == "string")
	{
		return fieldValue;
	}
	else if (fieldType == "bool")
	{
		if (fieldValue == "1")
		{
			return this->source()->sharedData()->tr("WESTRING_TRUE");
		}

		return this->source()->sharedData()->tr("WESTRING_FALSE");
	}

	const map::Txt::Section *section = this->objectTabData()->section(fieldType);

	/*
	 * If the section of the type does not exist simply return the value since there is no valid description available.
	 */
	if (section == 0)
	{
		return fieldValue;
	}

	/*
	 * Now we have to translate ALL values separately if it is a list of values.
	 */
	QStringList fieldValues = fieldValue.split(',');
	QStringList result = fieldValues;
	int missing = result.size();

	/*
	 * If there is a section for this field type search for the entry corresponding to value and get its readable description.
	 */
	for (std::size_t i = 0; i < section->entries.size() && missing > 0; ++i)
	{
		const QString sectionValue = QString::fromUtf8(section->entries[i].second.c_str());
		QStringList values = sectionValue.split(',');

		if (values.size() == 2)
		{
			/*
			 * Check for all field values if it is matching. Whenever it is matching decrease "missing" until it is 0.
			 */
			for (int i = 0; i < fieldValues.size() && missing > 0; ++i)
			{
				/*
				* It seems that values are existing like "Summoned" where it should be called "summoned"
				*/
				if (fieldValues[i].toLower() == values[0].toLower())
				{
					/*
					 * Many translatable strings contain & characters to indicate the Alt shortcut
					 * but we do not want to see this when listing the values.
					 */
					result[i] = this->source()->sharedData()->tr(values[1]).remove('&');
					missing--;
				}
			}
		}
	}

	return result.join(", ");
}

void ObjectData::importCustomUnits(const map::CustomUnits &units)
{
	this->clearModifications();

	for (map::CustomUnits::Table::size_type i = 0; i < units.originalTable().size(); ++i)
	{
		const map::CustomUnits::Unit &unit = units.originalTable()[i];
		const QString originalObjectId = map::idToString(unit.originalId()).c_str();
		const QString customObjectId = map::idToString(unit.customId()).c_str();

		for (map::CustomUnits::Unit::Modifications::size_type j = 0; j < unit.modifications().size(); ++j)
		{
			const map::CustomUnits::Modification &modification = unit.modifications()[j];
			this->modifyField(originalObjectId, customObjectId, map::idToString(modification.valueId()).c_str(), modification);
		}
	}

	qDebug() << "Custom Table size:" << units.customTable().size();

	for (map::CustomUnits::Table::size_type i = 0; i < units.customTable().size(); ++i)
	{
		const map::CustomUnits::Unit &unit = units.customTable()[i];
		const QString originalObjectId = map::idToString(unit.originalId()).c_str();
		const QString customObjectId = map::idToString(unit.customId()).c_str();

		for (map::CustomUnits::Unit::Modifications::size_type j = 0; j < unit.modifications().size(); ++j)
		{
			const map::CustomUnits::Modification &modification = unit.modifications()[j];
			this->modifyField(originalObjectId, customObjectId, map::idToString(modification.valueId()).c_str(), modification);
		}
	}

	//units.customTable()
	// TODO read all objects
}

map::CustomUnits ObjectData::customUnits() const
{
	map::CustomUnits units;

	for (Objects::const_iterator iterator = this->m_objects.begin(); iterator != this->m_objects.end(); ++iterator)
	{
		map::CustomUnits::Unit unit;
		// TODO which one is the custom id
		unit.setOriginalId(map::stringToId(iterator.key().first.toStdString()));
		unit.setCustomId(map::stringToId(iterator.key().second.toStdString()));

		foreach (map::CustomUnits::Modification modification, iterator.value())
		{
			unit.modifications().push_back(new map::CustomUnits::Modification(modification));
		}

		/*
		 * No custom ID means it is a standard unit.
		 */
		if (iterator.key().second.isEmpty())
		{
			units.originalTable().push_back(new map::CustomUnits::Unit(unit));
		}
		else
		{
			units.customTable().push_back(new map::CustomUnits::Unit(unit));
		}
	}

	return units;
}

map::CustomObjects ObjectData::customObjects() const
{
	map::CustomObjects objects = map::CustomObjects(map::CustomObjects::Type::Units);

	return objects;
}

}

}