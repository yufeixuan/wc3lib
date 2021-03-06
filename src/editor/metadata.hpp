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

#ifndef WC3LIB_EDITOR_METADATA_HPP
#define WC3LIB_EDITOR_METADATA_HPP

#include <QHash>

#include "resource.hpp"
#include "../map.hpp"

namespace wc3lib
{

namespace editor
{

class TextSourceInterface
{
	public:
		virtual ~TextSourceInterface();

		virtual void read(istream &in) = 0;
		virtual void write(ostream &out) const = 0;
		virtual bool hasValue(const QString &rowKey, const QString &columnKey) const = 0;
		virtual QString value(const QString &rowKey, const QString &columnKey) const = 0;
		virtual void setValue(const QString &rowKey, const QString &columnKey, const QString &value) = 0;
		virtual int rows() const = 0;
		virtual bool hasValue(int row, const QString &columnKey) const = 0;
		virtual QString value(int row, const QString &columnKey) const = 0;
		virtual bool isEmpty() const = 0;
		virtual void clear() = 0;

		/**
		 * Clones the text source on the heap and returns the cloned instance.
		 * This method is required by the copy constructor of \ref MetaData to create an exact copy of another meta data.
		 * \return Returns the cloned instance on the heap.
		 */
		virtual TextSourceInterface* clone() const = 0;
};

/**
 * \brief \ref MetaData backend for .slk files.
 *
 * \note Ignores cases of keys since .slk file section and value names are not case sensitive in Warcraft III.
 */
class SlkTextSource : public TextSourceInterface
{
	public:
		/**
		* Values of the first cells are used to get the corresponding number of row or column.
		*/
		typedef QHash<QString, map::Slk::Table::size_type> SlkKeys;

		map::Slk& slk();
		const map::Slk& slk() const;

		const SlkKeys& columnKeys() const;
		const SlkKeys& rowKeys() const;

		virtual bool isEmpty() const override;
		virtual void clear() override;
		virtual bool hasValue(const QString &rowKey, const QString &columnKey) const override;
		virtual QString value(const QString &rowKey, const QString &columnKey) const override;
		virtual void setValue(const QString &rowKey, const QString &columnKey, const QString &value) override;
		virtual int rows() const override;
		virtual bool hasValue(int row, const QString& columnKey) const override;
		virtual QString value(int row, const QString& columnKey) const override;
		virtual void read(istream &in) override;
		virtual void write(ostream& out) const override;
		virtual TextSourceInterface* clone() const override;

	private:
		map::Slk m_slk;
		SlkKeys m_columnKeys;
		SlkKeys m_rowKeys;

};

inline map::Slk& SlkTextSource::slk()
{
	return this->m_slk;
}

inline const map::Slk& SlkTextSource::slk() const
{
	return this->m_slk;
}

inline const SlkTextSource::SlkKeys& SlkTextSource::columnKeys() const
{
	return this->m_columnKeys;
}

inline const SlkTextSource::SlkKeys& SlkTextSource::rowKeys() const
{
	return this->m_rowKeys;
}


inline bool SlkTextSource::isEmpty() const
{
	return this->m_slk.table().empty();
}

inline void SlkTextSource::clear()
{
	this->m_columnKeys.clear();
	this->m_rowKeys.clear();
	this->m_slk.clear();
}

inline int SlkTextSource::rows() const
{
	return this->rowKeys().size();
}

/**
 * \brief \ref MetaData backend for .txt files.
 *
 * \note Ignores cases of keys since .txt file section and value names are not case sensitive in Warcraft III.
 */
class TxtTextSource : public TextSourceInterface
{
	public:
		/**
		* Use the section names as keys for the sections.
		*/
		typedef QHash<QString, const map::Txt::Section*> TxtSectionKeys;

		/**
		* The key to one TXT entry is its section and its value key.
		*/
		typedef QPair<const map::Txt::Section*, QString> TxtEntryKey;
		/**
		* Use the key of the key value pair as key for the entry.
		*/
		typedef QHash<TxtEntryKey, const map::Txt::Entry*> TxtEntryKeys;

		map::Txt& txt();
		const map::Txt& txt() const;

		const TxtSectionKeys& sectionKeys() const;
		const TxtEntryKeys& entryKeys() const;

		/**
		 * Can only be used for a stored TXT and returns the corresponding section of \p key
		 * which is the section's name.
		 */
		const map::Txt::Section* section(const QString &key) const;
		const map::Txt::Entry* entry(const QString &sectionKey, const QString &key) const;

		virtual bool isEmpty() const override;
		virtual void clear() override;
		virtual bool hasValue(const QString &rowKey, const QString &columnKey) const override;
		virtual bool hasValue(int row, const QString& columnKey) const override;
		virtual QString value(const QString &rowKey, const QString &columnKey) const override;
		virtual void setValue(const QString &rowKey, const QString &columnKey, const QString &value) override;
		virtual int rows() const override;
		virtual QString value(int row, const QString& columnKey) const override;
		virtual void read(istream &in) override;
		virtual void write(ostream &out) const override;
		virtual TextSourceInterface* clone() const override;

	private:
		map::Txt m_txt;
		TxtSectionKeys m_sectionKeys;
		TxtEntryKeys m_entryKeys;

};

inline map::Txt& TxtTextSource::txt()
{
	return this->m_txt;
}

inline const map::Txt& TxtTextSource::txt() const
{
	return this->m_txt;
}

inline const TxtTextSource::TxtSectionKeys& TxtTextSource::sectionKeys() const
{
	return this->m_sectionKeys;
}

inline const TxtTextSource::TxtEntryKeys& TxtTextSource::entryKeys() const
{
	return this->m_entryKeys;
}

inline const map::Txt::Section* TxtTextSource::section(const QString &key) const
{
	TxtSectionKeys::const_iterator iterator = this->sectionKeys().find(key);

	if (iterator == this->sectionKeys().end())
	{
		return 0;
	}

	return iterator.value();
}

inline const map::Txt::Entry* TxtTextSource::entry(const QString &sectionKey, const QString& key) const
{
	const map::Txt::Section *section = this->section(sectionKey);

	if (section == 0)
	{
		return 0;
	}

	TxtEntryKeys::const_iterator iterator = this->entryKeys().find(TxtEntryKey(section, key));

	if (iterator == this->entryKeys().end())
	{
		return 0;
	}

	return iterator.value();
}

inline bool TxtTextSource::isEmpty() const
{
	return this->m_txt.sections().empty();
}

inline void TxtTextSource::clear()
{
	this->m_entryKeys.clear();
	this->m_sectionKeys.clear();
	this->m_txt.sections().clear();
}

inline int TxtTextSource::rows() const
{
	return this->m_sectionKeys.size();
}

/**
 * \note Ignores the column key. Only uses the row key for string keys.
 */
class MapStringsTextSource : public TextSourceInterface
{
	public:
		/**
		 * Use the entry keys as key.
		 */
		typedef QHash<int, const map::MapStrings::Entry*> EntryKeys;

		map::MapStrings& mapStrings();
		const map::MapStrings& mapStrings() const;

		const EntryKeys& entryKeys() const;

		const map::MapStrings::Entry* entry(int key) const;

		virtual bool isEmpty() const override;
		virtual void clear() override;
		virtual bool hasValue(const QString &rowKey, const QString &columnKey) const override;
		virtual QString value(const QString &rowKey, const QString &columnKey) const override;
		virtual void setValue(const QString &rowKey, const QString &columnKey, const QString &value) override;
		virtual int rows() const override;
		virtual bool hasValue(int row, const QString& columnKey) const override;
		virtual QString value(int row, const QString& columnKey) const override;
		virtual void read(istream &in) override;
		virtual void write(ostream &out) const override;
		virtual TextSourceInterface* clone() const override;

	private:
		map::MapStrings m_mapStrings;
		EntryKeys m_entryKeys;

};

inline map::MapStrings& MapStringsTextSource::mapStrings()
{
	return this->m_mapStrings;
}

inline const map::MapStrings& MapStringsTextSource::mapStrings() const
{
	return this->m_mapStrings;
}

inline const MapStringsTextSource::EntryKeys& MapStringsTextSource::entryKeys() const
{
	return this->m_entryKeys;
}

inline const map::MapStrings::Entry* MapStringsTextSource::entry(int key) const
{
	EntryKeys::const_iterator iterator = this->entryKeys().find(key);

	if (iterator == this->entryKeys().end())
	{
		return 0;
	}

	return iterator.value();
}

inline bool MapStringsTextSource::isEmpty() const
{
	return this->mapStrings().entries().empty();
}

inline void MapStringsTextSource::clear()
{
	this->m_entryKeys.clear();
	this->mapStrings().entries().clear();
}

inline int MapStringsTextSource::rows() const
{
	return this->m_entryKeys.size();
}

/**
 * \brief Base class for all possible meta data formats (units, abilities, upgrades etc.).
 *
 * This class can either store a SYLK file or a .txt file and provides a unified interface
 * to retrieve any value by its keys.
 *
 * Retrieving values by keys is much faster than searching for them with linear complexity.
 *
 * SLK or .txt files in Warcraft III usually provide a row and column item or a section and entry name (for .txt files)
 * from which the required value can be identified.
 * This class allows fast access to both kind of values through hashing.
 * Use \ref value() to get a value by both keys.
 *
 * With \ref hasValue() it can be checked if a value does actually exist.
 *
 * Uses \ref map::Slk to load a SYLK file which contains all meta data.
 * In addition to \ref map::Slk it stores columns and rows by keys (their first cell values) through hashing.
 * This allows fast access of columns or rows by keys which is usually required for meta data.
 *
 * Uses \ref map::Txt to load a .txt file which contain additional object data such as strings.
 *
 * Both kind of files use the UTF-8 encoding by default.
 *
 * \note Theoretically it is possible to store several values under the same keys using multiple equally named rows, columns, sections or entries but this class assumes that each identifier is picked uniquely. An exception might be entries in sections since there can be at least one entry per section so if multiple sections have the same entry it still would be unique in that section which is enough to use it as a hash value.
 *
 * \todo Add FDF support.
 */
class MetaData : public Resource
{
	public:
		MetaData(const QUrl &url);
		/**
		 * Creates a copy of \p other.
		 */
		MetaData(const MetaData &other);
		virtual ~MetaData();

		virtual void clear();

		virtual void load() override;
		virtual void reload() override;
		virtual void save(const QUrl &url) const override;

		/**
		 * \return Returns an instance of the actual source which is hidden behind this class.
		 */
		TextSourceInterface* textSource() const;

		/**
		 * Searches for value \p rowKey in the first column and for value \p columnKey in the
		 * first row.
		 * If both values could be found it returns the corresponding cell value.
		 *
		 * This allows you fast retrieving of values by two keys which is usually
		 * what you need to do to get SLK or TXT data for Warcraft III.
		 *
		 * \note For TXT files the \p rowKey is the section name and the \p columnKey is the entry name of an entry in that section.
		 *
		 * \param rowKey The key of the row or section. Quotes will be added automatically for SLK files.
		 * \param columnKey The key of the column or the entry. Quotes will be added automatically for SLK files.
		 *
		 * \return Returns the keys matching value. If no value is found it returns an empty string. For SLK and TXT files quotes are removed automatically.
		 *
		 * \note It removes starting and ending quotes automatically.
		 *
		 * \sa hasValue()
		 */
		QString value(const QString &rowKey, const QString &columnKey) const;
		void setValue(const QString &rowKey, const QString &columnKey, const QString &value);

		/**
		 * \sa value()
		 */
		bool hasValue(const QString &rowKey, const QString &columnKey) const;
		int rows() const;
		bool hasValue(int row, const QString &columnKey) const;
		QString value(int row, const QString &columnKey) const;

		/**
		 * \return Returns true if the underlying file entries are empty.
		 */
		bool isEmpty() const;

		/**
		 * Cuts " characters at start and end if available and returns resulting string.
		 */
		static QString fromSlkString(const QString &value);
		/**
		 * Adds double quotes to the beginning and the end if they are not there already.
		 * \return Returns \p value with double quotes at the beginning and at the end.
		 */
		static QString toSlkString(const QString &value);

		/**
		 * Parses \p value and returns the token at position \p index.
		 * Tokens are separated by ',' characters and escaped by double quotes.
		 * Therefore ',' characters between double quotes are ignored.
		 *
		 * Values by indices are often required for level values of object data.
		 */
		static QString valueByIndex(const QString &value, int index);

		/**
		 * Converts \p value to a native file path.
		 */
		static QString fromFilePath(const QString &value);

	protected:
		TextSourceInterface *m_textSource;
};

inline TextSourceInterface* MetaData::textSource() const
{
	return this->m_textSource;
}

inline bool MetaData::isEmpty() const
{
	if (this->textSource() == 0)

	{
		return true;
	}

	return this->textSource()->isEmpty();
}

inline QString MetaData::fromSlkString(const QString &value)
{
	QString result = value;

	if (result.startsWith('"'))
	{
		result = result.mid(1);
	}

	if (result.endsWith('"'))
	{
		result = result.mid(0, result.size() - 1);
	}

	return result;
}

inline QString MetaData::toSlkString(const QString &value)
{
	QString result = value;

	if (!result.startsWith('"'))
	{
		result.prepend('"');
	}

	if (!result.endsWith('"'))
	{
		result.append('"');
	}

	return result;
}

inline QString MetaData::fromFilePath(const QString &value)
{
	QString result = value;
	result.replace('\\', '/');

	return result;
}

}

}

#endif
